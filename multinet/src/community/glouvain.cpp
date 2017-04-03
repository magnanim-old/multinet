#include "community/glouvain.h"
#include "community/group_handler.h"

namespace mlnet {

Eigen::SparseMatrix<double> glouvain::multicat(std::vector<Eigen::SparseMatrix<double>> a, double gamma, double omega) {

	size_t L = a.size();
	size_t N = a[0].rows();

	Eigen::SparseMatrix<double> B(N * L, N * L);
	B.reserve(Eigen::VectorXi::Constant(N * L, N * N * L + 2 * N * L));

	double twoum = 0.0;

	std::vector<Eigen::Triplet<double>> tlist;
	tlist.reserve(N * N * L + 2 * N * L);

	for (size_t i = 0; i < L; i++) {
		Eigen::MatrixXd kout = cutils::sum(a[i], 0);
		Eigen::MatrixXd kin = cutils::sum(a[i], 1);
		double mm = kout.array().sum();
		twoum += mm;

		Eigen::SparseMatrix<double> tmp, tmp1;
		tmp = (Eigen::SparseMatrix<double>(a[i].transpose()) + a[i]) / 2;

		Eigen::MatrixXd tmp2 = kin * kout.transpose();//().transpose()).array() / mm);
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

hash_set<ActorSharedPtr> glouvain::get_ml_community(MLNetworkSharedPtr mnet, double gamma, double omega) {

	std::vector<Eigen::SparseMatrix<double>> a = cutils::ml_network2adj_matrix(mnet);
	Eigen::SparseMatrix<double> B = multicat(a, 1, 0);//cutils::supraA(a, 0);

	//cutils::modmat(a, gamma, omega, B);
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

	//std::cout << eps << std::endl;

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
					//std::cout << i << " i" << std::endl;
					double di = move(g, i, M.row(i));
					dstep = dstep + di;
					std::cout << dstep << std::endl;
				}
				dtot = dtot + dstep;
				y = g.toVector();
			}
			yb = y;
		}

		//S = y(S);
		//S2 = y(S2);

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

	//std::cout << "out" << std::endl;
	hash_set<ActorSharedPtr> actors;
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
