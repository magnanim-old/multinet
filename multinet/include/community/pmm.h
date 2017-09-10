#ifndef PMM_H_
#define PMM_H_

namespace mlnet {

class pmm {

public:

	/*
		Data Invariant:
			PMM is a multiplex community detector based on the paper Uncovering Groups via Heterogeneous Interaction Analysis by
			Ltangasuedu, E and Wang, X (http://leitang.net/heterogeneous_network.html).
			It uses http://spectralib.org/ to find top eigenvectors of the modularity matrix
	*/

	/*
		Use : pmm p;
			  p.fit(mnet, k, ell);
		Pre : mnet is a multilayer network
			  k is the amount of clusters for kmeans to assign
			  ell is the amount of structural features to extract from each layer.
		Post: Return the communities that each actor in mnet belongs to.
	*/
	CommunityStructureSharedPtr fit(MLNetworkSharedPtr mnet, unsigned int k, unsigned int ell);

	/*
		Use: Eigen::MatrixXd m = modularitymaximization()
		Pre: a is an adjacency matrix from a layer in mnet
			ell is the amount of structural features to extract
		Post: MatrixXd contains the top eigenvectors of a
	*/
	Eigen::MatrixXd modularitymaximization(Eigen::SparseMatrix<double> a, unsigned int ell);


	class matrix_vector_multiplication {

		/*
			Data Invariant:
				Compute the top eigenvectors of the modularity matrix using the Lanczos method.
		*/

		public:
			Eigen::SparseMatrix<double> a;
			Eigen::MatrixXd d;
			double twoum;

			typedef Eigen::Matrix<double, Eigen::Dynamic, 1> Vector;
			typedef Eigen::Map<const Vector> MapConstVec;
			typedef Eigen::Map<Vector> MapVec;

			matrix_vector_multiplication(Eigen::SparseMatrix<double> _a, Eigen::MatrixXd _d) {
				a = _a;
				d = _d;
				twoum = d.array().sum();
			}

			int rows() { return a.rows(); }
			int cols() { return a.cols(); }

			void perform_op(double *x_in, double *y_out) {
				MapConstVec x(x_in, a.cols());
				MapVec y(y_out, a.rows());
				y.noalias() = a * x - (x * d.transpose()) / twoum * d;
			}
	};


};

}

#endif /* PMM_H_ */
