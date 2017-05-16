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
		Use: MatrixXd m = sparse_sum(X, axis);
		Pre: X is an initialized Eigen sparse matrix, axis is an integer with value 0 or 1
		Post: m is a dense 1-dimensional matrix that has the elements of X summed up along the give axis
			0 for horizontal, 1 for vertical. Since in Eigen Vector structures are just typedefs for Matrix we return
			it as a Matrix. Use it like this: m(i, 0) where 0 <= i <= X.rows()
	*/
	static Eigen::MatrixXd sparse_sum(Eigen::SparseMatrix<double> X, int axis);

	/*
		Use: std::vector<int> u = cutils::unique(std::vector<int> y);
		Pre: y is a stl vector
		Post: returns the unique integers in vector y in ascending order
	*/
	static std::vector<int> unique(std::vector<int> y);

	/*
		Use: std::vector<int> r = cutils::unique(size, rand);
		Post: returns a sequence of integers from 0 to < size. If randomize is set to true
			the contents of x are randomly shuffled
	*/
	static std::vector<int> range(size_t size, bool randomize);

	static CommunityStructureSharedPtr nodes2communities(MLNetworkSharedPtr mnet, std::vector<unsigned int> nodes2cid);
	static CommunityStructureSharedPtr actors2communities(MLNetworkSharedPtr mnet, std::vector<unsigned int> actors2cid);

};

}


#endif
