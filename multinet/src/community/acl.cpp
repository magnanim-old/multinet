#include <iostream>
#include <Eigen/Sparse>
#include <Eigen/Dense>
#include <MatOp/SparseGenMatProd.h>
#include <GenEigsSolver.h>
#include <assert.h>
#include <queue>
#include <tuple>
#include <ctime>
#include <algorithm>
#include <community.h>
#include <stdexcept> 

using namespace Eigen;

namespace mlnet {

  /////////////////
  // CONSTRUCTOR //
  /////////////////
  
  acl::acl(MLNetworkSharedPtr ml, int random_walk, double interlayer, double tp){
    num_layers = ml->get_layers()->size();
    num_actors = ml->get_actors()->size();
    msize = num_layers*num_actors;
    std::tuple<SparseMatrix<double>, VectorXd> trans_and_stat;
    mapping_attr = id2nodes(ml);
  
    if(random_walk == 1){
      //std::cout << "CLASSICAL" << std::endl;
      trans_and_stat = get_classical(ml, interlayer,tp);
    }else{
      //std::cout << "RELAXED" << std::endl;
      trans_and_stat = get_relaxed(ml, interlayer,tp);
    }
  
    transitionMatrix_attr = std::get<0>(trans_and_stat);
    rm = transitionMatrix_attr;
    stationaryDistribution_attr = std::get<1>(trans_and_stat)*msize;
  }

  ///////////////////////////////
  // RELAXED RANDOM WALK SETUP //
  ///////////////////////////////
  
  //Returns transition matrix and node strengths for the classical relaxed walk
  std::tuple<SparseMatrix<double>, VectorXd> acl::get_relaxed(MLNetworkSharedPtr ml, double re, double teleport){
    std::vector<Eigen::SparseMatrix<double>> am = ml_network2adj_matrix(ml);
    Eigen::SparseMatrix<double> m = Eigen::SparseMatrix<double>(msize,msize);
    std::vector<Eigen::Triplet<double>> tlist;
    std::vector<std::vector<int>> relaxed_edges(num_actors);
    std::vector<std::vector<int>> layer_edges(msize);
    size_t r, c;
    VectorXd stat_dense(num_layers);
    VectorXd kin = VectorXd::Zero(msize);
 
    //Get triplets
    r = 0;
    c = 0;
    for (size_t i = 0; i < am.size(); i++) {
      for (int j = 0; j < am[i].outerSize(); j++) {
	for (Eigen::SparseMatrix<double>::InnerIterator it(am[i], j); it; ++it) {
	  relaxed_edges[it.col()].push_back(r+it.row());
	  layer_edges[c + it.col()].push_back(r+it.row());
	  kin(i*num_actors + j) += it.value();
	}
      }
      r += am[i].rows();
      c += am[i].cols();
    }
  
    //Add interlayer
    for(size_t i = 0; i < relaxed_edges.size(); i++){
      for(size_t j = 0; j < relaxed_edges[i].size(); j++){
	for(size_t l = 0; l < num_layers; l++){
	  tlist.push_back(Triplet<double>(relaxed_edges[i][j], l*num_actors + i, re/(double)relaxed_edges[i].size()));
	}
      }
    }

    //Add intralayer
    for(size_t i = 0; i < layer_edges.size(); i++){
      for(size_t j = 0; j < layer_edges[i].size(); j++){
	tlist.push_back(Triplet<double>(layer_edges[i][j],i, (1-re)/(double)layer_edges[i].size()));
      }
    }

    //Build matrix
    m.setFromTriplets(tlist.begin(), tlist.end());

    //Get stationary
    stat_dense = get_stationary(m,kin,teleport); //OBS
  
    return std::make_tuple(m, stat_dense);
  }

  /////////////////////////////////
  // CLASSICAL RANDOM WALK SETUP //
  /////////////////////////////////
  
