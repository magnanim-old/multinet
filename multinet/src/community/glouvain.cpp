#include "community/glouvain.h"
#include "community/group_handler.h"

namespace mlnet {

hash_set<ActorSharedPtr> glouvain::get_ml_community(MLNetworkSharedPtr mnet, double gamma, double omega) {

	hash_set<ActorSharedPtr> actors;
	std::vector<Eigen::SparseMatrix<double>> a = cutils::ml_network2adj_matrix(mnet);
	Eigen::SparseMatrix<double> B = cutils::supraA(a, 0);

	cutils::modmat(a, gamma, omega, B);

	size_t n = B.rows();

	Eigen::SparseMatrix<double> M(B);
	std::vector<int> M_len(M.rows());
	std::iota(M_len.begin(), M_len.end(), 0);


	std::vector<int> S(n);
	std::iota(S.begin(), S.end(), 1);

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

		S = y;
		S2 = y;

		if (Sb == S2) {
			std::cout << "Sb and S2 are equal" << std::endl
			return actors;
		}

		

	}

	return actors;
}

}
