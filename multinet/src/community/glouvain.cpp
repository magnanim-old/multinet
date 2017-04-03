#include "community/glouvain.h"
#include "community/group_handler.h"

namespace mlnet {

hash_set<ActorSharedPtr> glouvain::get_ml_community(MLNetworkSharedPtr mnet, double gamma, double omega) {

	hash_set<ActorSharedPtr> actors;
	std::vector<Eigen::SparseMatrix<double>> a = cutils::ml_network2adj_matrix(mnet);
	Eigen::SparseMatrix<double> B = cutils::supraA(a, 0);

	cutils::modmat(a, gamma, omega, B);
	//modmat(a, gamma, omega, B);

	size_t n = B.rows();

	Eigen::SparseMatrix<double> M(B);


	std::vector<int> S(n);
	std::iota(S.begin(), S.end(), 0);

	std::vector<int> S0(S.begin(), S.end());
	std::vector<int> S2(S.begin(), S.end());
	std::vector<int> y(S.begin(), S.end());

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
				std::cout << M << std::endl;
				for (int i : M_len) {
					double di = move(g, i, M.col(i));
					dstep = dstep + di;
					std::cout << dstep << std::endl;
				}
				dtot = dtot + dstep;
				y = g.toVector();
			}
			yb = y;
		}

		S = y;
		S2 = y;

		if (Sb == S2) {

			for (auto i: Sb)
				std::cout << i << ' ';
			std::cout << std::endl;
			//P=sparse(y,1:length(y),1);
			//Q=full(sum(sum((P*M).*P)));


			return actors;
		}

		M = metanetwork(B, S2, a.size());

		y = S2;
		std::sort(y.begin(), y.end());
		std::vector<int>::iterator it;
		it = std::unique(y.begin(), y.end());
		y.resize(std::distance(y.begin(), it));
	}

	return actors;
}


Eigen::SparseMatrix<double> glouvain::metanetwork(Eigen::SparseMatrix<double> B, std::vector<int> S2, size_t L) {
	Eigen::SparseMatrix<double> PP(B.rows(), *std::max_element(S2.begin(), S2.end()) + 1);
	PP.reserve(B.rows());

	std::vector<Eigen::Triplet<double>> tlist;
	tlist.reserve(B.rows());

	for (size_t i = 0; i < L; i++) {
		for (int k = 0; k < PP.cols(); k++) {
			tlist.push_back(Eigen::Triplet<double>((i) * PP.cols() + k, k, 1));
		}
	}
	PP.setFromTriplets(tlist.begin(), tlist.end());
	return PP.transpose() * B * PP;
}


}
