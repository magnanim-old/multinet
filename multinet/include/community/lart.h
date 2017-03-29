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

	struct dist {
		int row;
		int col;
		double val;
	};

	struct cluster {
		int left;
		int right;
		size_t id;
		std::vector<int> orig;
	};

	/*
		Data Invariant:
			+ dist is a point in the supra adjacency matrix, A
				- in this context, left is the x co-ordinate. right is the y co-ordinate
				- val is the value that the supra adjacency matrix contains at this point
			+ cluster is a cluster object that the Agglomerative Clustering function produces.
				- left and right is the id of the clusters that merged. NOTE: left < right
				- id is the id of this cluster.
				- orig is a list of cluster ids that merged with this cluster.
					Only data point ids are stored here. Ex: 0 + 1 merges into cluster x. x.orig == [0, 1]

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
		Use: auto M = matrix_power(m, t);
		Pre: m is a matrix, t is an unsigned 32bit integer
		Post: M is a new matrix that is m raised to the power of t
	*/
	Eigen::SparseMatrix<double> matrix_power(Eigen::SparseMatrix<double> m, uint32_t t);

	/*
		Use: auto A = Dmat(Pt, D, L);
		Pre: Pt (?)
		Post:
	*/
	Eigen::MatrixXd Dmat(Eigen::SparseMatrix<double> Pt, Eigen::SparseMatrix<double> D, size_t L);

	/*
		Use: auto p = pairwise_distance(X, Y);
		Pre: X and Y are matrices (they may be the same objects)
		Post: p contains euclidean distances between pairs of X and Y
	*/
	Eigen::MatrixXd pairwise_distance(Eigen::MatrixXd X, Eigen::MatrixXd Y);

	Eigen::MatrixXd sum(Eigen::SparseMatrix<double> X, int axis);


	/*
		Use: auto clusters = AgglomerativeClustering(Dt, sA, Linkage);
		Pre: Dt is a distance matrix
			sA is a connectivity matrix (supra adjacency matrix without inter layer connections)
			Linkage is a string - for now only "average" is accepted
		Post: Produce a list of clusters from Dt using the agglomerative clustering method.
			NOTE: This implementation will only join clusters that are connected
			Linkage dictates what measure to use. For now only average distance is available
	*/
	std::vector<lart::cluster> AgglomerativeClustering(Eigen::SparseMatrix<double> Dt, Eigen::SparseMatrix<double> sA);

	/*
		Use: auto d = find_dist(Dt, sA);
		Pre: Dt is a distance matrix
			merges is the history of all merges
			sA is the connectivity matrix of Dt
		Post: dist is the smallest distance of nodes in Dt that is not 0 and are connected
	*/
	lart::dist find_dist(Eigen::SparseMatrix<double> Dt, std::vector<std::vector<int>> merges,
		Eigen::SparseMatrix<double> sA);

	/*
		Use: average_linkage(Dt, clusters, d);
		Pre: Dt is a supra adjacency distance matrix.
			clusters are the clusters generated so far
			d is the smallest distance of nodes in Dt
		Post: Dt is updated with new weights using the average metric
	*/
	void average_linkage(Eigen::SparseMatrix<double>& Dt, std::vector<lart::cluster> clusters, dist d);

	vector<double> modMLPX(vector<lart::cluster> x, std::vector<Eigen::SparseMatrix<double>> a,
		double gamma, Eigen::SparseMatrix<double> sA);

	/*
		Use: auto p = get_partition(clusters, maxmodix, L, N);
		Pre: clusters are the computed clusters of the agglomerative functions
			maxmodix is the index of the largest element of (?)
			L is the number of layers
			N is the amount of columns in L
		Post: p is the list of actors with their assigned cluster id's
	*/
	CommunitiesSharedPtr get_partition(MLNetworkSharedPtr mnet, vector<lart::cluster> clusters, int maxmodix, size_t L, size_t N);


};

}

#endif