  //Returns transition matrix and node strengths for the classical random walk
  std::tuple<SparseMatrix<double>, VectorXd> acl::get_classical(MLNetworkSharedPtr ml, double interlayerWeight, double teleport){
    size_t r, c;
    r = 0;
    c = 0;
    std::vector<Eigen::Triplet<double>> tlist;
    std::vector<Eigen::SparseMatrix<double>> am = ml_network2adj_matrix(ml);
    SparseMatrix<double> sup = SparseMatrix<double>(am[0].rows() * am.size(), am[0].cols() * am.size());
    VectorXd kin = VectorXd::Ones(msize)*(num_layers-1);
  
    //ADD ADJ MATRICES
    for (size_t i = 0; i < am.size(); i++) {
      for (int j = 0; j < am[i].outerSize(); j++) {
	for (Eigen::SparseMatrix<double>::InnerIterator it(am[i], j); it; ++it) {
	  tlist.push_back(Eigen::Triplet<double>(r + it.row(), c + it.col(), it.value()));
	  kin(i*num_actors + j) += it.value();
	}
      }
      r += am[i].rows();
      c += am[i].cols();
    }

    //ADD INTERLAYER EDGES
    for(size_t a = 0; a < num_actors; a++){
      for(size_t l1 = 0; l1 < num_layers; l1++){
	for(size_t l2 = 0; l2 < num_layers; l2++){
	  if(l1 == l2) continue;
	  tlist.push_back(Eigen::Triplet<double>(a + num_actors*l2, l1*num_actors + a, interlayerWeight));
	}
      }
    }

    //Build matrix
    sup.setFromTriplets(tlist.begin(), tlist.end());
  
    //Column normalize for transition matrix
    col_normalize(sup);
  
    return std::make_tuple(sup, get_stationary(sup,kin,teleport));
  }
  
  ////////////////
  // STATIONARY //
  ////////////////
  
  VectorXd acl::get_stationary(SparseMatrix<double> P, VectorXd s, double teleport){
    //std::cout << "stationary..." << std::endl;
    VectorXd te;

    //Use largest eigen if no teleport
    if(teleport==0){
      Spectra::SparseGenMatProd<double> op(P);
      Spectra::GenEigsSolver<double, Spectra::LARGEST_MAGN, Spectra::SparseGenMatProd<double>> eigs(&op, 1, 3);
      eigs.init();
      eigs.compute();
      te = eigs.eigenvectors().real();
    }

    //Else use a pagerank approach with random tp
    else{
      BiCGSTAB<SparseMatrix<double>> solver;
      solver.compute(sparse_identity_matrix(P.cols()) + (teleport-1)*P);
      te = solver.solve(teleport*s);
    }
  
    if(te.sum() < 0){
      te = te*(-1);
    }

    for(unsigned int i = 0; i < te.size(); i++){
      te(i) = std::max((double)0, te(i));
    }
  
    te = te/te.sum();;
    //std::cout << te << std::endl;
    return te;
  }

  //////////
  // APPR //
  //////////
  
  //Personalized PageRank vector approximation
  VectorXd acl::appr(VectorXd s, double teleport, double epsilon){
    //std::cout << "appr..." << std::endl;
    size_t size = msize;
    VectorXd apprv = VectorXd::Zero(size);
    VectorXd residual = s;
    teleport = (1-teleport)/(1+teleport); //Lazy tp
    int count = 0;
    int max_iter = 1000000000; //Will stop after this amount of iterations and print warning. Maybe should be a param.
    int current;
    double oldResidual;
    double delta;
  
    std::queue<int> q;
    for(size_t i = 0; i < size; i++){
      if(residual(i) >= epsilon*stationaryDistribution_attr(i)){
	q.push(i);
      }
    }
  
    while(!q.empty() && count < max_iter){
      count++;
      current = q.front();
      q.pop();
      oldResidual = residual(current);
      apprv(current) += teleport*oldResidual;
      residual(current) = (1-teleport)*oldResidual/2;

      //reinsert current if appropriate
      if(residual(current) > epsilon*stationaryDistribution_attr(current)){
	q.push(current);
      }
    
      //Push to all neighbors (out edges)
      for (SparseMatrix<double>::InnerIterator i(transitionMatrix_attr, current); i; ++i){
	delta = (1-teleport)*i.value()*oldResidual/2;
	residual(i.index()) += delta;
	if(0 < residual(i.index()) - epsilon*stationaryDistribution_attr(i.index()) && residual(i.index()) - epsilon*stationaryDistribution_attr(i.index()) <= delta){
	  q.push(i.index());
	}
      }
    }
    //std::cout << "---Apprv iteration: " << count << " ------ apprv sum: " << apprv.sum() << " --------- res sum: " << residual.sum() << std::endl;
    //std::cout << apprv << std::endl;
      if(count == max_iter)
          ;   /* TODO throw warning: max iterations reached */
    return apprv;
  }

