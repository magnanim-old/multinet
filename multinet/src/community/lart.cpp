#include "community/lart.h"
#include <dlib/clustering.h>
#include <unsupported/Eigen/MatrixFunctions>
#include <math.h>

namespace mlnet {

void lart::modmat(std::vector<Eigen::SparseMatrix<double>> a, double gamma, Eigen::SparseMatrix<double>& sA) {

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
	for (size_t i = 0; i < N; i++) {
		tlist.push_back(Eigen::Triplet<double>(i, N + i, sA.coeff(i, N + i)));
		tlist.push_back(Eigen::Triplet<double>(N + i, i, sA.coeff(N + i, i)));
	}

	for (size_t i = 0; i < L; i++) {
		Eigen::MatrixXd d = cutils::sparse_sum(a[i], 0);

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

// TODO: Parallelize this function
Eigen::MatrixXd lart::pairwise_distance(Eigen::MatrixXd X, Eigen::MatrixXd Y, bool same_obj) {
	DTRACE2(PDISTANCE_START, X.rows(), Y.rows());

	Eigen::MatrixXd XX = (X.array() * X.array()).rowwise().sum();
	Eigen::MatrixXd YY = (Y.array() * Y.array()).rowwise().sum().transpose();
	Eigen::MatrixXd distances = (X * Y.transpose()).unaryExpr([](const double x) { return x * -2;});

	for (int i = 0; i < distances.rows(); i++) {
		distances.col(i).array() += XX.array();
		distances.row(i).array() += YY.array();

		if(same_obj) {
			distances(i, i) = 0;
		}
	}

	return distances.array().unaryExpr([](const double x) { return x < 0 ? 0 : x;}).sqrt();
}

Eigen::MatrixXd lart::Dmat(Eigen::MatrixXd Pt, Eigen::SparseMatrix<double> dA, size_t L) {
	// INFO: dA side effect
	for (int j = 0; j < dA.outerSize(); j++) {
		for (Eigen::SparseMatrix<double>::InnerIterator it(dA, j); it; ++it) {
			dA.coeffRef(it.row(), it.col()) = std::sqrt(it.value());
		}
	}

	Eigen::MatrixXd newP = Pt * Eigen::MatrixXd(dA);

	size_t N = Pt.rows() / L;
	Eigen::MatrixXd Dmat = Eigen::MatrixXd::Zero(N * L, N * L);


	for (size_t i = 0; i < L; ++i) {
		Eigen::MatrixXd X = Eigen::MatrixXd(newP.block(i * N, 0, N, N * L));
		Eigen::MatrixXd m = pairwise_distance(X, X, true);

		for (int j = 0; j < m.rows(); j++) {
			for (int k = 0; k < m.cols(); k++) {
				Dmat(j + (i * N), k + (i * N)) = m(j, k);
			}
		}
	}

	for (size_t i = 0; i  < L - 1; ++i) {
		for (size_t j = i + 1; j < L; ++j) {
			if (i != j) {

				Eigen::MatrixXd newx = newP.block(i * N, i * N, N, N);
				Eigen::MatrixXd newy = newP.block(j * N, j * N, N, N);

				Eigen::MatrixXd tnewx = newP.block(j * N, i * N, N, N);
				Eigen::MatrixXd tnewy = newP.block(i * N, j * N, N, N);

				Eigen::MatrixXd m1(newx.rows(), newx.cols()+tnewy.cols());
				Eigen::MatrixXd m2(newy.rows(), newy.cols()+tnewx.cols());

				m1 << newx,tnewy;
				m2 << newy,tnewx;

				Eigen::MatrixXd dmat = pairwise_distance(m1, m2, false);
				Dmat.block(i * N, (i+1)*N, N, N) = dmat;
				Dmat.block((i+1)*N, i * N, N, N) = dmat.transpose();
			}
		}
	}
	return Dmat;
}

Eigen::SparseMatrix<double> lart::diagA(Eigen::SparseMatrix<double> A) {
	Eigen::SparseMatrix<double> dA = Eigen::SparseMatrix<double>(A.rows(), A.cols());
	dA.reserve(Eigen::VectorXi::Constant(A.rows() / 2, A.rows() / 2));

	Eigen::MatrixXd d = cutils::sparse_sum(A, 1);

	std::vector<Eigen::Triplet<double>> tlist;
	tlist.reserve(A.rows());

	for (int k = 0; k < A.rows(); k++) {
		tlist.push_back(Eigen::Triplet<double>(k, k, 1 / d(k, 0)));
	}
	dA.setFromTriplets(tlist.begin(), tlist.end());
	return dA;
}

Eigen::SparseMatrix<double> lart::block_diag(std::vector<Eigen::SparseMatrix<double>> a) {
	Eigen::SparseMatrix<double> m = Eigen::SparseMatrix<double>(
		a[0].rows() * a.size(), a[0].cols() * a.size());

	m.reserve(Eigen::VectorXi::Constant(a[0].rows(), a[0].rows()));

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

unsigned long lart::prcheck(std::vector<Eigen::SparseMatrix<double>> a, Eigen::SparseMatrix<double>& aP) {
	Eigen::SparseMatrix<double> aplus = Eigen::SparseMatrix<double>(a[0].rows(), a[0].cols());
	for (size_t i = 0; i < a.size(); i++) {
		aplus += a[i];
	}

	std::vector<unsigned long> labels;
	std::vector<dlib::sample_pair> edges;

	for (int j = 0; j < aplus.outerSize(); j++) {
		for (Eigen::SparseMatrix<double>::InnerIterator it(aplus, j); it; ++it) {
			edges.push_back(dlib::sample_pair(it.row(), it.col(), 1));
		}
	}

	unsigned long num_clusters = dlib::newman_cluster(edges, labels);
	std::vector<unsigned long> uq = labels;
	std::sort(uq.begin(), uq.end());
	std::vector<unsigned long>::iterator it;
	it = std::unique(uq.begin(), uq.end());
	uq.resize(std::distance(uq.begin(), it));

	std::random_device seed;
	std::mt19937 engine(seed()) ;
   	std::uniform_int_distribution<int> choose(0, labels.size() - 1);

	if (labels.size() > 0) {
		for (size_t i = 0; i < uq.size(); i++) {
			std::vector<unsigned long> tmp;

			int index = choose(engine);

			for (int j = 0; j < aP.rows(); j++) {
				//aP.coeffRef(index, j) = 0.85 * aP.coeff(index, j) + 0.15 / (a[0].size() * a.size());
			}
		}

	}
	return num_clusters;
}

Eigen::SparseMatrix<double> lart::supraA(std::vector<Eigen::SparseMatrix<double>> a, double eps) {
	Eigen::SparseMatrix<double> A = block_diag(a);
	size_t L = a.size();
	size_t N = a[0].rows();

	for (size_t i = 0; i  < L - 1; ++i) {
		for (size_t j = i + 1; j < L; ++j) {
			Eigen::MatrixXd d = cutils::sparse_sum(a[i].cwiseProduct(a[j]), 1);

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

CommunitiesSharedPtr lart::get_ml_community(
	MLNetworkSharedPtr mnet, uint32_t t, double eps, double gamma) {
	std::vector<Eigen::SparseMatrix<double>> a = cutils::ml_network2adj_matrix(mnet);
	Eigen::SparseMatrix<double> sA = supraA(a, eps);
	Eigen::SparseMatrix<double> dA = diagA(sA);

	Eigen::SparseMatrix<double> aP = dA * sA;
	int disconnected = prcheck(a, aP);

	DTRACE0(WALK_START);
	Eigen::MatrixXd A = Eigen::MatrixXd(aP);
	Eigen::MatrixPower<Eigen::MatrixXd> Apow(A);
	Eigen::MatrixXd Pt = Apow(t);
	DTRACE0(WALK_END);

	Eigen::MatrixXd Dt = Dmat(Pt, dA, a.size());

	/*if (disconnected) {
		Dt = Dt.array().unaryExpr([](double v) { return v != 0 ? v : 10.0; });
	}*/

	for (int i = 0; i < Dt.rows(); i++)
		Dt(i, i) = 10;

	DTRACE0(CLUSTER_START);
	std::vector<unsigned long> labels;
	dlib::matrix<double> dists = dlib::mat(Dt);
	dlib::bottom_up_cluster(dists, labels, 2);

	std::vector<unsigned long> uq = labels;
	std::sort(uq.begin(), uq.end());
	std::vector<unsigned long>::iterator it;
	it = std::unique(uq.begin(), uq.end());
	uq.resize(std::distance(uq.begin(), it));

	DTRACE1(CLUSTER_END, uq.size());

	//for (unsigned long k : labels)
	//	std::cout << k << " ";
	//std::cout << std::endl;

	return communities::create();

}

}
