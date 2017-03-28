#include "community/glouvain.h"
#include "community/group_handler.h"

namespace mlnet {

hash_set<ActorSharedPtr> glouvain::get_ml_community(MLNetworkSharedPtr mnet, double gamma, double omega) {

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

	std::cout << eps << std::endl;

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
					std::cout << i << " i" << std::endl;
					double di = move(g, i, M.row(i));
					dstep = dstep + di;
				}
				dtot = dtot + dstep;
				// y = group_handler;

			}
			yb = y;
		}

		//S = y(S);
		//S2 = y(S2);

		if (Sb == S2) {
			//P
		}

	}

	std::cout << "out" << std::endl;
	hash_set<ActorSharedPtr> actors;
	return actors;
}

}