  ///////////////
  // SWEEP CUT //
  ///////////////
  
  //returns conductances of the sweeps and the ordered pprv; so conductance[i] corresponds to pprv[0:i-1]
  std::tuple<std::vector<double>, std::vector<int>> acl::sweep_cut(VectorXd apprv){
    size_t size = msize;
    double mm = stationaryDistribution_attr.sum();
    std::vector<int> S;
    std::vector<int> S_sorted;
    std::vector<double> conductances;
    double vol = 0;
    double volc = mm;
    double c = 0;
    double e;
    size_t nonzeroes = 0;
  
    //CONVERT TO NORMAL VECTOR
    std::vector<std::tuple<double, int>> nodes;
    for (size_t i=0; i< size; ++i){
      nodes.push_back(std::make_tuple(apprv(i), i));
      if(stationaryDistribution_attr(i) > 0) nonzeroes++;
    }
  
    //SORT
    std::sort(begin(nodes), end(nodes), 
	      [](std::tuple<double, int> const &t1, std::tuple<double, int> const &t2) {
		return std::get<0>(t1) > std::get<0>(t2); // or use a custom compare function
	      });

    size_t ss_count;
    std::vector<int> S_complement;
    for(size_t i = 0; i < nodes.size(); i++){
      S_complement.push_back(i);
    }
  
    //Calculate conductances for each sweep - set
    for(size_t i = 0; i < nodes.size(); i++){
      if(std::get<0>(nodes[i]) == 0) break;
      int ia = std::get<1>(nodes[i]);
    
      //Remove Prob that you go from S to ia from conductance
      ss_count = 0;
      for (Eigen::SparseMatrix<double,RowMajor>::InnerIterator it(rm, ia); it; ++it) {
	if(S_sorted.size() > 0){
	  while(ss_count < S_sorted.size() && it.col() > S_sorted[ss_count]){
	    ss_count++;
	  }
	  if(ss_count >= S_sorted.size()){
	    break;
	  }
	  if(it.col() == S_sorted[ss_count]){
	    c -= it.value() * stationaryDistribution_attr(it.col());
	  }
	}
      }

      //Add prob that you go to other nodes than in S from ia to conductance
      ss_count = 0;
      for (Eigen::SparseMatrix<double>::InnerIterator it(transitionMatrix_attr, ia); it; ++it) {
	if(S_complement.size() > 0){
	  while(ss_count < S_complement.size() && it.row() > S_complement[ss_count]){
	    ss_count++;
	  }
	  if(ss_count >= S_complement.size()){
	    break;
	  }
	  if(it.row() == S_complement[ss_count]){
	    c += it.value() * stationaryDistribution_attr(ia);
	  }
	}
      }
    
      vol += stationaryDistribution_attr(ia);
      volc = mm-vol;

      //round to zero for numerical errors
      if (std::abs(c) < 0.00000001) c = 0; 
    
      //whole network == conductance 1
      e = c/std::min(vol,volc);
      if(i == nonzeroes -1){
	conductances.push_back(1);
      }else{
	conductances.push_back(e);
      }
      S.push_back(ia);
      insert_sorted(S_sorted,ia);
      S_complement.erase(std::remove(S_complement.begin(), S_complement.end(), ia), S_complement.end());
    }

    //std::cout << "nodes in approximation: " << S.size() << std::endl;
    return std::make_tuple(conductances,S);
  }

