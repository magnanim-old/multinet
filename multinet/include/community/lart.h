#ifndef LART_H_
#define LART_H_

#include "cutils.h"

namespace mlnet {

class lart {

public:

	/*
		Use : lart k;
			 k.get_ml_community(MLNetworkSharedPtr, uint32_t, double, double);
		Pre : MLNetworkSharedPtr is a multilayer network
			 t is the number of number of steps for random walker to take
			 eps
			 gamma
		Post: (?)
	*/
	CommunitiesSharedPtr get_ml_community(MLNetworkSharedPtr mnet, uint32_t t, double eps, double gamma);


private:

	/*
		Data Invariant:
			Locally adaptive random transition is a multiplex community detector based on the walktrap algorithm.
			This implementation is based on Zhana Kuncheva's paper (https://arxiv.org/pdf/1507.01890.pdf)
			and Python implementation.
			TODO: Write a bit more about the algorithm.
			It accepts a MLNetworkSharedPtr object and produces a list of actors with the cluster id's they belong to.
			To run this algorithm it is required to have the eigen3 C++ library installed (http://eigen.tuxfamily.org/)
			This algorithm contains DTrace and SystemTap probe points for performance testing and tuning.
	*/


	/*
		Use: auto m = block_diag(a);
		Pre: a is a list of adjacency matrixes
		Post: m is a a.size() * a[i].size() matrix, with the matrix contents of a
			stacked diagonally
	*/
	Eigen::SparseMatrix<double> block_diag(std::vector<Eigen::SparseMatrix<double>> a);

	/*
		Use: auto D = diagA(m);
		Pre: m is the supra adjacency matrix
		Post: D is NL x NL diagonal matrix defined by the multiplex node degrees

	*/
	Eigen::SparseMatrix<double> diagA(Eigen::SparseMatrix<double> m);

	/*
		Use: auto A = Dmat(Pt, D, L);
		Pre: Pt (?)
		Post:
	*/
	Eigen::MatrixXd Dmat(Eigen::MatrixXd Pt, Eigen::SparseMatrix<double> D, size_t L);

	/*
		Use: auto p = pairwise_distance(X, Y);
		Pre: X and Y are matrices (they may be the same objects)
		Post: p contains euclidean distances between pairs of X and Y
	*/
	Eigen::MatrixXd pairwise_distance(Eigen::MatrixXd X, Eigen::MatrixXd Y, bool same_obj);


	void modmat(std::vector<Eigen::SparseMatrix<double>> a, double gamma, Eigen::SparseMatrix<double>& sA);

	unsigned long prcheck(std::vector<Eigen::SparseMatrix<double>> a, Eigen::SparseMatrix<double>& sA);

	//vector<double> modMLPX(vector<lart::cluster> x, std::vector<Eigen::SparseMatrix<double>> a, double gamma, Eigen::SparseMatrix<double> sA);

};

}

#endif
