#include "community/cutils.h"

namespace mlnet {

CommunitiesSharedPtr cutils::nodes2communities(MLNetworkSharedPtr mnet, std::vector<int> nodes2cid) {
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


Eigen::MatrixXd cutils::sum(Eigen::SparseMatrix<double> X, int axis) {
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

Eigen::SparseMatrix<double> cutils::block_diag(std::vector<Eigen::SparseMatrix<double>> a) {
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


Eigen::SparseMatrix<double> cutils::supraA(std::vector<Eigen::SparseMatrix<double>> a, double eps) {
	Eigen::SparseMatrix<double> A = block_diag(a);
	size_t L = a.size();
	size_t N = a[0].rows();

	for (size_t i = 0; i  < L - 1; ++i) {
		for (size_t j = i + 1; j < L; ++j) {
			Eigen::MatrixXd d = sum(a[i].cwiseProduct(a[j]), 1);

			std::vector<Eigen::Triplet<double>> tlist;
			tlist.reserve(a[i].rows());

			for (int k = 0; k < A.outerSize(); k++) {
				for (Eigen::SparseMatrix<double>::InnerIterator it(A, k); it; ++it) {
					tlist.push_back(Eigen::Triplet<double>(it.row(), it.col(), it.value()));
				}
			}
			int ix_a = i * N;
			int ix_b = (i + 1) * N;

			for (int k = 0; k < a[i].rows(); k++) {
				double intra = d(k, 0) + eps;
				tlist.push_back(Eigen::Triplet<double>(ix_a + k, ix_b + k, intra));
				tlist.push_back(Eigen::Triplet<double>(ix_b + k, ix_a + k, intra));
			}

			for (int k = 0; k < A.rows(); k++) {
				tlist.push_back(Eigen::Triplet<double>(k, k, eps));
			}
			A.setFromTriplets(tlist.begin(), tlist.end());
		}
	}
	return A;
}

std::vector<Eigen::SparseMatrix<double>> cutils::ml_network2adj_matrix(MLNetworkSharedPtr mnet) {
	size_t L = mnet->get_layers()->size();
	size_t N = mnet->get_actors()->size();

	std::vector<Eigen::SparseMatrix<double>> adj(L);

	for (LayerSharedPtr l: *mnet->get_layers()) {
		Eigen::SparseMatrix<double> m = Eigen::SparseMatrix<double> (N, N);
		size_t reserve = N /2;
		m.reserve(Eigen::VectorXi::Constant(reserve, reserve));

		DTRACE2(ML2AM_RESERVE, N, reserve);

		for (EdgeSharedPtr e: *mnet->get_edges(l, l)) {
			int v1_id = e->v1->actor->id;
			int v2_id = e->v2->actor->id;
			m.insert(v1_id - 1, v2_id - 1) = 1;
			m.insert(v2_id - 1, v1_id - 1) = 1;
		}
		adj[l->id - 1] = m;
	}

	DTRACE2(ML2AM_END, L, N);
	return adj;
}



}
