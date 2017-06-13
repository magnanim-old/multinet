#include "community/lart.h"
#include <unsupported/Eigen/MatrixFunctions>
#include <math.h>

#include <dlib/graph.h>
#include <dlib/graph_utils.h>

#include <algorithm>


#define CONNECT_LAST_THRESHOLD 100

namespace mlnet {

vector<unsigned int> lart::get_partition(vector<dlib::bu_cluster> clusters, int maxmodix, size_t L, size_t N) {

	struct partition {
		std::vector<long> vals;
	};

	vector<partition> parts;
	partition p;
	p.vals.resize(L * N);
	std::iota (std::begin(p.vals), std::end(p.vals), 0);

	parts.push_back(p);

	for (size_t i = L * N; i < L * N + maxmodix; i++) {
		vector<long> tmp = {clusters[i].left, clusters[i].right};
		vector<long> out;

		std::set_symmetric_difference (
			parts[i-N*L].vals.begin(), parts[i-N*L].vals.end(),
			tmp.begin(), tmp.end(),
			std::back_inserter(out));

		partition p;
		p.vals = out;
		parts.push_back(p);
	}

	vector<partition> r;
	vector<long> val = parts[parts.size() - 1].vals;

	for (size_t i = 0; i < val.size(); i++) {
		partition pp;
		pp.vals = clusters[val[i]].orig;
		r.push_back(pp);
	}

	size_t l = r.size();
	size_t n = L * N;

	vector<unsigned int> result(n);
	for (size_t i = 0; i < l; i++) {
		for (size_t j = 0; j < r[i].vals.size(); j++) {
			result[r[i].vals[j]] = (unsigned int)i;
		}
	}


	return result;
}


vector<double> lart::modMLPX(vector<dlib::bu_cluster> clusters, std::vector<Eigen::SparseMatrix<double>> a, double gamma) {
	Eigen::SparseMatrix<double> sA0 = cutils::supraA(a, 0, false, false);
	vector<double> r;

	size_t L = a.size();
	size_t N = a[0].rows();

	double twoum = 0.0;

	sA0 = cutils::ng_modularity(twoum, a, gamma, 0);

	double diag = 0.0;
	for (int i = 0; i < sA0.rows(); i++){
		diag += sA0.coeff(i, i);
	}

	r.push_back(diag);

	for (size_t i = N * L; i < clusters.size(); i++) {
		dlib::bu_cluster data = clusters[i];

		vector<long> v1 = clusters[data.left].orig;
		vector<long> v2 = clusters[data.right].orig;
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

Eigen::MatrixXd lart::pairwise_distance(Eigen::MatrixXd X, Eigen::MatrixXd Y, bool same_obj) {
	Eigen::MatrixXd XX = (X.array() * X.array()).rowwise().sum();
	Eigen::MatrixXd YY = (Y.array() * Y.array()).rowwise().sum().transpose();
	Eigen::MatrixXd distances = (X * Y.transpose()).unaryExpr([](const double x) { return x * -2;});

	for (int i = 0; i < distances.rows(); i++) {
		distances.col(i).array() += XX.array();
		distances.row(i).array() += YY.array();

		if(same_obj) {
			distances(i, i) = 0;
		}
	}

	return distances.array().unaryExpr([](const double x) { return x < 0 ? 0 : x;}).sqrt();
}

Eigen::MatrixXd lart::Dmat(Eigen::SparseMatrix<double> Pt, Eigen::SparseMatrix<double> dA, size_t L) {
	Eigen::VectorXd diag = dA.diagonal();
	std::vector<Eigen::Triplet<double>> tlist;
	tlist.reserve(diag.rows());

	for (int i = 0; i < diag.rows(); i++) {
		tlist.push_back(Eigen::Triplet<double>(i, i, std::sqrt(diag(i, 0))));
	}

	dA.setFromTriplets(tlist.begin(), tlist.end());
	Eigen::SparseMatrix<double> newP = Pt * dA;

	size_t N = Pt.rows() / L;
	Eigen::MatrixXd Dmat = Eigen::MatrixXd::Zero(N * L, N * L);

	for (size_t i = 0; i < L; ++i) {
		Eigen::MatrixXd X = Eigen::MatrixXd(newP.block(i * N, 0, N, N * L));
		Eigen::MatrixXd m = pairwise_distance(X, X, true);

		for (int j = 0; j < m.rows(); j++) {
			for (int k = 0; k < m.cols(); k++) {
				Dmat(j + (i * N), k + (i * N)) = m(j, k);
			}
		}
	}

	std::vector<size_t> l(L);
	std::iota(std::begin(l), std::end(l), 0);

	for (size_t i = 0; i  < L - 1; ++i) {
		for (size_t j = i + 1; j < L; ++j) {
			if (i != j) {
				std::vector<size_t> diff;
				std::vector<size_t> tmp = {i, j};

				std::set_symmetric_difference(
				l.begin(),
				l.end(),
				tmp.begin(),
				tmp.end(),
				std::back_inserter(diff));

				Eigen::MatrixXd newx = newP.block(i * N, i * N, N, N);
				Eigen::MatrixXd newy = newP.block(j * N, j * N, N, N);

				Eigen::MatrixXd m1(newx.rows(), newx.cols() * 2);
				Eigen::MatrixXd m2(newy.rows(), newy.cols() * 2);

				m1 << newx,newy;
				m2 << newy,newx;

				for (size_t k = 0; k < diff.size(); k++) {
					newx = newP.block(N * i, diff[k] * N, N, N);
					newy = newP.block(N * j, diff[k] * N, N, N);

					Eigen::MatrixXd mx(m1.rows(), m1.cols()+newx.cols());
					Eigen::MatrixXd my(m2.rows(), m2.cols()+newy.cols());

					mx << m1, newx;
					my << m2, newy;

					m1 = mx;
					m2 = my;

				}

				Eigen::MatrixXd dmat = pairwise_distance(m1, m2, false);
				Dmat.block(i * N, j * N, N, N) = dmat;
				Dmat.block(j * N, i * N, N, N) = dmat.transpose();
			}
		}
	}

	return Dmat;
}

Eigen::SparseMatrix<double> lart::diagA(Eigen::SparseMatrix<double> A) {
	Eigen::SparseMatrix<double> dA = Eigen::SparseMatrix<double>(A.rows(), A.cols());
	Eigen::MatrixXd d = cutils::sparse_sum(A, 1);

	std::vector<Eigen::Triplet<double>> tlist;
	tlist.reserve(A.nonZeros() + A.rows());

	for (int k = 0; k < A.rows(); k++) {
		tlist.push_back(Eigen::Triplet<double>(k, k, 1 / d(k, 0)));
	}
	dA.setFromTriplets(tlist.begin(), tlist.end());
	return dA;
}

unsigned long lart::prcheck(Eigen::SparseMatrix<double>& aP, std::vector<dlib::sample_pair> edges, unsigned int LN) {
	std::vector<unsigned long> memb;
	unsigned long num_clusters = dlib::newman_cluster(edges, memb);

	std::vector<unsigned long> uq = memb;
	std::sort(uq.begin(), uq.end());
	std::vector<unsigned long>::iterator it;
	it = std::unique(uq.begin(), uq.end());
	uq.resize(std::distance(uq.begin(), it));

	std::random_device seed;
	std::mt19937 engine(seed());
	std::uniform_int_distribution<int> choose(0, memb.size() - 1);

	std::vector<Eigen::Triplet<double>> tlist;
	tlist.reserve(aP.nonZeros());

	for (int k = 0; k < aP.outerSize(); k++) {
		for (Eigen::SparseMatrix<double>::InnerIterator it(aP, k); it; ++it) {
			tlist.push_back(Eigen::Triplet<double>(it.row(), it.col(), it.value()));
		}
	}

	for (size_t i = 0; i < uq.size(); i++) {
		int index = choose(engine);
		for (int j = 0; j < aP.rows(); j++) {
			tlist.push_back(Eigen::Triplet<double>(index, j, 0.85 * aP.coeff(index, j) + 0.15 / (LN)));
		}
	}

	aP.setFromTriplets(tlist.begin(), tlist.end(), [] (const double, const double &b) { return b; });
	return num_clusters;
}

int lart::is_connected(std::vector<Eigen::SparseMatrix<double>> a, std::vector<dlib::sample_pair>& edges) {
	typedef dlib::matrix<double,2,1> node_vector_type;
	typedef dlib::matrix<double,1,1> edge_vector_type;
	typedef dlib::graph<node_vector_type, edge_vector_type>::kernel_1a_c graph_type;

	Eigen::SparseMatrix<double> aplus = Eigen::SparseMatrix<double>(a[0].rows(), a[0].cols());
	for (size_t i = 0; i < a.size(); i++) {
		aplus += a[i];
	}
	graph_type g;
	g.set_number_of_nodes(a[0].rows());

	Eigen::SparseMatrix<double> t = aplus.triangularView<Eigen::Upper>();
	for (int j = 0; j < t.outerSize(); j++) {
		for (Eigen::SparseMatrix<double>::InnerIterator it(t, j); it; ++it) {
			edges.push_back(dlib::sample_pair(it.row(), it.col(), 1));
			g.add_edge(it.row(), it.col());
		}
	}

	return dlib::graph_is_connected(g);
}

std::vector<unsigned long> lart::find_ix(std::vector<unsigned long> x, unsigned long y) {
	std::vector<unsigned long> ix;
	std::vector<unsigned long>::iterator iter = x.begin();
	while ((iter = std::find_if(iter, x.end(), [&y] (unsigned long o) { return o == y; })) != x.end()) {
		ix.push_back(iter - x.begin());
		iter++;
	}
	return ix;
}

void lart::updateDt(Eigen::MatrixXd& Dt, std::vector<Eigen::SparseMatrix<double>> a) {
	Eigen::SparseMatrix<double> g = cutils::supraA(a, 0, false, false);

	std::vector<unsigned long> memb;
	std::vector<dlib::sample_pair> edges;
	for (int j = 0; j < g.outerSize(); j++) {
		for (Eigen::SparseMatrix<double>::InnerIterator it(g, j); it; ++it) {
			edges.push_back(dlib::sample_pair(it.row(), it.col(), 1));
		}
	}

	dlib::newman_cluster(edges, memb);

	std::vector<unsigned long> n_comp = memb;
	std::sort(n_comp.begin(), n_comp.end());
	std::vector<unsigned long>::iterator it;
	it = std::unique(n_comp.begin(), n_comp.end());
	n_comp.resize(std::distance(n_comp.begin(), it));

	for (size_t i = 0; i < n_comp.size() - 1; i++) {
		for (size_t j = i + 1; j < n_comp.size(); j++) {
			if (i != j) {
				std::vector<unsigned long> row = find_ix(memb, n_comp[i]);
				std::vector<unsigned long> col = find_ix(memb, n_comp[j]);

				for (size_t k = 0; k < row.size(); k++) {
					for (size_t o = 0; o < col.size(); o++) {
						Dt(row[k], col[o]) = CONNECT_LAST_THRESHOLD;
						Dt(col[o], row[k]) = CONNECT_LAST_THRESHOLD;
					}
				}
			}
		}
	}

}

void lart::exp_by_squaring_iterative(Eigen::SparseMatrix<double>& x, int t) {
	Eigen::MatrixXd y = Eigen::MatrixXd(x);
	switch(t) {
		case 0 : {
				std::vector<Eigen::Triplet<double>> tlist;
				tlist.reserve(x.rows());
				for (int i = 0; i < x.rows(); i++) {
					tlist.push_back(Eigen::Triplet<double>(i, i, 1.0));
				}
				x.setFromTriplets(tlist.begin(), tlist.end());
				return;
		}
		case 1 : { return; }
		case 2 : {
			y = y * y;
			x = y.sparseView();
			return;
		}
		case 3 : {
			y = y * y * y;
			x = y.sparseView();
			return;
		}
		default : {
			Eigen::MatrixXd z = Eigen::MatrixXd(x);
			bool flag = true;
			while (t != 0) {
				if (t % 2) {
					if (flag) {
						y = z;
						flag = false;
					} else {
						y = y * z;
					}
				}

				z = z * z;
				t = t >> 1;
			}
			x = y.sparseView();
			return;
		}
	}
}

CommunityStructureSharedPtr lart::fit(
	MLNetworkSharedPtr mnet, int t, double eps, double gamma) {
	DTRACE4(LART_START, mnet->name.c_str(), t, std::to_string(eps), std::to_string(gamma));

	std::vector<Eigen::SparseMatrix<double>> a = cutils::ml_network2adj_matrix(mnet);

	std::vector<dlib::sample_pair> edges;
	int connected = is_connected(a, edges);

	Eigen::SparseMatrix<double> sA = cutils::supraA(a, eps, true, true);
	Eigen::SparseMatrix<double> dA = diagA(sA);
	Eigen::SparseMatrix<double> aP = dA * sA;
	sA.resize(0,0);

	if (!connected) {
		prcheck(aP, edges, a.size() * a[0].rows());
	}

	DTRACE0(LART_WALK_START);
	exp_by_squaring_iterative(aP, t);
	DTRACE0(LART_WALK_END);

	Eigen::MatrixXd Dt = Dmat(aP, dA, a.size());

	if (!connected) {
		updateDt(Dt, a);
	}

	std::vector<unsigned long> labels;
	std::vector<dlib::bu_cluster> clusters(Dt.rows());
	DTRACE0(LART_AC_START);
	dlib::bottom_up_cluster(dlib::mat(Dt), labels, 2, 100, clusters);
	DTRACE0(LART_AC_END);
	Dt.resize(0,0);

	vector<double> mod = modMLPX(clusters, a, gamma);

	auto maxmod = std::max_element(std::begin(mod), std::end(mod));
	int maxmodix = std::distance(std::begin(mod), maxmod);

	vector<unsigned int> partition = get_partition(clusters, maxmodix, a.size(), a[0].rows());

	DTRACE0(LART_END);
	return cutils::nodes2communities(mnet, partition);
}

}
