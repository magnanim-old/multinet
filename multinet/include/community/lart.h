#ifndef LART_H_
#define LART_H_

#include "cutils.h"
#include <dlib/clustering.h>

namespace mlnet {

class lart {

public:

	/*
		Use : lart k;
			  k.fit(mnet, t, eps, gamma);
		Pre : mnet is a multilayer network
			 t is the number of number of steps for random walker to take
			 eps adds weight to each node, i.e chance to not make a move
			 gamma is the resolution parameter that controls size of communities discovered
		Post: Return the communities that each node in mnet belongs to.
	*/
	CommunityStructureSharedPtr fit(MLNetworkSharedPtr mnet, int t, double eps, double gamma);

private:

	/*
		Data Invariant:
			Locally adaptive random transition is a multiplex community detector based on the walktrap algorithm.
			This implementation is based on Zhana Kuncheva's paper (https://arxiv.org/pdf/1507.01890.pdf)
			and Python implementation.
	*/

	/*
		Use: int i = is_connected(a, edges)
		Pre: a contains the layers of mnet as adjacency matrices
		Post: i is 1 if the multiplex has no disconnected parts 0 otherwise
	*/

	int is_connected(std::vector<Eigen::SparseMatrix<double>> a, std::vector<dlib::sample_pair>& edges);

	/*
		Use: auto D = diagA(m);
		Pre: m is the supra adjacency matrix
		Post: D is N * L * N * L diagonal matrix defined by the multiplex node degrees

	*/
	Eigen::SparseMatrix<double> diagA(Eigen::SparseMatrix<double> m);

	/*
		Use: prcheck(aP, edges, LN)
		Pre: x contains contains node degrees, which represent node transition probabilities.
			 edges is the amount of edges in the network, in dlib format
		Post: Add additional edges to the probability matrix aP, to make sure that the matrix is connected and has no disconnected components. This allows the walker to "teleport" to any node, regardless if its connected or not. The random probability to jump from node i to any other node in the multiplex is 1/(N*L) as we have N*L different N nodes across the L layers.
	*/
	unsigned long prcheck(Eigen::SparseMatrix<double>& aP, std::vector<dlib::sample_pair> edges, unsigned int LN);

	/*
		Use: exp_by_squaring_iterative(x, t);
		Pre: x contains contains node degrees, which represent node transition probabilities.
			t is the amount of steps a walker will take
		Post: x contains transition probabilities after t walks
	*/
	void exp_by_squaring_iterative(Eigen::SparseMatrix<double>& x, int t);

	/*
		Use: auto dMat = Dmat(Pt, D, L);
		Pre: Pt is the transition probability matrix, D
		Post: dMat is the dissimilarity matrix that has the distance from every node to any other
	*/
	Eigen::MatrixXd Dmat(Eigen::SparseMatrix<double> Pt, Eigen::SparseMatrix<double> D, size_t L);

	/*
		Use: auto p = pairwise_distance(X, Y);
		Pre: X and Y are matrices (they may be the same objects)
		Post: p contains euclidean distances between pairs of X and Y
	*/
	Eigen::MatrixXd pairwise_distance(Eigen::MatrixXd X, Eigen::MatrixXd Y, bool same_obj);

	/*
		Use: auto v = find_ix(x, y)
		Post: v is the index of all occurances of y in x
	*/
	std::vector<unsigned long> find_ix(std::vector<unsigned long> x, unsigned long y);

	/*
		Use: updateDt(Dt, a)
		Pre: Dt is the dissimilarity matrix that has the distance from every node to any other
			a contains the layers of mnet as adjacency matrices
		Post: Add artificial high weight to disconnected components to Dt, so they are connected last
	*/
	void updateDt(Eigen::MatrixXd& Dt, std::vector<Eigen::SparseMatrix<double>> a);

	/*
		Use: auto v = modMLPX(Dt, a)
		Pre: clusters contains the agglomerative clustering history of the dissimilarity matrix
			a contains the layers of mnet as adjacency matrices
			gamma is the resolution parameger, used in the modualrity computation
		Post: v is the modularity score for each pair of clusters in clusters
	*/
	vector<double> modMLPX(vector<dlib::bu_cluster> clusters, std::vector<Eigen::SparseMatrix<double>> a, double gamma);

	/*
		Use: auto v = modMLPX(Dt, a)
		Pre: clusters contains the agglomerative clustering history of the dissimilarity matrix
			maxmodix is the index of the highest modularity from modMLPx
			L is the amount of layers in the multiplex
			N is the amount of actors in the multiplex
		Post: v is node partitioning based on which partition in modMLPX gives the highest modularity score
	*/
	vector<unsigned int> get_partition(vector<dlib::bu_cluster> clusters, int maxmodix, size_t L, size_t N);
};

}

#endif /* LART_H_ */
