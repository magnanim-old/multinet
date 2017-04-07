#include "community/pmm.h"

#include <SymEigsSolver.h> 
#include <MatOp/SparseSymMatProd.h>
#include <Eigen/SVD>


#include <dlib/matrix.h>
#include <dlib/clustering.h>

namespace mlnet {

CommunitiesSharedPtr pmm::get_ml_community(MLNetworkSharedPtr mnet, unsigned int k, unsigned int ell, unsigned int maxKmeans) {
	std::vector<Eigen::SparseMatrix<double>> a = cutils::ml_network2adj_matrix(mnet);

	Eigen::MatrixXd features = Eigen::MatrixXd::Zero(a[0].rows(), a[0].cols() * a.size());

	for (size_t i = 0; i < a.size(); i++) {
		features.block(0, i * a[i].rows(), a[i].rows(), a[i].cols()) = modularitymaximization(a[i], ell);
	}


	Eigen::BDCSVD<Eigen::MatrixXd> svd(features, Eigen::ComputeFullU);

	typedef dlib::matrix<double,0,0> sample_t;
    typedef dlib::radial_basis_kernel<sample_t> kernel_type;
    std::vector<sample_t> initial_centers;




	dlib::kcentroid<kernel_type> kc(kernel_type(0.1),0.01, 8);
    dlib::kkmeans<kernel_type> test(kc);
	test.set_number_of_centers(k);

	dlib::matrix<double, 0, 0> f = dlib::mat(svd.matrixU());
	f[0][0];
	//pick_initial_centers(k, initial_centers, f, test.get_kernel());
	test.train(f, initial_centers);


	std::cout << f << std::endl;

	return communities::create();
}

Eigen::MatrixXd pmm::modularitymaximization(Eigen::SparseMatrix<double> a, unsigned int ell) {
	Spectra::SparseSymMatProd<double> op(a);
	Spectra::SymEigsSolver< double, Spectra::LARGEST_ALGE, Spectra::SparseSymMatProd<double> > eigs(&op, ell, a.rows());
	eigs.init();
	eigs.compute();
	return eigs.eigenvectors();
}

}
