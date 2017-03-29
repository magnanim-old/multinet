#ifndef CUTILS_H_ /* Community utils, common routines used by multilayer community detection algorithms */
#define CUTILS_H_

#include "community.h"

namespace mlnet {

class cutils {

public:
	/*
		Use : std::vector<Eigen::MatrixXd> a = ml_network2adj_matrix(MLNetworkSharedPtr ptr);
		Pre : MLNetworkSharedPtr is a non empty multilayer network
		Post: a is a list of Matrixes that is the adjacency matrix of mnet.
			Each matrix represents one layer
	*/
	static std::vector<Eigen::SparseMatrix<double>> ml_network2adj_matrix(MLNetworkSharedPtr mnet);

	/*
		Use: auto A = supraA(a, eps);
		Pre: a is a list of adjacency matrixes. eps is the probability for the walker to move between layers
		Post: A is the supra adjacency matrix from the list of matrixes, with inter layer connections
			on the off diagonal blocks.

	*/
	static Eigen::SparseMatrix<double> supraA(std::vector<Eigen::SparseMatrix<double>> a, double eps);

	static void modmat(std::vector<Eigen::SparseMatrix<double>> a, double gamma, double omega,
		Eigen::SparseMatrix<double>& sA);

	/*
		Use: auto m = block_diag(a);
		Pre: a is a list of adjacency matrixes
		Post: m is a a.size() * a[i].size() matrix, with the matrix contents of a
			stacked diagonally
	*/
	static Eigen::SparseMatrix<double> block_diag(std::vector<Eigen::SparseMatrix<double>> a);
	static Eigen::MatrixXd sum(Eigen::SparseMatrix<double> X, int axis);


};

}


#endif
