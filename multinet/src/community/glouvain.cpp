#include "community/glouvain.h"

namespace mlnet {

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

double glouvain::Q(Eigen::SparseMatrix<double> M, std::vector<int> y, double twoum) {
	Eigen::SparseMatrix<double> P(y.size(), y.size());

	std::vector<Eigen::Triplet<double>> tlist;
	tlist.reserve(y.size());

	for (size_t i = 0; i < y.size(); i++) {
		tlist.push_back(Eigen::Triplet<double>(i, i, 1));
	}

	P.setFromTriplets(tlist.begin(), tlist.end());
	return Eigen::MatrixXd((P*M).cwiseProduct(P)).sum() / twoum;
}

CommunityStructureSharedPtr glouvain::fit(MLNetworkSharedPtr mnet, std::string m, double gamma, double omega) {
	DTRACE4(GLOUVAIN_START, mnet->name.c_str(), m.c_str(), std::to_string(gamma), std::to_string(omega));

	std::vector<Eigen::SparseMatrix<double>> a = cutils::ml_network2adj_matrix(mnet);

	double twoum = 0.0;
	Eigen::SparseMatrix<double> B = cutils::ng_modularity(twoum, a, gamma, omega);

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
	double eps = 2.2204e-5;

	while (Sb != S2) {
		DTRACE4(GLOUVAIN_PASS, y.size(), M.rows(), M.cols(), std::to_string(Q(M, y, twoum)));
		Sb = S2;
		std::vector<int> yb;

		while (yb != y) {
			double dstep = 1.0;

			while (yb != y && (dstep/dtot > 2 * eps) && (dstep > 10 * eps)) {
				DTRACE0(GLOUVAIN_FIRST_PHASE_START);
				yb = y;
				dstep = 0;

				group_index g(y);
				for (int i : cutils::range(M.rows(), true)) {
					double di = move_func(g, i, M.col(i));
					dstep = dstep + di;
				}

				dtot = dtot + dstep;
				y = g.toVector();

				DTRACE3(GLOUVAIN_FIRST_PHASE_END, y.size(), std::to_string(dstep), std::to_string(dtot));
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

		DTRACE0(GLOUVAIN_PASS_END);
	}

	std::vector<unsigned int> partition(Sb.begin(), Sb.end());

	DTRACE4(GLOUVAIN_END, y.size(), M.rows(), M.cols(), std::to_string(Q(M, y, twoum)));

	return cutils::nodes2communities(mnet, partition);
}

}
