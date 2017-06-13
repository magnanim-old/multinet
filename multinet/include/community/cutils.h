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
			0 for horizontal, 1 for vertical. Since in Eigen, Vector structures are just typedefs for Matrix we return
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

	/*
		Use: CommunityStructureSharedPtr c = cutils::nodes2communities(mnet, nodes2cid);
		Pre: mnet is a mulltilayer network, with at least some connected actors and 1+ layers
			 nodes2cid is a vector of community assignment for the nodes in mnet, where
			 nodes2cid.size() == mnet->get_actors->size() * mnet->get_layers->size()
		Post: returns a community assignment of actors with their real attributes from the multilayer network
	*/
	static CommunityStructureSharedPtr nodes2communities(MLNetworkSharedPtr mnet, std::vector<unsigned int> nodes2cid);

	/*
		Use: CommunityStructureSharedPtr c = cutils::actors2communities(mnet, actors2cid);
		Pre: mnet is a mulltilayer network, with at least some connected actors and 1+ layers
			 actors2cid is a vector of community assignments for the actors in mnet, where
			 actors2cid.size() == mnet->get_actors->size()
		Post: returns a community assignment of actors with their real attributes from the multilayer network
	*/
	static CommunityStructureSharedPtr actors2communities(MLNetworkSharedPtr mnet, std::vector<unsigned int> actors2cid);

	/*
		Use: Eigen::SparseMatrix<double> B = cutils::ng_modularity(twom, a, gamma, omega);
		Pre: twoum gets the value of the degree of the nodes in the multilayer network
			 a is a vector of adjacency matrices (layers)
			 gamma is the resolution parameter, for all layers
			 omega is the inter-layer coupling strength parameter.
		Post: returns multilayer Girvan-Newman modularity matrix B and the total number of edges in the network * 2, as reference in the twoum parameter. This function is inspired by https://github.com/GenLouvain/GenLouvain/blob/master/HelperFunctions/multicat.m
	*/
	static Eigen::SparseMatrix<double> ng_modularity(double& twom, std::vector<Eigen::SparseMatrix<double>> a, double gamma, double omega);

	/*
		Use: Eigen::SparseMatrix<double> A = cutils::supraA(a, eps, use_node_degrees, use_self_loop);
		Pre: a is a vector of adjacency matrices (layers)
			eps is the diagonal and off-diagonal weight parameter
		Post: Returns a symmetric N * L * N * L matrix where N is the number of actors and L is the number layers
				Diagonal blocks of the matrix are adjacency matrices from a, also known as inter-layer edges. Off-diagonal blocks are the intra-layer edges connecting the same node across the layers. use_node_degrees will set node degree as intra-layer edge weight on the off-diagonal block. use_self_loop sets eps on the main diagonal, which is a self loop for every node.
	*/
	static Eigen::SparseMatrix<double> supraA(std::vector<Eigen::SparseMatrix<double>> a, double eps, bool use_node_degrees, bool use_self_loop);

	/*
		Use: Eigen::SparseMatrix<double> diag = cutils::block_diag(a, eps);
		Pre: a is a vector of adjacency matrices (layers)
			eps is the diagonal and off-diagonal weight parameter
		Post: Returns a symmetric N * L * N * L matrix where N is the number of actors and L is the number layers. Diagonal blocks are the intra-layuer edges.
	*/
	static Eigen::SparseMatrix<double> block_diag(std::vector<Eigen::SparseMatrix<double>> a, double eps);

};

}


#endif
