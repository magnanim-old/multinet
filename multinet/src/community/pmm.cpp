#include "community.h"

#include <SymEigsSolver.h>
#include <MatOp/SparseSymMatProd.h>

#include <Eigen/SVD>

#include <dlib/matrix.h>
#include <dlib/clustering.h>
#include <dlib/svm.h>
#include <dlib/matrix.h>
#include <random>


namespace mlnet {

CommunityStructureSharedPtr pmm::fit(MLNetworkSharedPtr mnet, unsigned int k, unsigned int ell) {
	//DTRACE3(PMM_START, mnet->name.c_str(), k, ell);
	std::vector<Eigen::SparseMatrix<double>> a = cutils::ml_network2adj_matrix(mnet);

    if (ell==1) ell=2;
    
	if (ell > (a[0].rows() - 1)) {
		ell = a[0].rows() - 1;
	}

    Eigen::MatrixXd features = Eigen::MatrixXd::Zero(a[0].rows(), ell * a.size());

    #pragma omp parallel for
    for (size_t i = 0; i < a.size(); i++) {
		features.block(0, i * ell, a[i].rows(), ell) = modularitymaximization(a[i], ell);
	}

    //DTRACE0(PMM_SVD_START);
    Eigen::JacobiSVD<Eigen::MatrixXd> svd(features, Eigen::ComputeThinU);
	features = svd.matrixU();
	//DTRACE0(PMM_SVD_END);
    
    if (features.cols() > (k - 1)) {
		features.conservativeResize(Eigen::NoChange, k - 1);
	}

    
    features.normalize();
	//DTRACE0(PMM_KMEANS_START);
	typedef dlib::matrix<double, 1, 0> sample_type;
	std::vector<sample_type> samples;

    
    for (int i = 0; i < features.rows(); ++i) {
		sample_type s;
		s.set_size(1, features.cols());
		for (int j = 0; j < features.cols(); ++j) {
			s(0, j) = features(i, j);
        }
		samples.push_back(s);
    }
    
    // assigning the initial centroids manually
    std::vector<sample_type> centers;
    for (unsigned int i=0; i<k; i++) {
        centers.push_back(samples.at(i));
    }
    //dlib::pick_initial_centers(k, centers, samples);
    dlib::find_clusters_using_kmeans(samples, centers);

    
    std::vector<unsigned int> partition(samples.size(), 0);
	for (unsigned long i = 0; i < samples.size(); ++i) {
		unsigned long best_idx = -1;
		double best_dist = 1e100;
		for (unsigned long j = 0; j < centers.size(); ++j) {
			if (dlib::length(samples[i] - centers[j]) < best_dist) {
				best_dist = length(samples[i] - centers[j]);
				best_idx = j;
			}
		}
		partition[i] = best_idx;
	}
    
    //DTRACE0(PMM_END);
	return cutils::actors2communities(mnet, partition);
}

Eigen::MatrixXd pmm::modularitymaximization(Eigen::SparseMatrix<double> a, unsigned int ell) {
	Eigen::MatrixXd d = cutils::sparse_sum(a, 1);

	unsigned int conv_speed = 2 * ell;
	if (conv_speed > a.rows()) {
		conv_speed = a.rows();
	}

	if (ell >= conv_speed) {
		if (conv_speed == a.rows()) {
			ell = ell - 1;
		} else {
			conv_speed = conv_speed + 1;
		}

	}

	matrix_vector_multiplication op(a, d);
	Spectra::SymEigsSolver<double, Spectra::LARGEST_ALGE, matrix_vector_multiplication> eigs(&op, ell, conv_speed);
	eigs.init();
	eigs.compute();
	return eigs.eigenvectors();
}

}
