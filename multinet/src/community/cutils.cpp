
#include "community.h"

namespace mlnet {
    
    Eigen::SparseMatrix<double> cutils::supraA(std::vector<Eigen::SparseMatrix<double>> a, double eps, bool use_node_degrees, bool use_self_loop) {
        Eigen::SparseMatrix<double> A;
        
        if (use_self_loop) {
            A = block_diag(a, eps);
        } else {
            A = block_diag(a, 0);
        }
        
        
        size_t L = a.size();
        size_t N = a[0].rows();
        
        for (size_t i = 0; i  < L - 1; ++i) {
            for (size_t j = i + 1; j < L; ++j) {
                Eigen::MatrixXd d = cutils::sparse_sum(a[i].cwiseProduct(a[j]), 1);
                
                std::vector<Eigen::Triplet<double>> tlist;
                tlist.reserve(a[0].rows());
                
                for (int k = 0; k < A.outerSize(); k++) {
                    for (Eigen::SparseMatrix<double>::InnerIterator it(A, k); it; ++it) {
                        tlist.push_back(Eigen::Triplet<double>(it.row(), it.col(), it.value()));
                    }
                }
                
                int ix_i = i * N;
                int ix_j = j * N;
                
                if (use_node_degrees) {
                    for (int k = 0; k < a[i].rows(); k++) {
                        double intra = d(k, 0) + eps;
                        tlist.push_back(Eigen::Triplet<double>((ix_i + k), (ix_j + k), intra));
                        tlist.push_back(Eigen::Triplet<double>((ix_j + k), (ix_i + k), intra));
                    }
                } else {
                    for (int k = 0; k < a[i].rows(); k++) {
                        tlist.push_back(Eigen::Triplet<double>((ix_i + k), (ix_j + k), eps));
                        tlist.push_back(Eigen::Triplet<double>((ix_j + k), (ix_i + k), eps));
                    }
                }
                A.setFromTriplets(tlist.begin(), tlist.end());
                
            }
        }
        return A;
    }
    
    Eigen::SparseMatrix<double> cutils::block_diag(std::vector<Eigen::SparseMatrix<double>> a, double eps) {
        Eigen::SparseMatrix<double> m = Eigen::SparseMatrix<double>(
                                                                    a[0].rows() * a.size(), a[0].cols() * a.size());
        
        int nnz = 0;
        for (auto l: a)
            nnz += l.nonZeros();
        
        size_t r, c;
        r = 0;
        c = 0;
        
        std::vector<Eigen::Triplet<double>> tlist;
        tlist.reserve(nnz);
        
        for (size_t i = 0; i < a.size(); i++) {
            for (int j = 0; j < a[i].outerSize(); j++) {
                for (Eigen::SparseMatrix<double>::InnerIterator it(a[i], j); it; ++it) {
                    tlist.push_back(Eigen::Triplet<double>(r + it.row(), c + it.col(), it.value()));
                }
            }
            r += a[i].rows();
            c += a[i].cols();
        }
        
        for (int i = 0; i < m.rows(); i++) {
            tlist.push_back(Eigen::Triplet<double>(i, i, eps));
        }
        
        m.setFromTriplets(tlist.begin(), tlist.end());
        return m;
    }
    
    
    
    
    Eigen::SparseMatrix<double> cutils::ng_modularity(double &twoum, std::vector<Eigen::SparseMatrix<double>> a, double gamma, double omega) {
        size_t L = a.size();
        size_t N = a[0].rows();
        
        Eigen::SparseMatrix<double> B(N * L, N * L);
        B.reserve(Eigen::VectorXi::Constant(N * L, N * L));
        
        twoum = 0.0;
        
        std::vector<Eigen::Triplet<double>> tlist;
        tlist.reserve(N * L);
        
        for (size_t i = 0; i < L; i++) {
            Eigen::MatrixXd kout = cutils::sparse_sum(a[i], 0);
            Eigen::MatrixXd kin = cutils::sparse_sum(a[i], 1);
            double mm = kout.array().sum();
            twoum = twoum + mm;
            
            Eigen::SparseMatrix<double> tmp, tmp1;
            tmp = (Eigen::SparseMatrix<double>(a[i].transpose()) + a[i]) / 2;
            
            Eigen::MatrixXd tmp2 = kin * kout.transpose();
            Eigen::MatrixXd tmp3 = gamma / 2 * (tmp2 + tmp2)/ mm;
            
            tmp = tmp - tmp3.sparseView();
            
            for (size_t j = 0; j < N; j++) {
                for (size_t k = 0; k < N; k++) {
                    tlist.push_back(
                                    Eigen::Triplet<double>(j + (i * N), k + (i * N), tmp.coeff(j, k)));
                }
            }
        }
        
        for (size_t i = 0; i  < L - 1; ++i) {
            for (size_t j = i + 1; j < L; ++j) {
                int ix_i = i * N;
                int ix_j = j * N;
                
                for (int k = 0; k < a[i].rows(); k++) {
                    tlist.push_back(Eigen::Triplet<double>((ix_i + k), (ix_j + k), omega));
                    tlist.push_back(Eigen::Triplet<double>((ix_j + k), (ix_i + k), omega));
                }
            }
        }
        B.setFromTriplets(tlist.begin(), tlist.end());
        
        twoum = twoum + (N * L * (L - 1) * omega);
        return B;
    }
    
