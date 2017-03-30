#include "community/lart.h"

namespace mlnet {

CommunitiesSharedPtr lart::get_partition(MLNetworkSharedPtr mnet, vector<lart::cluster> clusters, int maxmodix, size_t L, size_t N) {

	struct partition {
		std::vector<int> vals;
	};

	vector<partition> parts;
	partition p;
	p.vals.resize(L * N);
	std::iota (std::begin(p.vals), std::end(p.vals), 0);
	parts.push_back(p);

	for (size_t i = L * N; i < L * N + maxmodix; i++) {
		vector<int> tmp = {clusters[i].left, clusters[i].right};
		vector<int> out;

		std::set_symmetric_difference (
			parts[i-N*L].vals.begin(), parts[i-N*L].vals.end(),
			tmp.begin(), tmp.end(),
			std::back_inserter(out));

		out.push_back(i);
		partition p;
		p.vals = out;
		parts.push_back(p);
	}


	vector<partition> r;
	vector<int> val = parts[parts.size() - 1].vals;

	for (size_t i = 0; i < val.size(); i++) {
		partition pp;
		pp.vals = clusters[val[i]].orig;
		r.push_back(pp);
	}

	size_t l = r.size();
	size_t n = L * N;


	// matrix nodeid2cid
	vector<int> result(n);
	for (size_t i = 0; i < l; i++) {
		for (size_t j = 0; j < r[i].vals.size(); j++) {
			result[r[i].vals[j]] = i;
		}
	}

	// aid2cid
	vector<vector<int>> layered(L);
	for (size_t i = 0; i < L; i++) {
		vector<int> v;
		for (size_t j = i * N; j < (1 + i) * N; j++) {
			v.push_back(result[j]);
		}
		layered[i] = v;
	}

	// cid2aid
	std::map<int, vector<int>> cid2aid;
	for (size_t i = 0; i < layered.size(); i++) {
		for (size_t j = 0; j < layered[i].size(); j++) {
			cid2aid[layered[i][j]].push_back(j);

		}
	}

	// actual nodeid 2 communities
	CommunitiesSharedPtr communities = communities::create();
	for(std::map<int,std::vector<int>>::iterator iter = cid2aid.begin(); iter != cid2aid.end(); ++iter) {
		CommunitySharedPtr c = community::create();
		for (size_t i = 0; i < iter->second.size(); i++) {
			for (NodeSharedPtr n : *mnet->get_nodes(((mnet->get_actors()->get_at_index(iter->second[i]))))) {
				(*c).add_node(n);
			} 
		}
		(*communities).add_community(c);
	}

	return communities;
}

vector<double> lart::modMLPX(vector<lart::cluster> clusters, std::vector<Eigen::SparseMatrix<double>> a, double gamma, Eigen::SparseMatrix<double> sA0) {
	vector<double> r;

	size_t L = a.size();
	size_t N = a[0].rows();

	cutils::modmat(a, gamma, 0, sA0);

	double diag = 0.0;
	for (int i = 0; i < sA0.rows(); i++){
		diag += sA0.coeff(i, i);
	}

	r.push_back(diag);

	for (size_t i = N * L; i < clusters.size(); i++) {
		cluster data = clusters[i];

		vector<int> v1 = clusters[data.left].orig;
		vector<int> v2 = clusters[data.right].orig;
		double tmp = 0.0;

		for (size_t i = 0; i < v1.size(); i++) {
			for (size_t j = 0; j < v2.size(); j++) {
				tmp += sA0.coeff(v1[i], v2[j]);
			}
		}

		tmp *= 2;
		r.push_back(r[r.size() - 1] + tmp);
	}

	return r;
}

void lart::average_linkage(Eigen::SparseMatrix<double>& Dt, std::vector<lart::cluster> clusters, lart::dist d) {
	size_t x = clusters[d.row].orig.size();
	size_t y = clusters[d.col].orig.size();

	for (int i = 0; i < Dt.rows(); i++) {
		Dt.coeffRef(d.row, i) = ((Dt.coeff(d.row, i) * x) + (Dt.coeff(i, d.col) * y)) / (x + y);
		Dt.coeffRef(i, d.row) = Dt.coeff(d.row, i);
		Dt.coeffRef(i, i) = 0;
	}
}

lart::dist lart::find_dist(Eigen::SparseMatrix<double> Dt, std::vector<std::vector<int>> merges, Eigen::SparseMatrix<double> sA) {

	lart::dist d;
	d.row = -1;
	d.col = -1;
	d.val = 1000; // anything above 1 is ok actually

	/*	Iterate the top right corner of the distance matrix created in matrix power
		to find the smallest distance between nodes OR clusters. Only nodes or clusters
		that are connected (connection matrix is sA) are allowed to merge.
	*/
	Eigen::SparseMatrix<double> t = Dt.triangularView<Eigen::Upper>();
	for (int i = 0; i < t.outerSize(); i++) {
		for (Eigen::SparseMatrix<double>::InnerIterator it(t, i); it; ++it) {
			double tmp = Dt.coeff(it.row(), it.col());
			if (tmp < d.val && tmp > 0) {
				for (size_t j = 0; j < merges[it.row()].size(); j++) {
					for (size_t k = 0; k < merges[it.col()].size(); k++) {
						if (sA.coeff(merges[it.row()][j], merges[it.col()][k]) > 0){
							d.row = it.row();
							d.col = it.col();
							d.val = tmp;

							/* We found a valid connection (but maybe not the smallest distance)
								Stop searching for connection and look at more values
							 */
							j = merges[it.row()].size();
							k = merges[it.col()].size();
							break;
						}
					}
				}
			}

		}
	}

	DTRACE3(FIND_DIST_END, d.row, d.col, std::to_string(d.val));
	return d;
}

std::vector<lart::cluster> lart::AgglomerativeClustering(Eigen::SparseMatrix<double> Dt, Eigen::SparseMatrix<double> sA) {
	// TODO Create a parameter for different linkage function and make it more abstract
	std::vector<lart::cluster> clusters(Dt.rows());
	for (int i = 0; i < Dt.rows(); i++) {
		cluster c;
		c.left = -1;
		c.right = -1;
		c.id = i;
		c.orig.push_back(i);
		clusters[i] = c;
	}

	// This matrix gets updated every iteration
	Eigen::SparseMatrix<double> tmp(Dt);

	// Used to track merges
	std::vector<std::vector<int>> merges (Dt.rows());
	for (size_t i = 0; i < merges.size(); i++) {
		std::vector<int> v = {(int)i};
		merges[i] = v;
	}

	// Used to track node id's and merges. TODO: Can we remove this?
	std::vector<int> labels (Dt.rows());
	std::iota (std::begin(labels), std::end(labels), 0);


	for (int i = 0; i < Dt.rows() - 1; i++) {
		// Loop invariant: All nodes except two have been merged
		lart::dist d = find_dist(tmp, merges, sA);

		if (d.row < 0) {
			// No more connected components were found
			break;
		}

		/* Add a new cluster into the mix with its history.
			Keep the orde */
		cluster c;
		c.id = Dt.rows() + i;
		if (labels[d.row] < labels[d.col]) {
			c.left = clusters[labels[d.row]].id;
			c.right = clusters[labels[d.col]].id;
		} else {
			c.left = clusters[labels[d.col]].id;
			c.right = clusters[labels[d.row]].id;
		}

		/* Merge its history */
		c.orig.insert(c.orig.begin(), clusters[labels[d.row]].orig.begin(), clusters[labels[d.row]].orig.end());
		c.orig.insert(c.orig.end(), clusters[labels[d.col]].orig.begin(), clusters[labels[d.col]].orig.end());

		clusters.push_back(c);

		/* Update the distance matrix */
		average_linkage(tmp, clusters, d);
		/* Remove the merged node from the distance matrix
			TODO: Improve the speed.
		*/
		DTRACE0(AGGLO_PRUNE_START);
		tmp.prune([&d](int i, int j, double ) { return i!=d.col && j!=d.col; });
		DTRACE0(AGGLO_PRUNE_END);

		/* Update labels and history merge. If 0 merges with 10, we remove 10 from the pool and merge with 0 and it becomes cluster number 11 */
		merges[d.row].push_back(d.col);
		labels[d.row] = Dt.rows() + i;
	}

	DTRACE1(AGGLO_END, clusters.size());
	return clusters;
}

// TODO: Parallelize this function
Eigen::MatrixXd lart::pairwise_distance(Eigen::MatrixXd X, Eigen::MatrixXd Y) {
	DTRACE2(PDISTANCE_START, X.rows(), Y.rows());

	Eigen::MatrixXd XX = (X.array() * X.array()).rowwise().sum();
	Eigen::MatrixXd YY = (Y.array() * Y.array()).rowwise().sum().transpose();
	Eigen::MatrixXd distances = (X * Y.transpose()).unaryExpr([](const double x) { return x * -2;});

	for (int i = 0; i < distances.rows(); i++) {
		distances.col(i).array() += XX.array();
		distances.row(i).array() += YY.array();
		distances(i, i) = 0.0;
	}

	return distances.array().sqrt();
}

Eigen::MatrixXd lart::Dmat(Eigen::SparseMatrix<double> Pt, Eigen::SparseMatrix<double> dA, size_t L) {
	// NOTE: dA side effect
	for (int j = 0; j < dA.outerSize(); j++) {
		for (Eigen::SparseMatrix<double>::InnerIterator it(dA, j); it; ++it) {
			dA.coeffRef(it.row(), it.col()) = std::sqrt(it.value());
		}
	}

	Eigen::SparseMatrix<double> newP = Pt * dA;

	size_t N = Pt.rows() / L;
	Eigen::MatrixXd Dmat = Eigen::MatrixXd::Zero(N * L, N * L);

	for (size_t i = 0; i < L; ++i) {
		Eigen::MatrixXd X = Eigen::MatrixXd(newP.block(i * N, 0, N, N * L));
		Eigen::MatrixXd m = pairwise_distance(X, X);

		for (int j = 0; j < m.rows(); j++) {
			for (int k = 0; k < m.cols(); k++) {
				Dmat(j + (i * N), k + (i * N)) = m(j, k);
			}
		}
	}

	for (size_t i = 0; i  < L - 1; ++i) {
		for (size_t j = i + 1; j < L; ++j) {
			if (i != j) {
				Eigen::MatrixXd newx = newP.block(i * N, i * N, N, N);
				Eigen::MatrixXd newy = newP.block(j * N, j * N, N, N);

				Eigen::MatrixXd tnewx = newP.block(j * N, i * N, N, N);
				Eigen::MatrixXd tnewy = newP.block(i * N, j * N, N, N);

				Eigen::MatrixXd m1(newx.rows(), newx.cols()+tnewy.cols());
				Eigen::MatrixXd m2(newy.rows(), newy.cols()+tnewx.cols());

				m1 << newx,tnewy;
				m2 << newy,tnewx;

				Eigen::MatrixXd dmat = pairwise_distance(m1, m2);
				Dmat.block(i * N, (i+1)*N, N, N) = dmat;
				Dmat.block((i+1)*N, i * N, N, N) = dmat.transpose();
			}
		}
	}
	return Dmat;
}

Eigen::SparseMatrix<double> lart::matrix_power(Eigen::SparseMatrix<double> m, uint32_t t) {
	if (t == 0) {
		return Eigen::MatrixXd::Identity(m.rows(), m.cols()).sparseView();
	}

	Eigen::SparseMatrix<double> Dt(m);
	for (uint32_t i = 1; i < t; i++) {
		// TODO Write more and consider the pruning
		Dt = Dt * m;//.pruned(0.001, 10);
		//Dt.makeCompressed();
	}
	return Dt;
}

Eigen::SparseMatrix<double> lart::diagA(Eigen::SparseMatrix<double> A) {
	Eigen::SparseMatrix<double> dA = Eigen::SparseMatrix<double>(A.rows(), A.cols());
	dA.reserve(Eigen::VectorXi::Constant(A.rows() / 2, A.rows() / 2));

	DTRACE2(DIAGA_RESERVE, A.rows(), A.rows() / 2);

	Eigen::MatrixXd d = sum(A, 1);

	std::vector<Eigen::Triplet<double>> tlist;
	tlist.reserve(A.rows());

	for (int k = 0; k < A.rows(); k++) {
		tlist.push_back(Eigen::Triplet<double>(k, k, 1 / d(k, 0)));
	}
	dA.setFromTriplets(tlist.begin(), tlist.end());
	return dA;
}

Eigen::MatrixXd lart::sum(Eigen::SparseMatrix<double> X, int axis) {
	Eigen::MatrixXd d = Eigen::MatrixXd::Zero(X.rows(), 1);
	for (int i = 0; i < X.outerSize(); i++) {
		for (Eigen::SparseMatrix<double>::InnerIterator it(X, i); it; ++it) {
			if (axis){
				d(it.col(), 0) = it.value() + d(it.col(), 0);
			} else {
				d(it.row(), 0) = it.value() + d(it.row(), 0);
			}
		}
	}
	return d;
}

Eigen::SparseMatrix<double> lart::block_diag(std::vector<Eigen::SparseMatrix<double>> a) {
	Eigen::SparseMatrix<double> m = Eigen::SparseMatrix<double>(
		a[0].rows() * a.size(), a[0].cols() * a.size());

	m.reserve(Eigen::VectorXi::Constant(a[0].rows(), a[0].rows()));
	DTRACE2(BLOCK_DIAG_RESERVE, a[0].rows() * a.size(), a[0].rows());

	size_t r, c;
	r = 0;
	c = 0;

	std::vector<Eigen::Triplet<double>> tlist;
	tlist.reserve(a[0].rows() * a.size());

	for (size_t i = 0; i < a.size(); i++) {
		for (int j = 0; j < a[i].outerSize(); j++) {
			for (Eigen::SparseMatrix<double>::InnerIterator it(a[i], j); it; ++it) {
				tlist.push_back(Eigen::Triplet<double>(r + it.row(), c + it.col(), it.value()));
			}
		}
		r += a[i].rows();
		c += a[i].cols();
	}
	m.setFromTriplets(tlist.begin(), tlist.end());
	return m;
}


CommunitiesSharedPtr lart::get_ml_community(
	MLNetworkSharedPtr mnet, uint32_t t, double eps, double gamma) {

	std::vector<Eigen::SparseMatrix<double>> a = cutils::ml_network2adj_matrix(mnet);

	Eigen::SparseMatrix<double> sA = cutils::supraA(a, eps);
	Eigen::SparseMatrix<double> sA0 = cutils::supraA(a, 0);
	sA0.prune(0, 0); // Used in clustering, don't want to see any zero's

	Eigen::SparseMatrix<double> dA = diagA(sA);
	Eigen::SparseMatrix<double> aP = dA * sA;
	Eigen::SparseMatrix<double> Pt = matrix_power(aP, t);
	Eigen::MatrixXd Dt = Dmat(Pt, dA, a.size());
	std::vector<lart::cluster> clusters = AgglomerativeClustering(Dt.sparseView(), sA0);

	vector<double> mod = modMLPX(clusters, a, gamma, sA0);
	auto maxmod = std::max_element(std::begin(mod), std::end(mod));
	int maxmodix = std::distance(std::begin(mod), maxmod);

	CommunitiesSharedPtr c = get_partition(mnet, clusters, maxmodix, a.size(), a[0].rows());
	return c;

}

}
