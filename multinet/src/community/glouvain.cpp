#include "community/glouvain.h"

namespace mlnet {

Eigen::SparseMatrix<double> glouvain::ng_modularity(std::vector<Eigen::SparseMatrix<double>> a, double gamma, double omega) {
	size_t L = a.size();
	size_t N = a[0].rows();

	Eigen::SparseMatrix<double> B(N * L, N * L);
	B.reserve(Eigen::VectorXi::Constant(N * L, N * L));

	double twoum = 0.0;

	std::vector<Eigen::Triplet<double>> tlist;
	tlist.reserve(N * L);

	for (size_t i = 0; i < L; i++) {
		Eigen::MatrixXd kout = cutils::sparse_sum(a[i], 0);
		Eigen::MatrixXd kin = cutils::sparse_sum(a[i], 1);
		double mm = kout.array().sum();
		twoum += mm;

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
	return B;
}

std::vector<int> glouvain::mapV2I(std::vector<int> a, std::vector<int> b) {
	std::vector<int> v(b.size());
	for (size_t i = 0; i < b.size(); i++) {
		v[i] = a[b[i]];
	}
	return v;
}

Eigen::SparseMatrix<double> glouvain::metanetwork(Eigen::SparseMatrix<double> B, std::vector<int> S2) {
	Eigen::SparseMatrix<double> PP(B.rows(), *std::max_element(S2.begin(), S2.end()) + 1);
	PP.reserve(B.rows());

	std::vector<Eigen::Triplet<double>> tlist;
	tlist.reserve(B.rows());

	for (size_t i = 0; i < S2.size(); i++) {
		tlist.push_back(Eigen::Triplet<double>(i, S2[i], 1));
	}
	PP.setFromTriplets(tlist.begin(), tlist.end());
	return PP.transpose() * B * PP;
}

CommunityStructureSharedPtr glouvain::fit(MLNetworkSharedPtr mnet, std::string m, double gamma, double omega) {
	std::vector<Eigen::SparseMatrix<double>> a = cutils::ml_network2adj_matrix(mnet);
	Eigen::SparseMatrix<double> B = ng_modularity(a, gamma, omega);

	double (*move_func)(group_index &, int, Eigen::SparseMatrix<double>);
	if ("moverandw" == m) {
		move_func = &moverandw;
	} else {
		move_func = &move;
	}

	size_t n = B.rows();
	Eigen::SparseMatrix<double> M(B);

	std::vector<int> S(n), S2, y;
	std::iota(S.begin(), S.end(), 0);
	S2 = S;
	y = S;

	std::vector<int> Sb;

	double dtot = 0;
	double eps = 2.2204e-16;

	while (Sb != S2) {
		Sb = S2;
		std::vector<int> yb;

		while (yb != y) {
			double dstep = 1.0;

			while (yb != y && (dstep/dtot > 2 * eps) && (dstep > 10 * eps)) {
				yb = y;
				dstep = 0;

				group_index g(y);
				for (int i : cutils::range(M.rows(), true)) {
					double di = move_func(g, i, M.col(i));
					dstep = dstep + di;
				}

				dtot = dtot + dstep;
				y = g.toVector();

			}
			yb = y;
		}

		S = mapV2I(y, S);
		S2 = S;

		if (Sb == S2) {
			break;
		}

		M = metanetwork(B, S2);
		y = cutils::unique(S2);

	}

	std::vector<unsigned int> partition(Sb.begin(), Sb.end());
	DTRACE5(GLOUVAIN_END, mnet->name.c_str(), m.c_str(), std::to_string(gamma), std::to_string(omega),
		std::set<unsigned int>(partition.begin(), partition.end()).size());

	return cutils::nodes2communities(mnet, partition);
}

}
