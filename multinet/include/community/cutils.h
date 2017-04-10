#ifndef CUTILS_H_ /* Community utils, common routines used by multilayer community detection algorithms */
#define CUTILS_H_

#include "community.h"

namespace mlnet {

class cutils {

public:

	static CommunitiesSharedPtr nodes2communities(MLNetworkSharedPtr mnet, std::vector<int> nodes2cid);
	static CommunitiesSharedPtr actors2communities(MLNetworkSharedPtr mnet, std::vector<unsigned int> actors2cid);

	/*
		Use : std::vector<Eigen::MatrixXd> a = ml_network2adj_matrix(MLNetworkSharedPtr ptr);
		Pre : MLNetworkSharedPtr is a non empty multilayer network
		Post: a is a list of Matrixes that is the adjacency matrix of mnet.
			Each matrix represents one layer
	*/
	static std::vector<Eigen::SparseMatrix<double>> ml_network2adj_matrix(MLNetworkSharedPtr mnet);

	/*
		Use: auto m = block_diag(a);
		Pre: a is a list of adjacency matrixes
		Post: m is a a.size() * a[i].size() matrix, with the matrix contents of a
			stacked diagonally
	*/
	static Eigen::MatrixXd sparse_sum(Eigen::SparseMatrix<double> X, int axis);

};

}


#endif
