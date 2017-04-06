#include "community/pmm.h"

#include <Eigen/Core>
#include <Eigen/SparseCore>
#include <GenEigsSolver.h>
#include <MatOp/SparseGenMatProd.h>

namespace mlnet {

CommunitiesSharedPtr pmm::get_ml_community(MLNetworkSharedPtr mnet, unsigned int k, unsigned int ell, unsigned int maxKmeans) {
	std::vector<Eigen::SparseMatrix<double>> a = cutils::ml_network2adj_matrix(mnet);

	for (size_t i = 0; i < a.size(); i++) {
		modularitymaximization(a[i], ell);
		break;
	}

	return communities::create();
}


void pmm::modularitymaximization(Eigen::SparseMatrix<double> a, unsigned int ell) {
	Eigen::MatrixXd d = cutils::sum(a, 2);
	double twom = d.array().sum();
	int n = d.rows();

	Spectra::SparseGenMatProd<double> op(a);
	Spectra::GenEigsSolver< double, Spectra::LARGEST_MAGN, Spectra::SparseGenMatProd<double> > eigs(&op, ell, n);

	// Initialize and compute
	eigs.init();
	int nconv = eigs.compute();

	// Retrieve results
	Eigen::VectorXd x;
	if(eigs.info() == Spectra::SUCCESSFUL)
		x = eigs.eigenvalues().real();


	std::cout << "Eigenvalues found:\n" << x << " " << x.cols() <<std::endl;
	std::cout << "Eigenvectors found:\n" << x << " " << eigs.eigenvectors() <<std::endl;


}

}
