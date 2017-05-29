#ifndef LART_H_
#define LART_H_

#include "cutils.h"
#include <dlib/clustering.h>

namespace mlnet {

class lart {

public:

	/*
		Use : lart k;
			 k.get_ml_community(MLNetworkSharedPtr, uint32_t, double, double);
		Pre : MLNetworkSharedPtr is a multilayer network
			 t is the number of number of steps for random walker to take
			 eps
  		Post: (?)
	*/
	CommunityStructureSharedPtr fit(MLNetworkSharedPtr mnet, int t, double eps, double gamma);

private:

	void exp_by_squaring_iterative(Eigen::SparseMatrix<double>& x, int t);

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
		Use: auto A = supraA(a, eps);
		Pre: a is a list of adjacency matrixes. eps is the probability for the walker to move between layers
		Post: A is the supra adjacency matrix from the list of matrixes, with inter layer connections
			on the off diagonal blocks.

	*/
	Eigen::SparseMatrix<double> supraA(std::vector<Eigen::SparseMatrix<double>> a, double eps);

	/*
		Use: auto m = block_diag(a);
		Pre: a is a list of adjacency matrixes
		Post: m is a a.size() * a[i].size() matrix, with the matrix contents of a
			stacked diagonally
	*/
	Eigen::SparseMatrix<double> block_diag(std::vector<Eigen::SparseMatrix<double>> a, double eps);

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
	Eigen::MatrixXd Dmat(Eigen::SparseMatrix<double> Pt, Eigen::SparseMatrix<double> D, size_t L);

	/*
		Use: auto p = pairwise_distance(X, Y);
		Pre: X and Y are matrices (they may be the same objects)
		Post: p contains euclidean distances between pairs of X and Y
	*/
	Eigen::MatrixXd pairwise_distance(Eigen::MatrixXd X, Eigen::MatrixXd Y, bool same_obj);

	unsigned long prcheck(Eigen::SparseMatrix<double>& aP, std::vector<dlib::sample_pair> edges, unsigned int LN);


	int is_connected(std::vector<Eigen::SparseMatrix<double>> a, std::vector<dlib::sample_pair>& edges);


	void updateDt(Eigen::MatrixXd& Dt, std::vector<Eigen::SparseMatrix<double>>);


	std::vector<unsigned long> find_ix(std::vector<unsigned long> x, unsigned long y);

	vector<double> modMLPX(vector<dlib::bu_cluster> clusters, std::vector<Eigen::SparseMatrix<double>> a, double gamma);

	void modmat(std::vector<Eigen::SparseMatrix<double>> a, double gamma, Eigen::SparseMatrix<double>& sA);

	vector<unsigned int> get_partition(vector<dlib::bu_cluster> clusters, int maxmodix, size_t L, size_t N);


};

}

#endif