  ////////////
  // ACLCUT //
  ////////////
  
  //Returns sweep sets
  std::tuple<std::vector<double>, std::vector<int>> acl::aclcut(VectorXd seeds, double teleport, double epsilon){

    //GET APPRV
    VectorXd apprv(transitionMatrix_attr.rows());
    apprv = appr(seeds, teleport, epsilon);

    //Rescale apprv and set nodes with zero node strength to 0
    for(unsigned int i = 0; i < apprv.size(); i++){
      apprv(i) = apprv(i)/stationaryDistribution_attr(i);
      if(std::isnan(apprv(i)) || std::isinf(apprv(i)))
	apprv(i) = 0;
    }

    //GET SWEEP SETS
    return sweep_cut(apprv);
  }

  /////////////////////////////
  // GET_COMMUNTITY          //
  // -Wrappers around aclcut //
  /////////////////////////////
  
  std::tuple<CommunitySharedPtr, double> acl::get_community_conductance(double teleport, double epsilon, std::vector<NodeSharedPtr> seeds){
    assert(epsilon >= 1);
    epsilon = epsilon/msize;
    std::tuple<std::vector<double>, std::vector<int>> sweep_set;
    VectorXd seed_dense;
    std::tuple<double, std::vector<int>> cut;
    std::vector<int> c;
    size_t csize;
    CommunitySharedPtr com;
  
    seed_dense = getSeeds(seeds);
    sweep_set = aclcut(seed_dense, teleport, epsilon);
    //print_sweep_set(sweep_set,false);
    cut = get_smallest_conductance_cut(sweep_set); //(conductance, set of nodes)
    c = std::get<1>(cut);
    csize = c.size();
    com = community::create();
    for(size_t i = 0; i < csize; i++){
      if(mapping_attr[c[i]])
	com->add_node(mapping_attr[c[i]]);
    }
  
    return std::make_tuple(com,std::get<0>(cut));
  }

  CommunitySharedPtr acl::get_community(double teleport, double epsilon, std::vector<NodeSharedPtr> seeds){
    return std::get<0>(get_community_conductance(teleport, epsilon, seeds));
  }

  //////////////////////
  // HELPER FUNCTIONS //
  //////////////////////
  
  //Returns a square sparse identity matrix of size size
  SparseMatrix<double> acl::sparse_identity_matrix(size_t size){
    SparseMatrix<double> id = SparseMatrix<double>(size,size);
    std::vector<Eigen::Triplet<double>> tlist;
    for(size_t i = 0; i < size; i++){
      tlist.push_back(Triplet<double>(i, i, 1));
    }
    id.setFromTriplets(tlist.begin(), tlist.end());
    return id;
  }

  //Normalizes the columns of A
  void acl::col_normalize(SparseMatrix<double>& A){
    std::vector<double> colSums(A.rows());
    size_t size = A.rows();
  
    for (size_t i = 0; i < size; ++i) {
      colSums[i] = A.col(i).sum();
    }

    for(size_t i = 0; i < size; ++i) {
      for (SparseMatrix<double>::InnerIterator it(A,i); it; ++it){
	it.valueRef() = it.value()/colSums[i];
      }    
    }
  }

