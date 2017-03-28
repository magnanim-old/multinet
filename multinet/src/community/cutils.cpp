#include "community/cutils.h"

namespace mlnet {

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

void cutils::modmat(std::vector<Eigen::SparseMatrix<double>> a,
	double gamma, double omega, Eigen::SparseMatrix<double>& sA) {

	double twoum = 0.0;
	for (int j = 0; j < sA.outerSize(); j++) {
		for (Eigen::SparseMatrix<double>::InnerIterator it(sA, j); it; ++it) {
			twoum += it.value();
		}
	}

	size_t L = a.size();
	size_t N = a[0].rows();


	Eigen::SparseMatrix<double> copy (sA);

	std::vector<Eigen::Triplet<double>> tlist;
	tlist.reserve(copy.rows() * 1.5);

	// Cache the intra layer weights because they don't change
	if (omega == 0) {
		for (size_t i = 0; i < N; i++) {
			tlist.push_back(Eigen::Triplet<double>(i, N + i, sA.coeff(i, N + i)));
			tlist.push_back(Eigen::Triplet<double>(N + i, i, sA.coeff(N + i, i)));
		}
	}

	for (size_t i = 0; i < L; i++) {
		Eigen::MatrixXd d = sum(a[i], 0);

		Eigen::MatrixXd	product = d * d.transpose();

		double asum = 0;
		for (int j = 0; j < a[i].outerSize(); j++) {
			for (Eigen::SparseMatrix<double>::InnerIterator it(a[i], j); it; ++it) {
				asum += it.value();
			}
		}

		Eigen::MatrixXd	s1 = product.array() / asum;
		Eigen::MatrixXd	s2 = s1.array() * gamma;
		Eigen::MatrixXd s3 = Eigen::MatrixXd(copy.block(i * N, i * N, N, N)) - s2;

		for (int j = 0; j < s3.rows(); j++) {
			for (int k = 0; k < s3.cols(); k++) {
				tlist.push_back(Eigen::Triplet<double>(j + (i * N), k + (i * N), s3(j, k)));
			}
		}
	}

	sA.setFromTriplets(tlist.begin(), tlist.end());
	sA /= twoum;
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
	size_t N = mnet->get_layers()->size();
	size_t M = mnet->get_actors()->size();

	std::vector<Eigen::SparseMatrix<double>> adj(N);

	for (LayerSharedPtr l: *mnet->get_layers()) {
		Eigen::SparseMatrix<double> m = Eigen::SparseMatrix<double> (M, M);
		size_t reserve = M /2;
		m.reserve(Eigen::VectorXi::Constant(reserve, reserve));

		DTRACE2(ML2AM_RESERVE, M, reserve);

		for (EdgeSharedPtr e: *mnet->get_edges(l, l)) {
			int v1_id = e->v1->actor->id;
			int v2_id = e->v2->actor->id;
			m.insert(v1_id - 1, v2_id - 1) = 1;
			m.insert(v2_id - 1, v1_id - 1) = 1;
		}
		adj[l->id - 1] = m;
	}

	DTRACE2(ML2AM_END, N, M);
	return adj;
}



}
