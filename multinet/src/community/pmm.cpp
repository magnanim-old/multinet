#include "community/pmm.h"

#include <SymEigsSolver.h>
#include <MatOp/SparseSymMatProd.h>

#include <Eigen/SVD>

#include <dlib/matrix.h>
#include <dlib/clustering.h>

namespace mlnet {

CommunityStructureSharedPtr pmm::fit(MLNetworkSharedPtr mnet, unsigned int k, unsigned int ell, double gamma) {
	DTRACE4(PMM_START, mnet->name.c_str(), k, ell, std::to_string(gamma));
	std::vector<Eigen::SparseMatrix<double>> a = cutils::ml_network2adj_matrix(mnet);

	Eigen::MatrixXd features = Eigen::MatrixXd(a[0].rows(), ell * a.size());

	for (size_t i = 0; i < a.size(); i++) {
		try {
			features.block(0, i * ell, a[i].rows(), ell) = modularitymaximization(a[i], ell, gamma);
			DTRACE0(PMM_FEATURES_END);
		} catch (std::exception &e) {
			DTRACE1(PMM_ERROR, e.what());
			return community_structure::create();
		}
	}

	DTRACE0(PMM_SVD_START);
	//Eigen::BDCSVD<Eigen::MatrixXd> svd(features, Eigen::ComputeFullU);
	Eigen::JacobiSVD<Eigen::MatrixXd> svd(features, Eigen::ComputeFullU);
	DTRACE0(PMM_SVD_END);

	typedef dlib::matrix<double,0,1> sample_t;
	typedef dlib::radial_basis_kernel<sample_t> kernel_type;
	std::vector<sample_t> initial_centers;

	std::vector<sample_t> samples(features.rows());
	for (int i = 0; i < features.rows(); i++) {
		sample_t sample(features.cols());
		for (int j = 0; j < features.cols(); j++) {
			sample(j) = features.coeff(i, j);
		}
		samples[i] = sample;
	}

	DTRACE0(PMM_KMEANS_START);
	dlib::kcentroid<kernel_type> kc(kernel_type(0.1), 0.01, 8);
	dlib::kkmeans<kernel_type> test(kc);
	test.set_number_of_centers(k);

	pick_initial_centers(k, initial_centers, samples, test.get_kernel());
	test.train(samples, initial_centers);
	DTRACE0(PMM_END);

	std::vector<unsigned int> partition;
	for (unsigned long i = 0; i < samples.size(); ++i) {
		partition.push_back(test(samples[i]));
	}
	return cutils::actors2communities(mnet, partition);
}

Eigen::MatrixXd pmm::modularitymaximization(Eigen::SparseMatrix<double> a, unsigned int ell, double gamma) {
	std::vector<Eigen::SparseMatrix<double>> in = {a};
	double twoum = 0;

	Eigen::SparseMatrix<double> mod = cutils::ng_modularity(twoum, in, gamma, 1);

	if (ell > (a.rows() - 1)) {
		ell = a.rows() - 1;
	}

	unsigned int conv_speed = 2 * ell;
	if (conv_speed > a.rows()) {
		conv_speed = a.rows();
	}

	if (ell >= conv_speed) {
		if (conv_speed == a.rows()) {
			ell = ell -1;
		} else {
			conv_speed = conv_speed + 1;
		}

	}

	Spectra::SparseSymMatProd<double> op(mod);
	Spectra::SymEigsSolver<double, Spectra::LARGEST_ALGE, Spectra::SparseSymMatProd<double>> eigs(&op, ell, conv_speed);
	eigs.init();
	eigs.compute();
	return eigs.eigenvectors();
}

}
