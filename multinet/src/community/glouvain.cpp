#include "community/glouvain.h"
#include "community/group_handler.h"

namespace mlnet {

Eigen::SparseMatrix<double> glouvain::multicat(std::vector<Eigen::SparseMatrix<double>> a, double gamma, double omega) {

	size_t L = a.size();
	size_t N = a[0].rows();

	Eigen::SparseMatrix<double> B(N * L, N * L);
	B.reserve(Eigen::VectorXi::Constant(N * L, N * L));

	double twoum = 0.0;

	std::vector<Eigen::Triplet<double>> tlist;
	tlist.reserve(N * L);

	for (size_t i = 0; i < L; i++) {
		Eigen::MatrixXd kout = cutils::sum(a[i], 0);
		Eigen::MatrixXd kin = cutils::sum(a[i], 1);
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
					Eigen::Triplet<double>(j + (i * N), k + ( i * N), tmp.coeff(j, k)));
			}
		}
	}

	for (size_t i = 0; i < N; i++) {
		tlist.push_back(
			Eigen::Triplet<double>(i, i + N, omega));
		tlist.push_back(
			Eigen::Triplet<double>(i + N, i, omega));
	}
	B.setFromTriplets(tlist.begin(), tlist.end());
	return B;
}

CommunitiesSharedPtr glouvain::get_ml_community(MLNetworkSharedPtr mnet, double gamma, double omega) {
	std::vector<Eigen::SparseMatrix<double>> a = cutils::ml_network2adj_matrix(mnet);
	Eigen::SparseMatrix<double> B = multicat(a, gamma, omega);

	size_t n = B.rows();
	Eigen::SparseMatrix<double> M(B);

	std::vector<int> S(n), S2, y;
	std::iota(S.begin(), S.end(), 0);
	S2 = S;
	y = S;
	
	std::vector<int> Sb;

	double dtot = 0;
	double eps = std::numeric_limits<double>::min();
	while (Sb != S2) {
		Sb = S2;
		std::vector<int> yb;

		while (yb != y) {
			double dstep = 1.0;

			while (yb != y && dstep/dtot > 2 * eps && dstep > 10 * eps){
				yb = y;
				dstep = 0;

				group_index g(y);
				std::vector<int> M_len(M.rows());
				std::iota(M_len.begin(), M_len.end(), 0);
				std::random_shuffle (M_len.begin(), M_len.end());

				for (int i : M_len) {
					double di = move(g, i, M.col(i));
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
		y = S2;
		std::sort(y.begin(), y.end());
		std::vector<int>::iterator it;
		it = std::unique(y.begin(), y.end());
		y.resize(std::distance(y.begin(), it));

	}

	return cutils::nodes2communities(mnet, Sb);

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


}
