#include "community/cutils.h"

namespace mlnet {

Eigen::SparseMatrix<double> cutils::ng_modularity(double &twoum, std::vector<Eigen::SparseMatrix<double>> a, double gamma, double omega) {
	size_t L = a.size();
	size_t N = a[0].rows();

	Eigen::SparseMatrix<double> B(N * L, N * L);
	B.reserve(Eigen::VectorXi::Constant(N * L, N * L));

	twoum = 0.0;

	std::vector<Eigen::Triplet<double>> tlist;
	tlist.reserve(N * L);

	for (size_t i = 0; i < L; i++) {
		Eigen::MatrixXd kout = cutils::sparse_sum(a[i], 0);
		Eigen::MatrixXd kin = cutils::sparse_sum(a[i], 1);
		double mm = kout.array().sum();
		twoum = twoum + mm;

		Eigen::SparseMatrix<double> tmp, tmp1;
		tmp = (Eigen::SparseMatrix<double>(a[i].transpose()) + a[i]) / 2;

		Eigen::MatrixXd tmp2 = kin * kout.transpose();
		Eigen::MatrixXd tmp3 = gamma / 2 * (tmp2 + tmp2)/ mm;

		tmp = tmp - tmp3.sparseView();

		for (size_t j = 0; j < N; j++) {
			for (size_t k = 0; k < N; k++) {
				tlist.push_back(
					Eigen::Triplet<double>(j + (i * N), k + (i * N), tmp.coeff(j, k)));
			}
		}
	}

	for (size_t i = 0; i  < L - 1; ++i) {
		for (size_t j = i + 1; j < L; ++j) {
			int ix_i = i * N;
			int ix_j = j * N;

			for (int k = 0; k < a[i].rows(); k++) {
				tlist.push_back(Eigen::Triplet<double>((ix_i + k), (ix_j + k), omega));
				tlist.push_back(Eigen::Triplet<double>((ix_j + k), (ix_i + k), omega));
			}
		}
	}
	B.setFromTriplets(tlist.begin(), tlist.end());

	twoum = twoum + (N * L * (L - 1) * omega);
	return B;
}

CommunityStructureSharedPtr cutils::nodes2communities(MLNetworkSharedPtr mnet, std::vector<unsigned int> nodes2cid) {
	size_t L = mnet->get_layers()->size();
	size_t N = mnet->get_actors()->size();

	vector<vector<int>> layered(L);
	for (size_t i = 0; i < L; i++) {
		vector<int> v;
		for (size_t j = i * N; j < (1 + i) * N; j++) {
			v.push_back(nodes2cid[j]);
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
	CommunityStructureSharedPtr communities = community_structure::create();
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

CommunityStructureSharedPtr cutils::actors2communities(MLNetworkSharedPtr mnet, std::vector<unsigned int> actors2cid) {
	CommunityStructureSharedPtr communities = community_structure::create();

	for (size_t i = 0; i < actors2cid.size(); i++) {
		std::vector<CommunitySharedPtr> cptr = (*communities).get_communities();

		if (actors2cid[i] < cptr.size()) {
			CommunitySharedPtr c = (*communities).get_community(actors2cid[i]);
			for (NodeSharedPtr n : *mnet->get_nodes(((mnet->get_actors()->get_at_index(i))))) {
				(*c).add_node(n);
			}
		} else {
			CommunitySharedPtr c = community::create();
			for (NodeSharedPtr n : *mnet->get_nodes(((mnet->get_actors()->get_at_index(i))))) {
				(*c).add_node(n);
			}
			(*communities).add_community(c);
		}
	}

	return communities;
}

Eigen::MatrixXd cutils::sparse_sum(Eigen::SparseMatrix<double> X, int axis) {
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

std::vector<Eigen::SparseMatrix<double>> cutils::ml_network2adj_matrix(MLNetworkSharedPtr mnet) {
	size_t L = mnet->get_layers()->size();
	size_t N = mnet->get_actors()->size();

	std::vector<Eigen::SparseMatrix<double>> a(L);

	for (LayerSharedPtr l: *mnet->get_layers()) {
		Eigen::SparseMatrix<double> m = Eigen::SparseMatrix<double> (N, N);

		std::vector<Eigen::Triplet<double>> tlist;
		tlist.reserve(mnet->get_edges()->size());

		for (EdgeSharedPtr e: *mnet->get_edges(l, l)) {
			int v1_id = e->v1->actor->id;
			int v2_id = e->v2->actor->id;
			tlist.push_back(Eigen::Triplet<double>(v1_id - 1, v2_id - 1, 1));

			if (!(e->directionality)) {
				tlist.push_back(Eigen::Triplet<double>(v2_id - 1, v1_id - 1, 1));
			}

		}

		m.setFromTriplets(tlist.begin(), tlist.end());
		a[l->id - 1] = m;
	}
	return a;
}

std::vector<int> cutils::unique(std::vector<int> y) {
	std::sort(y.begin(), y.end());
	std::vector<int>::iterator it;
	it = std::unique(y.begin(), y.end());
	y.resize(std::distance(y.begin(), it));
	return y;
}

std::vector<int> cutils::range(size_t size, bool randomize) {
	std::vector<int> range(size);
	std::iota(range.begin(), range.end(), 0);
	if (randomize) {
		std::random_shuffle (range.begin(), range.end());
	}
	return range;
}

}