    CommunityStructureSharedPtr cutils::nodes2communities(MLNetworkSharedPtr mnet, std::vector<unsigned int> nodes2cid) {
        size_t L = mnet->get_layers()->size();
        size_t N = mnet->get_actors()->size();
        
        hash_map<long, std::set<NodeSharedPtr> > result;
        
        for (size_t i = 0; i < L; i++) {
            LayerSharedPtr l = mnet->get_layers()->get_at_index(i);
            
            for (size_t j = i * N; j < (1 + i) * N; j++) {
                ActorSharedPtr a = mnet->get_actors()->get_at_index(j - (i * N));
                
                NodeSharedPtr n = mnet->get_node(a,l);
                if (n){
                    result[nodes2cid[j]].insert(n);
                }
            }
        }
        
        CommunityStructureSharedPtr communities = community_structure::create();
        
        for (auto pair: result) {
            CommunitySharedPtr c = community::create();
            for (NodeSharedPtr node: pair.second) {
                c->add_node(node);
            }
            communities->add_community(c);
        }
        
        return communities;
    }
    
    CommunityStructureSharedPtr cutils::actors2communities(MLNetworkSharedPtr mnet, std::vector<unsigned int> actors2cid) {
        hash_map<long, std::set<NodeSharedPtr> > result;
        
        for (size_t i = 0; i < actors2cid.size(); i++) {
            ActorSharedPtr a = mnet->get_actors()->get_at_index(i);
            
            for (auto l : *mnet->get_layers()) {
                NodeSharedPtr n = mnet->get_node(a,l);
                if (n) {
                    result[actors2cid[i]].insert(n);
                }
            }
        }
        
        CommunityStructureSharedPtr communities = community_structure::create();
        
        for (auto pair: result) {
            CommunitySharedPtr c = community::create();
            for (NodeSharedPtr node: pair.second) {
                c->add_node(node);
            }
            communities->add_community(c);
        }
        
        return communities;
    }
    
    Eigen::MatrixXd cutils::sparse_sum(Eigen::SparseMatrix<double> X, int axis) {
        Eigen::MatrixXd d = Eigen::MatrixXd::Zero(X.rows(), 1);
        for (int i = 0; i < X.outerSize(); i++) {
            for (Eigen::SparseMatrix<double>::InnerIterator it(X, i); it; ++it) {
                if (axis){
                    d(it.col(), 0) = it.value() + d(it.col(), 0);
                } else {
                    d(it.row(), 0) = it.value() + d(it.row(), 0);
                }
            }
        }
        return d;
    }
    
    std::vector<Eigen::SparseMatrix<double>> cutils::ml_network2adj_matrix(MLNetworkSharedPtr mnet) {
        size_t L = mnet->get_layers()->size();
        size_t N = mnet->get_actors()->size();
        
        std::vector<Eigen::SparseMatrix<double>> a(L);
        
        for (LayerSharedPtr l: *mnet->get_layers()) {
            Eigen::SparseMatrix<double> m = Eigen::SparseMatrix<double> (N, N);
            
            std::vector<Eigen::Triplet<double>> tlist;
            tlist.reserve(mnet->get_edges()->size());
            
            for (EdgeSharedPtr e: *mnet->get_edges(l, l)) {
                int v1_id = mnet->get_actors()->get_index(e->v1->actor);
                int v2_id = mnet->get_actors()->get_index(e->v2->actor);
                
                tlist.push_back(Eigen::Triplet<double>(v1_id, v2_id, 1));
                
                if (!(e->directionality)) {
                    tlist.push_back(Eigen::Triplet<double>(v2_id, v1_id, 1));
                }
                
            }
            
            m.setFromTriplets(tlist.begin(), tlist.end());
            a[l->id - 1] = m;
        }
        return a;
    }
    
    std::vector<int> cutils::unique(std::vector<int> y) {
        std::sort(y.begin(), y.end());
        std::vector<int>::iterator it;
        it = std::unique(y.begin(), y.end());
        y.resize(std::distance(y.begin(), it));
        return y;
    }
    
    std::vector<int> cutils::range(size_t size, bool randomize) {
        std::vector<int> range(size);
        std::iota(range.begin(), range.end(), 0);
        if (randomize) {
            std::random_shuffle (range.begin(), range.end());
        }
        return range;
    }
    
}