  //Translates a mlnetnetwork to a vector of adjacency matrices (one for each layer)
  std::vector<Eigen::SparseMatrix<double>> acl::ml_network2adj_matrix(MLNetworkSharedPtr mnet) {
    size_t N = mnet->get_layers()->size();
    size_t M = mnet->get_actors()->size();
    std::vector<Eigen::SparseMatrix<double>> adj(N);
  
    for (LayerSharedPtr l: *mnet->get_layers()) {
      Eigen::SparseMatrix<double> m = Eigen::SparseMatrix<double> (M, M);
      std::vector<Eigen::Triplet<double>> tlist;
      for (EdgeSharedPtr e: *mnet->get_edges(l, l)) {
	int v1_id = e->v1->actor->id;
	int v2_id = e->v2->actor->id;
	//if undirected push edge both ways
	if(!(e->directionality)) tlist.push_back(Triplet<double>(v1_id - 1, v2_id - 1, 1));
	tlist.push_back(Triplet<double>(v2_id - 1, v1_id - 1, 1));
      }
      m.setFromTriplets(tlist.begin(), tlist.end());
      adj[l->id - 1] = m;
    }

    return adj;
  }

  //Takes seed nodes and creates a probability vector normalized over the seeds
  //Example: {1 0 0 1 1 0 1} -> {0.25 0 0 0.25 0.25 0 0.25}
  VectorXd acl::getSeeds(std::vector<NodeSharedPtr> seeds){
    assert(seeds.size() > 0);
    VectorXd seedVector = VectorXd::Zero(msize);

    for(NodeSharedPtr n : seeds) {
      seedVector(((n->layer->id - 1)*num_actors)+n->actor->id - 1) = 1/(double)seeds.size();
    }
  
    return seedVector;  
  }

  //Insertion sort into cont
  void acl::insert_sorted( std::vector<int> &cont, int value ) {
    std::vector<int>::iterator it = std::lower_bound( cont.begin(), cont.end(), value, std::less<int>() );
    cont.insert( it, value ); // insert before iterator it
  }

  //Creates a mapping of matrix IDs to MLnetwork-node ID's
  std::vector<NodeSharedPtr> acl::id2nodes(MLNetworkSharedPtr ml2){
    //std::vector<NodeSharedPtr> mapping(ml2->get_nodes()->size());
    std::vector<NodeSharedPtr> mapping(msize,nullptr);
  
    //size_t num_actors = ml2->get_actors()->size();
    for(NodeSharedPtr n : *ml2->get_nodes()){
      mapping[(n->layer->id-1)*num_actors + n->actor->id -1] = n;
    }
    return mapping;
  }

  //Takes a sweep cut and returns the smallest conductance subset with corresponding conductance as (conductance, subset)
  std::tuple<double, std::vector<int>> acl::get_smallest_conductance_cut(std::tuple<std::vector<double>, std::vector<int>> sweep_sets){
    std::vector<double> cs = std::get<0>(sweep_sets);
    std::vector<int> pprv = std::get<1>(sweep_sets);

    //If sweep set empty
    if(std::get<1>(sweep_sets).size() == 0){
      pprv.resize(0);
      return std::make_tuple(1, pprv);
    }
  
    size_t i = min_element(cs.begin(), cs.end()) - cs.begin();
    pprv.resize(i+1);
    
    return std::make_tuple(cs[i], pprv);
  }

  //Prints a sweep set
  void acl::print_sweep_set(std::tuple<std::vector<double>, std::vector<int>> sweep_sets, bool print_sets) {

      /*
    std::vector<double> cs = std::get<0>(sweep_sets);
    std::vector<int> pprv = std::get<1>(sweep_sets);

      
    std::cout << "PRINT STUFF" << std::endl;
    
    double mini = 1;
    size_t i_mini = 0;
    
    for(size_t i = 0; i < cs.size(); i++){
      std::cout << i+1 << " " << "Conductance: " << cs[i] << " Set: {";

      if(cs[i] < mini){
	mini = cs[i];
	i_mini = i;
      }
      
      if(print_sets){
	for(size_t j = 0; j <= i; j++){
	  std::cout << ", a" << pprv[j]%num_actors +1 << "l" << pprv[j]/num_actors;
	}
      }
      std::cout << "}" << std::endl;
    }
    if(cs.size() != 0)
      std::cout << "Lowest conductance: " << i_mini+1 << " " << cs[i_mini] << std::endl;  
     */
  }
    
  
}
