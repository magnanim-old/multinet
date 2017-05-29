#include "community/glouvain.h"

namespace mlnet {

std::vector<int> glouvain::mapV2I(std::vector<int> a, std::vector<int> b) {
	std::vector<int> v(b.size());
	for (size_t i = 0; i < b.size(); i++) {
		v[i] = a[b[i]];
	}
	return v;
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

double glouvain::Q_handle(metanet M, std::vector<int> y, double twoum) {
	Eigen::SparseMatrix<double> P(y.size(), y.size());

	std::vector<Eigen::Triplet<double>> tlist;
	tlist.reserve(y.size());
	for (size_t i = 0; i < y.size(); i++) {
		tlist.push_back(Eigen::Triplet<double>(i, i, 1));
	}
	P.setFromTriplets(tlist.begin(), tlist.end());

	double Q = 0;
	for (size_t i = 0; i < y.size(); i++) {
		Q += Eigen::MatrixXd(Eigen::SparseMatrix<double>(P * M.get(i)).transpose() * P.col(i)).array().sum();
	}

	return Q / twoum;
}

CommunityStructureSharedPtr glouvain::fit(MLNetworkSharedPtr mnet, std::string m, double gamma, double omega, size_t limit) {
	DTRACE4(GLOUVAIN_START, mnet->name.c_str(), m.c_str(), std::to_string(gamma), std::to_string(omega));

	double (*move_func)(group_index &, int, Eigen::SparseMatrix<double>);
	if ("moverandw" == m) {
		move_func = &moverandw;
	} else {
		move_func = &move;
	}

	std::vector<int> S(mnet->get_actors()->size() * mnet->get_layers()->size());
	std::iota(S.begin(), S.end(), 0);

	std::vector<int> y, Sb;
	y = S;

	double eps = 2.2204e-12;
	double dtot = 0;
	Eigen::SparseMatrix<double> B, M;

	double twoum = 0;
	if (limit < y.size()) {
		std::vector<Eigen::SparseMatrix<double>> A = cutils::ml_network2adj_matrix(mnet);
		for (auto m: A){
			twoum += m.nonZeros();
		}
		twoum = twoum + (A[0].rows() * A.size() * (A.size() - 1) * omega);

		metanet meta(A, gamma, omega);
		meta.assign(S);

		while (Sb != S) {
			DTRACE1(GLOUVAIN_PASS, y.size());
			Sb = S;
			std::vector<int> yb;

			while (yb != y) {
				double dstep = 1.0;

				while (yb != y && (dstep/dtot > 2 * eps) && (dstep > 10 * eps)) {
					DTRACE0(GLOUVAIN_FIRST_PHASE_START);
					yb = y;
					dstep = 0;

					group_index g(y);
					for (int i: cutils::range(meta.get(0).rows(), true)) {
						double di = move_func(g, i, meta.get(i));
						dstep = dstep + di;
					}

					dtot = dtot + dstep;
					y = g.toVector();
					DTRACE3(GLOUVAIN_FIRST_PHASE_END, y.size(), std::to_string(dstep), std::to_string(dtot));
				}
				yb = y;
			}

			S = mapV2I(y, S);
			y = cutils::unique(S);

			if (Sb == S) {
				std::vector<unsigned int> partition(S.begin(), S.end());
				DTRACE4(GLOUVAIN_END, y.size(), meta.get(0).rows(), meta.get(0).cols(), std::to_string(Q_handle(meta, y, twoum)));
				return cutils::nodes2communities(mnet, partition);
			}

			meta.assign(S);

			if (y.size() < limit) {
				std::vector<Eigen::Triplet<double>> tlist;
				Eigen::SparseMatrix<double> t = meta.get(0);
				tlist.reserve(t.nonZeros() * y.size());

				for (int i = 0; i < (int) y.size(); i++) {
					Eigen::SparseMatrix<double> tmp = meta.get(i);
					for (int j = 0; j < tmp.outerSize(); ++j) {
						for (Eigen::SparseMatrix<double>::InnerIterator it(tmp, j); it; ++it) {
							tlist.push_back(Eigen::Triplet<double>(it.row(), i, it.value()));
						}
					}
				}

				B = Eigen::SparseMatrix<double>(t.rows(), y.size());
				B.setFromTriplets(tlist.begin(), tlist.end());
				M = B;
				break;
			}
		}
	} else {
		B = cutils::ng_modularity(twoum, cutils::ml_network2adj_matrix(mnet), gamma, omega);
		M = B;
	}

	std::vector<int> S2(B.rows());
	std::iota(S2.begin(), S2.end(), 0);
	Sb.clear();

	while (Sb != S2) {
		DTRACE1(GLOUVAIN_PASS, y.size());
		Sb = S2;
		std::vector<int> yb;

		while (yb != y) {
				double dstep = 1.0;

				while (yb != y && (dstep/dtot > 2 * eps) && (dstep > 10 * eps)) {
					DTRACE0(GLOUVAIN_FIRST_PHASE_START);
					yb = y;
					dstep = 0;

					group_index g(y);
					for (int i: cutils::range(M.cols(), true)) {
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
		S2 = mapV2I(y, S2);

		if (Sb == S2) {
			break;
		}

		M = metanetwork(B, S2);
		y = cutils::unique(S2);

	}

	std::vector<unsigned int> partition(S.begin(), S.end());
	DTRACE4(GLOUVAIN_END, y.size(), M.rows(), M.cols(), std::to_string(Q(M, y, twoum)));
	return cutils::nodes2communities(mnet, partition);
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
