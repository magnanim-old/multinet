#ifndef ACL_H_
#define ACL_H_

//#include "../multinet-clustering/multinet/include/multinet.h"
//#include <Eigen/Sparse>
//#include <Eigen/Dense>
#include <community.h>

namespace mlnet {

class acl {

 public:

  /*
    Use:
    acl a(MLNetworkSharedPtr,int,double,double);
    
    Pre:
    MLNetworkSharedPtr is an undirected multiplex network.
    Classical is an int indictating what random walk. 1 for classical, 0 for relaxed
    interlayerWeight is the interlayer weights of the random walks. If classical random walk is choosen, then interlayerweight should be between [0, inf). If relaxed: [0,1].
    teleport is a teleportration paratmeter for calculating node strength. 0 uses the largest eigenvector, > 0 uses a "pagerank" approach with random teleportation. Use > 0 for disconnected networks. 0.1 recommended.
    
    Post:
    a will be initiated with a transistion matrix and node strengths corresponding to ml with supplied random walk using interlayerWeight
  */
  acl(MLNetworkSharedPtr ml, int classical, double interlayerWeight, double teleport);


  /*
    Use:
    acl a(MLNetworkSharedPtr,int,double,double);
    CommunitySharedPtr c = a.get_community(double,double,std::vector<mlnet::NodeSharedPtr>);

    Pre: 
    teleport is a double indictating the teleportarion parameter of the personalized pagerank vector. Should be in the range (0,1). Values close to 1 indicate a low teleportation rate.
    epsilon is the truncation parameter of the personalized pagerank approximation. Should be between 1 and number of nodes in ml. 1 means a good approximation and big approximation.
    seeds is a vector of seeds.

    Post:
    CommunitySharedPtr corresponding to the communty found using teleport epsilon and seed
  */
  CommunitySharedPtr get_community(double teleport, double epsilon, std::vector<mlnet::NodeSharedPtr> seeds);
  
  /*
    Use:
    acl a(MLNetworkSharedPtr,int,double,double);
    CommunitiesSharedPtr cs = a.get_communities(double,double,std::vector<std::vector<mlnet::NodeSharedPtr>>);

    Pre: 
    teleport is a double indictating the teleportarion parameter of the personalized pagerank vector. Should be in the range (0,1). Values close to 1 indicate a low teleportation rate.
    epsilon is the truncation parameter of the personalized pagerank approximation. Should be between 1 and number of nodes in ml. 1 means a good approximation and big approximation.
    seeds is a vector of seeds.

    Post:
    CommunitiesSharedPtr where the communities correspond to the supplied seed nodes in the same order
  */
  mlnet::CommunitiesSharedPtr get_communities(double teleport, double epsilon, std::vector<std::vector<mlnet::NodeSharedPtr>> seeds);  

  
 private:
  size_t num_layers;
  size_t num_actors;
  size_t msize;
  std::vector<mlnet::NodeSharedPtr> mapping_attr;
  Eigen::SparseMatrix<double> transitionMatrix_attr;
  Eigen::SparseMatrix<double, Eigen::RowMajor> rm;
  Eigen::VectorXd stationaryDistribution_attr;
  Eigen::SparseMatrix<double> sparse_identity_matrix(size_t size);
  Eigen::VectorXd get_stationary(Eigen::SparseMatrix<double> P, Eigen::VectorXd s, double teleport);
  std::tuple<Eigen::SparseMatrix<double>, Eigen::VectorXd> get_relaxed(mlnet::MLNetworkSharedPtr ml, double re, double teleport);
  std::tuple<Eigen::SparseMatrix<double>, Eigen::VectorXd> get_classical(mlnet::MLNetworkSharedPtr ml, double interlayerWeight, double teleport);
  Eigen::VectorXd getSeeds(std::vector<mlnet::NodeSharedPtr> seeds);
  Eigen::VectorXd appr(Eigen::VectorXd s, double teleport, double epsilon);
  std::vector<Eigen::SparseMatrix<double>> ml_network2adj_matrix(mlnet::MLNetworkSharedPtr mnet);
  void col_normalize(Eigen::SparseMatrix<double>& A);
  void insert_sorted( std::vector<int> &cont, int value );
  std::tuple<std::vector<double>, std::vector<int>> sweep_cut(Eigen::VectorXd apprv);
  std::vector<mlnet::NodeSharedPtr> id2nodes(mlnet::MLNetworkSharedPtr ml2);
  std::tuple<double, std::vector<int>> get_smallest_conductance_cut(std::tuple<std::vector<double>, std::vector<int>> sweep_sets);
  void print_sweep_set(std::tuple<std::vector<double>, std::vector<int>> sweep_sets, bool print_sets);
  std::tuple<std::vector<double>, std::vector<int>> aclcut(Eigen::VectorXd seeds, double teleport, double epsilon);
  
  
};

}

#endif
