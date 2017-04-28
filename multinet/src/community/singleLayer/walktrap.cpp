#include "community/singleLayer/walktrap.h"
#include <ctime>
#include <set>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <string>

using namespace std;

namespace mlnet{

int WalkTrapProbabilities::length = 0;
WalkTrapCommunities* WalkTrapProbabilities::C = 0;
float* WalkTrapProbabilities::tmp_vector1 = 0;
float* WalkTrapProbabilities::tmp_vector2 = 0;
int* WalkTrapProbabilities::id = 0;
int* WalkTrapProbabilities::vertices1 = 0;
int* WalkTrapProbabilities::vertices2 = 0;
int WalkTrapProbabilities::current_id = 0;


Neighbor::Neighbor() {
  next_community1 = 0;
  previous_community1 = 0;
  next_community2 = 0;       
  previous_community2 = 0;
  heap_index = -1;
}

WalkTrapProbabilities::~WalkTrapProbabilities() {
  C->memory_used -= memory();
  if(P) delete[] P;
  if(vertices) delete[] vertices;
}
WalkTrapProbabilities::WalkTrapProbabilities(int community) {
  WalktrapGraph* G = C->G;
  int nb_vertices1 = 0;
  int nb_vertices2 = 0;

  float initial_proba = 1./float(C->communities[community].size);
  int last =  C->members[C->communities[community].last_member];  
  for(int m = C->communities[community].first_member; m != last; m = C->members[m]) {
    tmp_vector1[m] = initial_proba;
    vertices1[nb_vertices1++] = m;
  }
  
  for(int t = 0; t < length; t++) {
    current_id++;    
    if(nb_vertices1 > (G->nb_vertices/2)) {
      nb_vertices2 = G->nb_vertices;
      for(int i = 0; i < G->nb_vertices; i++)
  tmp_vector2[i] = 0.;
      if(nb_vertices1 == G->nb_vertices) {
  for(int i = 0; i < G->nb_vertices; i++) {
    float proba = tmp_vector1[i]/G->vertices[i].total_weight;
      for(int j = 0; j < G->vertices[i].degree; j++)
      tmp_vector2[G->vertices[i].edges[j].neighbor] += proba*G->vertices[i].edges[j].weight;
  }
      }
      else {
  for(int i = 0; i < nb_vertices1; i++) {
    int v1 = vertices1[i];
    float proba = tmp_vector1[v1]/G->vertices[v1].total_weight;
      for(int j = 0; j < G->vertices[v1].degree; j++)
      tmp_vector2[G->vertices[v1].edges[j].neighbor] += proba*G->vertices[v1].edges[j].weight;
  }
      }
    }
    else {
      nb_vertices2 = 0;
      for(int i = 0; i < nb_vertices1; i++) {
  int v1 = vertices1[i];
        float proba = tmp_vector1[v1]/G->vertices[v1].total_weight;
        for(int j = 0; j < G->vertices[v1].degree; j++) {
    int v2 = G->vertices[v1].edges[j].neighbor;
    if(id[v2] == current_id)
        tmp_vector2[v2] += proba*G->vertices[v1].edges[j].weight;
      else {
      tmp_vector2[v2] = proba*G->vertices[v1].edges[j].weight;
      id[v2] = current_id;
      vertices2[nb_vertices2++] = v2;
    }
        }
      }
    }
    float* tmp = tmp_vector2;
    tmp_vector2 = tmp_vector1;
    tmp_vector1 = tmp;

    int* tmp2 = vertices2;
    vertices2 = vertices1;
    vertices1 = tmp2;

    nb_vertices1 = nb_vertices2;
  }

  if(nb_vertices1 > (G->nb_vertices/2)) {
    P = new float[G->nb_vertices];
    size = G->nb_vertices;
    vertices = 0;
    if(nb_vertices1 == G->nb_vertices) {
      for(int i = 0; i < G->nb_vertices; i++)
  P[i] = tmp_vector1[i]/sqrt(G->vertices[i].total_weight);
    }
    else {
      for(int i = 0; i < G->nb_vertices; i++)
  P[i] = 0.;
      for(int i = 0; i < nb_vertices1; i++)
  P[vertices1[i]] = tmp_vector1[vertices1[i]]/sqrt(G->vertices[vertices1[i]].total_weight);
    }
  }
  else {
    P = new float[nb_vertices1];
    size = nb_vertices1;
    vertices = new int[nb_vertices1];
    int j = 0;
    for(int i = 0; i < G->nb_vertices; i++) {
      if(id[i] == current_id) {
  P[j] = tmp_vector1[i]/sqrt(G->vertices[i].total_weight);
  vertices[j] = i;
  j++;
      }
    }
  }
  C->memory_used += memory();
}

WalkTrapProbabilities::WalkTrapProbabilities(int community1, int community2) {
  // The two following probability vectors must exist.
  // Do not call this function if it is not the case.
  WalkTrapProbabilities* P1 = C->communities[community1].P;
  WalkTrapProbabilities* P2 = C->communities[community2].P;
  
  float w1 = float(C->communities[community1].size)/float(C->communities[community1].size + C->communities[community2].size);
  float w2 = float(C->communities[community2].size)/float(C->communities[community1].size + C->communities[community2].size);


  if(P1->size == C->G->nb_vertices) {
    P = new float[C->G->nb_vertices];
    size = C->G->nb_vertices;
    vertices = 0;
    
    if(P2->size == C->G->nb_vertices) { // two full vectors
      for(int i = 0; i < C->G->nb_vertices; i++)
  P[i] = P1->P[i]*w1 + P2->P[i]*w2;
    }
    else {  // P1 full vector, P2 partial vector
      int j = 0;
      for(int i = 0; i < P2->size; i++) {
  for(; j < P2->vertices[i]; j++)
    P[j] = P1->P[j]*w1;
  P[j] = P1->P[j]*w1 + P2->P[i]*w2;
  j++;
      }
      for(; j < C->G->nb_vertices; j++)
  P[j] = P1->P[j]*w1;
    }
  }
  else {
    if(P2->size == C->G->nb_vertices) { // P1 partial vector, P2 full vector
      P = new float[C->G->nb_vertices];
      size = C->G->nb_vertices;
      vertices = 0;

      int j = 0;
      for(int i = 0; i < P1->size; i++) {
  for(; j < P1->vertices[i]; j++)
    P[j] = P2->P[j]*w2;
  P[j] = P1->P[i]*w1 + P2->P[j]*w2;
  j++;
      }
      for(; j < C->G->nb_vertices; j++)
  P[j] = P2->P[j]*w2;
    }
    else {  // two partial vectors
      int i = 0;
      int j = 0;
      int nb_vertices1 = 0;
      while((i < P1->size) && (j < P2->size)) {
  if(P1->vertices[i] < P2->vertices[j]) {
    tmp_vector1[P1->vertices[i]] = P1->P[i]*w1;
    vertices1[nb_vertices1++] = P1->vertices[i];
    i++;
    continue;
  }
  if(P1->vertices[i] > P2->vertices[j]) {
    tmp_vector1[P2->vertices[j]] = P2->P[j]*w2;
    vertices1[nb_vertices1++] = P2->vertices[j];
    j++;
    continue;
  }
  tmp_vector1[P1->vertices[i]] = P1->P[i]*w1 + P2->P[j]*w2;
  vertices1[nb_vertices1++] = P1->vertices[i];
  i++;
  j++;
      }
      if(i == P1->size) {
  for(; j < P2->size; j++) {
    tmp_vector1[P2->vertices[j]] = P2->P[j]*w2;
    vertices1[nb_vertices1++] = P2->vertices[j];
  }
      }
      else {
  for(; i < P1->size; i++) {
    tmp_vector1[P1->vertices[i]] = P1->P[i]*w1;
    vertices1[nb_vertices1++] = P1->vertices[i];
  }
      }

      if(nb_vertices1 > (C->G->nb_vertices/2)) {
  P = new float[C->G->nb_vertices];
  size = C->G->nb_vertices;
  vertices = 0;
  for(int i = 0; i < C->G->nb_vertices; i++)
    P[i] = 0.;
  for(int i = 0; i < nb_vertices1; i++)
    P[vertices1[i]] = tmp_vector1[vertices1[i]];
      }
      else {
  P = new float[nb_vertices1];
  size = nb_vertices1;
  vertices = new int[nb_vertices1];
  for(int i = 0; i < nb_vertices1; i++) {
    vertices[i] = vertices1[i];
    P[i] = tmp_vector1[vertices1[i]];
  }
      }
    }
  }

  C->memory_used += memory();
}

double WalkTrapProbabilities::compute_distance(const WalkTrapProbabilities* P2) const {
  double r = 0.;
  if(vertices) {
    if(P2->vertices) {  // two partial vectors
      int i = 0;
      int j = 0;
      while((i < size) && (j < P2->size)) {
  if(vertices[i] < P2->vertices[j]) {
    r += P[i]*P[i];
    i++;
    continue;
  }
  if(vertices[i] > P2->vertices[j]) {
    r += P2->P[j]*P2->P[j];
    j++;
    continue;
  }
  r += (P[i] - P2->P[j])*(P[i] - P2->P[j]);
  i++;
  j++;
      }
      if(i == size) {
  for(; j < P2->size; j++)
    r += P2->P[j]*P2->P[j];
      }
      else {
  for(; i < size; i++)
    r += P[i]*P[i];
      }
    }
    else {  // P1 partial vector, P2 full vector 

      int i = 0;
      for(int j = 0; j < size; j++) {
  for(; i < vertices[j]; i++)
    r += P2->P[i]*P2->P[i];
  r += (P[j] - P2->P[i])*(P[j] - P2->P[i]);
  i++;
      }
      for(; i < P2->size; i++)
  r += P2->P[i]*P2->P[i];      
    }
  }
  else {
    if(P2->vertices) {  // P1 full vector, P2 partial vector
      int i = 0;
      for(int j = 0; j < P2->size; j++) {
  for(; i < P2->vertices[j]; i++)
    r += P[i]*P[i];
  r += (P[i] - P2->P[j])*(P[i] - P2->P[j]);
  i++;
      }
      for(; i < size; i++)
  r += P[i]*P[i];
    }
    else {  // two full vectors
      for(int i = 0; i < size; i++)
  r += (P[i] - P2->P[i])*(P[i] - P2->P[i]);
    }
  }
  return r;
}

long WalkTrapProbabilities::memory() {
  if(vertices)
    return (sizeof(WalkTrapProbabilities) + long(size)*(sizeof(float) + sizeof(int)));
  else
    return (sizeof(WalkTrapProbabilities) + long(size)*sizeof(float));
}

WalkTrapCommunity::WalkTrapCommunity() {
  P = 0;
  first_neighbor = 0;
  last_neighbor = 0;
  sub_community_of = -1;
  sub_communities[0] = -1;
  sub_communities[1] = -1;
  sigma = 0.;
  internal_weight = 0.;
  total_weight = 0.;
}

WalkTrapCommunity::~WalkTrapCommunity() {
  if(P) delete P;
}


WalkTrapCommunities::WalkTrapCommunities(WalktrapGraph* graph, int random_walks_length, bool s, int d, long m) {
  silent = s;
  details = d;
  max_memory = m;
  memory_used = 0;
  G = graph;
  
  WalkTrapProbabilities::C = this;
  WalkTrapProbabilities::length = random_walks_length;
  WalkTrapProbabilities::tmp_vector1 = new float[G->nb_vertices];
  WalkTrapProbabilities::tmp_vector2 = new float[G->nb_vertices];
  WalkTrapProbabilities::id = new int[G->nb_vertices];
  for(int i = 0; i < G->nb_vertices; i++) WalkTrapProbabilities::id[i] = 0;
  WalkTrapProbabilities::vertices1 = new int[G->nb_vertices];
  WalkTrapProbabilities::vertices2 = new int[G->nb_vertices];
  WalkTrapProbabilities::current_id = 0;

  
  members = new int[G->nb_vertices];  
  for(int i = 0; i < G->nb_vertices; i++)
    members[i] = -1;

  H = new Neighbor_heap(G->nb_edges);
  communities = new WalkTrapCommunity[2*G->nb_vertices];

// init the n single vertex communities

  if(max_memory != -1)
    min_delta_sigma = new Min_delta_sigma_heap(G->nb_vertices*2);
  else min_delta_sigma = 0;
  
  for(int i = 0; i < G->nb_vertices; i++) {
    communities[i].this_community = i;
    communities[i].first_member = i;
    communities[i].last_member = i;
    communities[i].size = 1;
    communities[i].sub_community_of = 0;
  }

  nb_communities = G->nb_vertices;
  nb_active_communities = G->nb_vertices;

  if(!silent) cerr << "computing random walks and the first distances:";
  for(int i = 0; i < G->nb_vertices; i++)
    for(int j = 0; j < G->vertices[i].degree; j++)
      if (i < G->vertices[i].edges[j].neighbor) {
  communities[i].total_weight += G->vertices[i].edges[j].weight/2.;
  communities[G->vertices[i].edges[j].neighbor].total_weight += G->vertices[i].edges[j].weight/2.;
  Neighbor* N = new Neighbor;
  N->community1 = i;
  N->community2 = G->vertices[i].edges[j].neighbor;
  N->delta_sigma = -1./double(min(G->vertices[i].degree,  G->vertices[G->vertices[i].edges[j].neighbor].degree));
  N->weight = G->vertices[i].edges[j].weight;
  N->exact = false;
  add_neighbor(N);
      }

  if(max_memory != -1) {
    memory_used += min_delta_sigma->memory();
    memory_used += 2*long(G->nb_vertices)*sizeof(WalkTrapCommunity);
    memory_used += long(G->nb_vertices)*(2*sizeof(float) + 3*sizeof(int)); // the static data of WalkTrapProbabilities class
    memory_used += H->memory() + long(G->nb_edges)*sizeof(Neighbor);
    memory_used += G->memory();    
  }

  int c = 0;
  Neighbor* N = H->get_first();  
  while(!N->exact) {
    update_neighbor(N, compute_delta_sigma(N->community1, N->community2));
    N->exact = true;
    N = H->get_first();
    if(max_memory != -1) manage_memory();
    if(!silent) {
      c++;
      for(int k = (500*(c-1))/G->nb_edges + 1; k <= (500*c)/G->nb_edges; k++) {
  if(k % 50 == 1) {cerr.width(2); cerr << endl << k/ 5 << "% ";}
  cerr << ".";
      }
    }
  }
  
  if(!silent) cerr << endl << endl;

  if (details >= 2) cout << "Partition 0" << " (" << G->nb_vertices << " communities)" << endl;
  if (details >= 2) for(int i = 0; i < G->nb_vertices; i++) print_community(i);
  if (details >= 2) cout << endl;
}

WalkTrapCommunities::~WalkTrapCommunities() {
  delete[] members;
  delete[] communities;
  delete H;
  if(min_delta_sigma) delete min_delta_sigma;
  
  delete[] WalkTrapProbabilities::tmp_vector1;
  delete[] WalkTrapProbabilities::tmp_vector2;
  delete[] WalkTrapProbabilities::id;
  delete[] WalkTrapProbabilities::vertices1;
  delete[] WalkTrapProbabilities::vertices2;
}

float WalkTrapCommunity::min_delta_sigma() {
  float r = 1.;
  for(Neighbor* N = first_neighbor; N != 0;) {
    if(N->delta_sigma < r) r = N->delta_sigma;
    if(N->community1 == this_community)
      N = N->next_community1;
    else
      N = N->next_community2;
  }
  return r;
}


void WalkTrapCommunity::add_neighbor(Neighbor* N) { // add a new neighbor at the end of the list
  if (last_neighbor) {
    if(last_neighbor->community1 == this_community)
      last_neighbor->next_community1 = N;
    else
      last_neighbor->next_community2 = N;
    
    if(N->community1 == this_community)
      N->previous_community1 = last_neighbor;
    else
      N->previous_community2 = last_neighbor;
  }
  else {
    first_neighbor = N;
    if(N->community1 == this_community)
      N->previous_community1 = 0;
    else
      N->previous_community2 = 0;
  }
  last_neighbor = N;
}

void WalkTrapCommunity::remove_neighbor(Neighbor* N) {  // remove a neighbor from the list
  if (N->community1 == this_community) {
    if(N->next_community1) {
//      if (N->next_community1->community1 == this_community)
  N->next_community1->previous_community1 = N->previous_community1;
//      else 
//  N->next_community1->previous_community2 = N->previous_community1;
    }
    else last_neighbor = N->previous_community1;
    if(N->previous_community1) {
      if (N->previous_community1->community1 == this_community)
  N->previous_community1->next_community1 = N->next_community1;
      else 
  N->previous_community1->next_community2 = N->next_community1;
    }
    else first_neighbor = N->next_community1;
  }
  else {
    if(N->next_community2) {
      if (N->next_community2->community1 == this_community)
  N->next_community2->previous_community1 = N->previous_community2;
      else 
  N->next_community2->previous_community2 = N->previous_community2;
    }
    else last_neighbor = N->previous_community2;
    if(N->previous_community2) {
//      if (N->previous_community2->community1 == this_community)
//  N->previous_community2->next_community1 = N->next_community2;
//      else 
  N->previous_community2->next_community2 = N->next_community2;
    }
    else first_neighbor = N->next_community2;
  }
}

void WalkTrapCommunities::remove_neighbor(Neighbor* N) {
  communities[N->community1].remove_neighbor(N);
  communities[N->community2].remove_neighbor(N);
  H->remove(N);

  if(max_memory !=-1) {
    if(N->delta_sigma == min_delta_sigma->delta_sigma[N->community1]) {
      min_delta_sigma->delta_sigma[N->community1] = communities[N->community1].min_delta_sigma();
      if(communities[N->community1].P) min_delta_sigma->update(N->community1);
    }

    if(N->delta_sigma == min_delta_sigma->delta_sigma[N->community2]) {
      min_delta_sigma->delta_sigma[N->community2] = communities[N->community2].min_delta_sigma();
      if(communities[N->community2].P) min_delta_sigma->update(N->community2);
    }
  }
}

void WalkTrapCommunities::add_neighbor(Neighbor* N) {
  communities[N->community1].add_neighbor(N);
  communities[N->community2].add_neighbor(N);
  H->add(N);

  if(max_memory !=-1) {
    if(N->delta_sigma < min_delta_sigma->delta_sigma[N->community1]) {
      min_delta_sigma->delta_sigma[N->community1] = N->delta_sigma;
      if(communities[N->community1].P) min_delta_sigma->update(N->community1);
    }

    if(N->delta_sigma < min_delta_sigma->delta_sigma[N->community2]) {
      min_delta_sigma->delta_sigma[N->community2] = N->delta_sigma;
      if(communities[N->community2].P) min_delta_sigma->update(N->community2);
    }
  }
}

void WalkTrapCommunities::update_neighbor(Neighbor* N, float new_delta_sigma) {
  if(max_memory !=-1) {
    if(new_delta_sigma < min_delta_sigma->delta_sigma[N->community1]) {
      min_delta_sigma->delta_sigma[N->community1] = new_delta_sigma;
      if(communities[N->community1].P) min_delta_sigma->update(N->community1);
    }
   
    if(new_delta_sigma < min_delta_sigma->delta_sigma[N->community2]) {
      min_delta_sigma->delta_sigma[N->community2] = new_delta_sigma;
      if(communities[N->community2].P) min_delta_sigma->update(N->community2);
    }

    float old_delta_sigma = N->delta_sigma;
    N->delta_sigma = new_delta_sigma;
    H->update(N);

    if(old_delta_sigma == min_delta_sigma->delta_sigma[N->community1]) {
      min_delta_sigma->delta_sigma[N->community1] = communities[N->community1].min_delta_sigma();
      if(communities[N->community1].P) min_delta_sigma->update(N->community1);
    }

    if(old_delta_sigma == min_delta_sigma->delta_sigma[N->community2]) {
      min_delta_sigma->delta_sigma[N->community2] = communities[N->community2].min_delta_sigma();
      if(communities[N->community2].P) min_delta_sigma->update(N->community2);
    }
  }
  else {
    N->delta_sigma = new_delta_sigma;
    H->update(N);
  }
}

void WalkTrapCommunities::manage_memory() {
  while((memory_used > max_memory) && !min_delta_sigma->is_empty()) {
    int c = min_delta_sigma->get_max_community();
    delete communities[c].P;
    communities[c].P = 0;
    min_delta_sigma->remove_community(c);
  }  
}



void WalkTrapCommunities::merge_communities(Neighbor* merge_N) {
  int c1 = merge_N->community1;
  int c2 = merge_N->community2;
  
  communities[nb_communities].first_member = communities[c1].first_member;  // merge the 
  communities[nb_communities].last_member = communities[c2].last_member;  // two lists   
  members[communities[c1].last_member] = communities[c2].first_member;    // of members

  communities[nb_communities].size = communities[c1].size + communities[c2].size;
  communities[nb_communities].this_community = nb_communities;
  communities[nb_communities].sub_community_of = 0;
  communities[nb_communities].sub_communities[0] = c1;
  communities[nb_communities].sub_communities[1] = c2;
  communities[nb_communities].total_weight = communities[c1].total_weight + communities[c2].total_weight;
  communities[nb_communities].internal_weight = communities[c1].internal_weight + communities[c2].internal_weight + merge_N->weight;
  communities[nb_communities].sigma = communities[c1].sigma + communities[c2].sigma + merge_N->delta_sigma;
  
  communities[c1].sub_community_of = nb_communities;
  communities[c2].sub_community_of = nb_communities;

// update the new probability vector...
  
  if(communities[c1].P && communities[c2].P) communities[nb_communities].P = new WalkTrapProbabilities(c1, c2);

  if(communities[c1].P) {
    delete communities[c1].P; 
    communities[c1].P = 0;
    if(max_memory != -1) min_delta_sigma->remove_community(c1);
  }
  if(communities[c2].P) {
    delete communities[c2].P;
    communities[c2].P = 0;
    if(max_memory != -1) min_delta_sigma->remove_community(c2);
  }

  if(max_memory != -1) {
    min_delta_sigma->delta_sigma[c1] = -1.;       // to avoid to update the min_delta_sigma for these communities
    min_delta_sigma->delta_sigma[c2] = -1.;       // 
    min_delta_sigma->delta_sigma[nb_communities] = -1.;
  }
  
// update the new neighbors
// by enumerating all the neighbors of c1 and c2

  Neighbor* N1 = communities[c1].first_neighbor;
  Neighbor* N2 = communities[c2].first_neighbor;

  while(N1 && N2) { 
    int neighbor_community1;
    int neighbor_community2;
    
    if (N1->community1 == c1) neighbor_community1 = N1->community2;
    else neighbor_community1 = N1->community1;
    if (N2->community1 == c2) neighbor_community2 = N2->community2;
    else neighbor_community2 = N2->community1;

    if (neighbor_community1 < neighbor_community2) {
      Neighbor* tmp = N1;
      if (N1->community1 == c1) N1 = N1->next_community1;
      else N1 = N1->next_community2;
      remove_neighbor(tmp);
      Neighbor* N = new Neighbor;
      N->weight = tmp->weight;
      N->community1 = neighbor_community1;
      N->community2 = nb_communities;
      N->delta_sigma = (double(communities[c1].size+communities[neighbor_community1].size)*tmp->delta_sigma + double(communities[c2].size)*merge_N->delta_sigma)/(double(communities[c1].size+communities[c2].size+communities[neighbor_community1].size));//compute_delta_sigma(neighbor_community1, nb_communities);
      N->exact = false;
      delete tmp;
      add_neighbor(N); 
    }
    
    if (neighbor_community2 < neighbor_community1) {
      Neighbor* tmp = N2;
      if (N2->community1 == c2) N2 = N2->next_community1;
      else N2 = N2->next_community2;
      remove_neighbor(tmp);
      Neighbor* N = new Neighbor;
      N->weight = tmp->weight;
      N->community1 = neighbor_community2;
      N->community2 = nb_communities;
      N->delta_sigma = (double(communities[c1].size)*merge_N->delta_sigma + double(communities[c2].size+communities[neighbor_community2].size)*tmp->delta_sigma)/(double(communities[c1].size+communities[c2].size+communities[neighbor_community2].size));//compute_delta_sigma(neighbor_community2, nb_communities);
      N->exact = false;
      delete tmp;
      add_neighbor(N); 
    }
    
    if (neighbor_community1 == neighbor_community2) {
      Neighbor* tmp1 = N1;
      Neighbor* tmp2 = N2;
      bool exact = N1->exact && N2->exact;
      if (N1->community1 == c1) N1 = N1->next_community1;
      else N1 = N1->next_community2;
      if (N2->community1 == c2) N2 = N2->next_community1;
      else N2 = N2->next_community2;
      remove_neighbor(tmp1);
      remove_neighbor(tmp2);
      Neighbor* N = new Neighbor;
      N->weight = tmp1->weight + tmp2->weight;
      N->community1 = neighbor_community1;
      N->community2 = nb_communities;
      N->delta_sigma = (double(communities[c1].size+communities[neighbor_community1].size)*tmp1->delta_sigma + double(communities[c2].size+communities[neighbor_community1].size)*tmp2->delta_sigma - double(communities[neighbor_community1].size)*merge_N->delta_sigma)/(double(communities[c1].size+communities[c2].size+communities[neighbor_community1].size));
      N->exact = exact;
      delete tmp1;
      delete tmp2;
      add_neighbor(N);
    }
  }

  
  if(!N1) {
    while(N2) {
//      double delta_sigma2 = N2->delta_sigma;
      int neighbor_community;
      if (N2->community1 == c2) neighbor_community = N2->community2;
      else neighbor_community = N2->community1;
      Neighbor* tmp = N2;
      if (N2->community1 == c2) N2 = N2->next_community1;
      else N2 = N2->next_community2;
      remove_neighbor(tmp);
      Neighbor* N = new Neighbor;
      N->weight = tmp->weight;
      N->community1 = neighbor_community;
      N->community2 = nb_communities;
      N->delta_sigma = (double(communities[c1].size)*merge_N->delta_sigma + double(communities[c2].size+communities[neighbor_community].size)*tmp->delta_sigma)/(double(communities[c1].size+communities[c2].size+communities[neighbor_community].size));//compute_delta_sigma(neighbor_community, nb_communities);
      N->exact = false;
      delete tmp;
      add_neighbor(N);
    }
  }
  if(!N2) {
    while(N1) {
//      double delta_sigma1 = N1->delta_sigma;
      int neighbor_community;
      if (N1->community1 == c1) neighbor_community = N1->community2;
      else neighbor_community = N1->community1;
      Neighbor* tmp = N1;
      if (N1->community1 == c1) N1 = N1->next_community1;
      else N1 = N1->next_community2;
      remove_neighbor(tmp);
      Neighbor* N = new Neighbor;
      N->weight = tmp->weight;
      N->community1 = neighbor_community;
      N->community2 = nb_communities;
      N->delta_sigma = (double(communities[c1].size+communities[neighbor_community].size)*tmp->delta_sigma + double(communities[c2].size)*merge_N->delta_sigma)/(double(communities[c1].size+communities[c2].size+communities[neighbor_community].size));//compute_delta_sigma(neighbor_community, nb_communities);
      N->exact = false;
      delete tmp;
      add_neighbor(N);
    }
  }

  if(max_memory != -1) {
    min_delta_sigma->delta_sigma[nb_communities] = communities[nb_communities].min_delta_sigma();
    min_delta_sigma->update(nb_communities);
  } 

  nb_communities++;
  nb_active_communities--;
}

double WalkTrapCommunities::merge_nearest_communities() {
  Neighbor* N = H->get_first();  
  while(!N->exact) {
    update_neighbor(N, compute_delta_sigma(N->community1, N->community2));
    N->exact = true;
    N = H->get_first();
    if(max_memory != -1) manage_memory();
  }

  double d = N->delta_sigma;
  remove_neighbor(N);

  merge_communities(N);
  if(max_memory != -1) manage_memory();

  if(details >= 2) cout << "Partition " << nb_communities - G->nb_vertices << " (" << 2*G->nb_vertices - nb_communities << " communities)" << endl;
  if(details >= 2) cout << "community " << N->community1 << " + community " << N->community2 << " --> community " << nb_communities - 1 << endl;

  if(details >= 3) {
  float Q = 0.;
  for(int i = 0; i < nb_communities; i++)
    if(communities[i].sub_community_of == 0) 
      Q += (communities[i].internal_weight - communities[i].total_weight*communities[i].total_weight/G->total_weight)/G->total_weight;
  cout << "Q = " << Q << "  #  " << "delta_sigma = " << d << endl;  
  }

  if(details == 4) print_community(nb_communities-1);
  if(details >= 5) print_partition(2*G->nb_vertices - nb_communities); 
  if(details >= 2) cout << endl;
  delete N;

  if(!silent) {
    for(int k = (500*(G->nb_vertices - nb_active_communities - 1))/(G->nb_vertices-1) + 1; k <= (500*(G->nb_vertices - nb_active_communities))/(G->nb_vertices-1); k++) {
      if(k % 50 == 1) {cerr.width(2); cerr << endl << k/ 5 << "% ";}
      cerr << ".";
    }
  }
  return d;
}

CommunitiesSharedPtr  WalkTrapCommunities::toMlnetCommunities(const LayerSharedPtr& layer)
{
  CommunitiesSharedPtr com = communities::create();
  //For each resulted community from the Walktrap Algorithm 
  int actNum =0;
  for (int i =0; i< nb_communities;i++)
  {
    //For each 
    CommunitySharedPtr c = community::create();
    
    for(int m = communities[i].first_member; m != members[communities[i].last_member]; m = members[m]) {
    if(G->index) 
    {
      ActorSharedPtr a(new actor(actNum++,G->index[m]));
      NodeSharedPtr  n(new node(1,a,layer));  
      c->add_node(n);
    }
    else 
    {
     std::string asStr = std::to_string(m);
     ActorSharedPtr a(new actor(actNum++,asStr));
     NodeSharedPtr  n(new node(1,a,layer));
     c->add_node(n);
    }

    //if(members[m] != members[communities[c].last_member]) cout << ", ";
    }
    com->add_community(c);
  }
  return com;
}

double WalkTrapCommunities::compute_delta_sigma(int community1, int community2) {
  if(!communities[community1].P) {
    communities[community1].P = new WalkTrapProbabilities(community1);
    if(max_memory != -1) min_delta_sigma->update(community1);
  }
  if(!communities[community2].P) {
    communities[community2].P = new WalkTrapProbabilities(community2);
    if(max_memory != -1) min_delta_sigma->update(community2);
  }
  
  return communities[community1].P->compute_distance(communities[community2].P)*double(communities[community1].size)*double(communities[community2].size)/double(communities[community1].size + communities[community2].size);
}


void WalkTrapCommunities::print_community(int c) {
  cout << "community " << c << " = {";
  for(int m = communities[c].first_member; m != members[communities[c].last_member]; m = members[m]) {
    if(G->index) cout << G->index[m];
    else cout << m;
    if(members[m] != members[communities[c].last_member]) cout << ", ";
  }
  cout << "}" << endl;
}

void WalkTrapCommunities::print_state() {
  cout << "number of communities : " << nb_active_communities << endl;
  for(int c = 0; c < nb_communities; c++)
    if(communities[c].sub_community_of == 0) 
      print_community(c);
}

void WalkTrapCommunities::print_partition(int nb_remaining_commities) {
  int last_community = 2*G->nb_vertices - nb_remaining_commities - 1;
  cout << "Partition " << G->nb_vertices - nb_remaining_commities << " (" << nb_remaining_commities << " communities)" << endl;
  for(int c = 0; c <= last_community; c++)
    if((communities[c].sub_community_of == 0) || (communities[c].sub_community_of > last_community))
      print_community(c);
}

float WalkTrapCommunities::find_best_modularity(int community, bool* max_modularity) {
  float Q = (communities[community].internal_weight - communities[community].total_weight*communities[community].total_weight/G->total_weight)/G->total_weight;
  if(communities[community].sub_communities[0] == -1) {
    max_modularity[community] = true;
    return Q;
  }
  float Q2 = find_best_modularity(communities[community].sub_communities[0], max_modularity) + find_best_modularity(communities[community].sub_communities[1], max_modularity);
  if(Q2 > Q) {
    max_modularity[community] = false;
    return Q2;
  }
  else {
    max_modularity[community] = true;
    return Q;
  }
}

void WalkTrapCommunities::print_best_modularity_partition() {
  bool* max_modularity = new bool[nb_communities];
  float Q = find_best_modularity(nb_communities-1, max_modularity);
  cout << "Maximal modularity Q = " << Q << " for partition :" << endl;
  print_best_modularity_partition(nb_communities-1, max_modularity);
  delete[] max_modularity;
}

void WalkTrapCommunities::print_best_modularity_partition(int community, bool* max_modularity) {
  if(max_modularity[community]) 
    print_community(community);
  else {
    print_best_modularity_partition(communities[community].sub_communities[0], max_modularity);
    print_best_modularity_partition(communities[community].sub_communities[1], max_modularity);
  }
}



bool operator<(const WalktrapEdge& E1, const WalktrapEdge& E2) {
  return(E1.neighbor < E2.neighbor);
}


WalktrapVertex::WalktrapVertex() {
  degree = 0;
  edges = 0;
  total_weight = 0.;
}

WalktrapVertex::~WalktrapVertex() {
  if(edges) delete[] edges;
}

WalktrapGraph::WalktrapGraph() {
  nb_vertices = 0;
  nb_edges = 0;
  vertices = 0;
  index = 0;
  total_weight = 0.;
}

WalktrapGraph::~WalktrapGraph () {
  if (vertices) delete[] vertices;
}

class Edge_list {  
public:
  int* V1;
  int* V2;
  float* W;

  int size;
  int size_max;
  
  void add(int v1, int v2, float w);
  Edge_list() {
    size = 0;
    size_max = 1024;
    V1 = new int[1024];
    V2 = new int[1024];
    W = new float[1024];
  }
  ~Edge_list() {
    if(V1) delete[] V1;
    if(V2) delete[] V2;
    if(W) delete[] W;
  }
};

void Edge_list::add(int v1, int v2, float w) {
  if(size == size_max) {
    int* tmp1 = new int[2*size_max];
    int* tmp2 = new int[2*size_max];
    float* tmp3 = new float[2*size_max];
    for(int i = 0; i < size_max; i++) {
      tmp1[i] = V1[i];
      tmp2[i] = V2[i];      
      tmp3[i] = W[i];
    }
    delete[] V1;
    delete[] V2;
    delete[] W;
    V1 = tmp1;
    V2 = tmp2;
    W = tmp3;
    size_max *= 2;
  }
  V1[size] = v1;
  V2[size] = v2;
  W[size] = w;
  size++;
}


istream& operator>>(istream& stream, WalktrapGraph& G) {
  if(G.vertices) delete[] G.vertices;

  int nb_line = 0;
  int max_vertex = 0;
  
  Edge_list EL;

  while (!stream.eof()) { // loop for each line of the file
    nb_line++;
    string str;
    getline(stream, str);
    if(str[0] == '#') continue; // a comment line
    istringstream line(str);

    int v1;
    line >> v1;
    if(line.fail()) {
      if(line.eof()) continue;
      cerr << "error : unable to read line " << nb_line << " : " << str << endl;
      exit(0);
    }
    int v2;
    line >> v2;
    if(line.fail()) {
      cerr << "error : unable to read line " << nb_line << " : " << str << endl;
      exit(0);
    }
    float w;
    line >> w;
    if(line.fail()) {
      if(line.eof()) w = 1.;
      else {
  cerr << "error : unable to read line " << nb_line << " : " << str << endl;
  exit(0);
      }
    }
    if(!line.eof()) {
      cerr << "error : line " << nb_line << " too long : " << str << endl;
      exit(0);
    }

    if(v1 > max_vertex) max_vertex = v1;
    if(v2 > max_vertex) max_vertex = v2;
    if((v1 < 0) || (v2 < 0)) {
      cerr << "error : line " << nb_line << " negative vertex number : " << str << endl;
      exit(0);
    }
    if(w < 0) {
      cerr << "error : line " << nb_line << " negative weight : " << str << endl;
      exit(0);
    }
    EL.add(v1, v2, w);
  }

  G.nb_vertices = max_vertex + 1; 
  G.vertices = new WalktrapVertex[G.nb_vertices];
  G.nb_edges = 0;
  G.total_weight = 0.;

  for(int i = 0; i < EL.size; i++) {
      G.vertices[EL.V1[i]].degree++;
      G.vertices[EL.V2[i]].degree++;
      G.vertices[EL.V1[i]].total_weight += EL.W[i];
      G.vertices[EL.V2[i]].total_weight += EL.W[i];
      G.nb_edges++;
      G.total_weight += EL.W[i];
    }

  for(int i = 0; i < G.nb_vertices; i++) {
    if(G.vertices[i].degree == 0) {
      cerr << "error : degree of vertex " << i << " is 0" << endl;
      exit(0);
    }
    G.vertices[i].edges = new WalktrapEdge[G.vertices[i].degree + 1];
    G.vertices[i].edges[0].neighbor = i;
    G.vertices[i].edges[0].weight = G.vertices[i].total_weight/double(G.vertices[i].degree);
    G.vertices[i].total_weight+= G.vertices[i].total_weight/double(G.vertices[i].degree);
    G.vertices[i].degree = 1;
  }
 
  for(int i = 0; i < EL.size; i++) {
    G.vertices[EL.V1[i]].edges[G.vertices[EL.V1[i]].degree].neighbor = EL.V2[i];
    G.vertices[EL.V1[i]].edges[G.vertices[EL.V1[i]].degree].weight = EL.W[i];
    G.vertices[EL.V1[i]].degree++;
    G.vertices[EL.V2[i]].edges[G.vertices[EL.V2[i]].degree].neighbor = EL.V1[i];
    G.vertices[EL.V2[i]].edges[G.vertices[EL.V2[i]].degree].weight = EL.W[i];
    G.vertices[EL.V2[i]].degree++;
  }  
  
  for(int i = 0; i < G.nb_vertices; i++)
    sort(G.vertices[i].edges, G.vertices[i].edges+G.vertices[i].degree);

  for(int i = 0; i < G.nb_vertices; i++) {  // merge multi edges
    int a = 0;
    for(int b = 1; b < G.vertices[i].degree; b++) {
      if(G.vertices[i].edges[b].neighbor == G.vertices[i].edges[a].neighbor)
  G.vertices[i].edges[a].weight += G.vertices[i].edges[b].weight;
      else 
  G.vertices[i].edges[++a] = G.vertices[i].edges[b];
    }
    G.vertices[i].degree = a+1;
  }

  return stream;
}

void fromLayerToGraph (const MLNetworkSharedPtr& mlnet, const LayerSharedPtr& layer, WalktrapGraph& G)
    {

        //Add Edges to this graph
        Edge_list EL;
        size_t num_edges=0;
        for (EdgeSharedPtr edge : *mlnet->get_edges()) {
          num_edges++;
          EL.add(edge->v1->id , edge->v2->id ,1) ;//Add an edge with weight equal to 1
        }
       
        //Add vertices to this graph
     G.nb_vertices = mlnet->get_nodes(layer)->size();
     G.vertices = new WalktrapVertex[G.nb_vertices];
     G.nb_edges = 0;
     G.total_weight = 0.;

     for(int i = 0; i < EL.size; i++) {
          G.vertices[EL.V1[i]].degree++;
          G.vertices[EL.V2[i]].degree++;
          G.vertices[EL.V1[i]].total_weight += EL.W[i];
          G.vertices[EL.V2[i]].total_weight += EL.W[i];
          G.nb_edges++;
          G.total_weight += EL.W[i];
     }
     
     for(int i = 0; i < G.nb_vertices; i++) {
        if(G.vertices[i].degree == 0) {
          cerr << "error : degree of vertex " << i << " is 0" << endl;
          exit(0);
        }

      G.vertices[i].edges = new WalktrapEdge[G.vertices[i].degree + 1];
      G.vertices[i].edges[0].neighbor = i;
      G.vertices[i].edges[0].weight = G.vertices[i].total_weight/double(G.vertices[i].degree);
      G.vertices[i].total_weight+= G.vertices[i].total_weight/double(G.vertices[i].degree);
      G.vertices[i].degree = 1;
    }
 
  for(int i = 0; i < EL.size; i++) {
    G.vertices[EL.V1[i]].edges[G.vertices[EL.V1[i]].degree].neighbor = EL.V2[i];
    G.vertices[EL.V1[i]].edges[G.vertices[EL.V1[i]].degree].weight = EL.W[i];
    G.vertices[EL.V1[i]].degree++;
    G.vertices[EL.V2[i]].edges[G.vertices[EL.V2[i]].degree].neighbor = EL.V1[i];
    G.vertices[EL.V2[i]].edges[G.vertices[EL.V2[i]].degree].weight = EL.W[i];
    G.vertices[EL.V2[i]].degree++;
  }  
  
  for(int i = 0; i < G.nb_vertices; i++)
    sort(G.vertices[i].edges, G.vertices[i].edges+G.vertices[i].degree);

  for(int i = 0; i < G.nb_vertices; i++) {  // merge multi edges
    int a = 0;
    for(int b = 1; b < G.vertices[i].degree; b++) {
      if(G.vertices[i].edges[b].neighbor == G.vertices[i].edges[a].neighbor)
  G.vertices[i].edges[a].weight += G.vertices[i].edges[b].weight;
      else 
  G.vertices[i].edges[++a] = G.vertices[i].edges[b];
    }
    G.vertices[i].degree = a+1;
  }

}



long WalktrapGraph::memory() {
  long m = 0;
  m += long(nb_vertices)*sizeof(WalktrapVertex);
  m += 2*long(nb_edges)*sizeof(WalktrapEdge);
  m += sizeof(WalktrapGraph);
  if(index != 0) {
    m += long(nb_vertices)*sizeof(char*);
    for(int i = 0; i < nb_vertices; i++)
      m += strlen(index[i]) + 1;
  }
  return m;
}


bool WalktrapGraph::load_index(char* input_file) {
  ifstream f;
  f.open(input_file, ios::in);
  if(!f.is_open()) {
    cerr << "unable to open file " << index << " , index is ignored" << endl; 
    return false;
  }
  index = new char*[nb_vertices];
  for(int i = 0; i < nb_vertices; ++i)
    index[i] = 0;
   
  int nb_line = 0;
  while (!f.eof()) {  // loop for each line of the file
    nb_line++;
    int i;
    f >> i;
    if(f.fail()) {
      if(f.eof()) break;
      cerr << "error : unable to read line " << nb_line << " , index is ignored" << endl;
      for(int i = 0; i < nb_vertices; ++i)
  if(index[i]) delete[] index[i];
      delete[] index;
      index = 0;
      return false;
    }
    if(i < 0 || i >= nb_vertices || index[i] != 0) {
      cerr << "error : invalid vertex number at line : " << nb_line << " , index is ignored" << endl;
      for(int i = 0; i < nb_vertices; ++i)
  if(index[i]) delete[] index[i];
      delete[] index;
      index = 0;
      return false;
    }
    string str;
    getline(f, str);
    if(str.length() <= 1) {
      cerr << "error : unable to read line " << nb_line << " , index is ignored" << endl;
      for(int i = 0; i < nb_vertices; ++i)
  if(index[i]) delete[] index[i];
      delete[] index;
      index = 0;
      return false;
    }
    char* c = new char[str.size()];
    strcpy(c, str.c_str()+1);
    index[i] = c;
  }
  
  for(int i = 0; i < nb_vertices; ++i)
    if(index[i] == 0) {
      cerr << "error : vertex " << i << "not found, index is ignored" << endl;
      for(int i = 0; i < nb_vertices; ++i)
  if(index[i]) delete[] index[i];
      delete[] index;
      index = 0;
      return false;
    }
  return true;
}


void Neighbor_heap::move_up(int index) {
  while(H[index/2]->delta_sigma > H[index]->delta_sigma) {
    Neighbor* tmp = H[index/2];
    H[index]->heap_index = index/2;
    H[index/2] = H[index];
    tmp->heap_index = index;
    H[index] = tmp;
    index = index/2;
  }
}

void Neighbor_heap::move_down(int index) {
  while(true) {
    int min = index;
    if((2*index < size) && (H[2*index]->delta_sigma < H[min]->delta_sigma))
      min = 2*index;
    if(2*index+1 < size && H[2*index+1]->delta_sigma < H[min]->delta_sigma)
      min = 2*index+1;
    if(min != index) {
      Neighbor* tmp = H[min];
      H[index]->heap_index = min;
      H[min] = H[index];
      tmp->heap_index = index;
      H[index] = tmp;
      index = min;
    }
    else break;
  }
}

Neighbor* Neighbor_heap::get_first() {
  if(size == 0) return 0;
  else return H[0];
}

void Neighbor_heap::remove(Neighbor* N) {
  if(N->heap_index == -1 || size == 0) return;
  Neighbor* last_N = H[--size];
  H[N->heap_index] = last_N;
  last_N->heap_index = N->heap_index;
  move_up(last_N->heap_index);
  move_down(last_N->heap_index);  
  N->heap_index = -1;
}

void Neighbor_heap::add(Neighbor* N) {
  if(size >= max_size) return;
    N->heap_index = size++;
    H[N->heap_index] = N;
  move_up(N->heap_index);
}

void Neighbor_heap::update(Neighbor* N) {
  if(N->heap_index == -1) return;
  move_up(N->heap_index);
  move_down(N->heap_index);
}

long Neighbor_heap::memory() {
  return (sizeof(Neighbor_heap) + long(max_size)*sizeof(Neighbor*));
}
  
Neighbor_heap::Neighbor_heap(int max_s) {
  max_size = max_s;
  size = 0;
  H = new Neighbor*[max_s];
}

Neighbor_heap::~Neighbor_heap() {
  delete[] H;
}

bool Neighbor_heap::is_empty() {
  return (size == 0);
}



//#################################################################

void Min_delta_sigma_heap::move_up(int index) {
  while(delta_sigma[H[index/2]] < delta_sigma[H[index]]) {
    int tmp = H[index/2];
    I[H[index]] = index/2;
    H[index/2] = H[index];
    I[tmp] = index;
    H[index] = tmp;
    index = index/2;
  }
}

void Min_delta_sigma_heap::move_down(int index) {
  while(true) {
    int max = index;
    if(2*index < size && delta_sigma[H[2*index]] > delta_sigma[H[max]])
      max = 2*index;
    if(2*index+1 < size && delta_sigma[H[2*index+1]] > delta_sigma[H[max]])
      max = 2*index+1;
    if(max != index) {
      int tmp = H[max];
      I[H[index]] = max;
      H[max] = H[index];
      I[tmp] = index;
      H[index] = tmp;
      index = max;
    }
    else break;
  }
}

int Min_delta_sigma_heap::get_max_community() {
  if(size == 0) return -1;
  else return H[0];
}

void Min_delta_sigma_heap::remove_community(int community) {
  if(I[community] == -1 || size == 0) return;
  int last_community = H[--size];
  H[I[community]] = last_community;
  I[last_community] = I[community];
  move_up(I[last_community]);
  move_down(I[last_community]);  
  I[community] = -1;
}

void Min_delta_sigma_heap::update(int community) {
  if(community < 0 || community >= max_size) return;
  if(I[community] == -1) {
    I[community] = size++;
    H[I[community]] = community;
  }
  move_up(I[community]);
  move_down(I[community]);
}

long Min_delta_sigma_heap::memory() {
  return (sizeof(Min_delta_sigma_heap) + long(max_size)*(2*sizeof(int) + sizeof(float)));
}
  
Min_delta_sigma_heap::Min_delta_sigma_heap(int max_s) {
  max_size = max_s;
  size = 0;
  H = new int[max_s];
  I = new int[max_s];
  delta_sigma = new float[max_s];
  for(int i = 0; i < max_size; i++) {
    I[i] = -1;
    delta_sigma[i] = 1.;
  }
}

Min_delta_sigma_heap::~Min_delta_sigma_heap() {
  delete[] H;
  delete[] I;
  delete[] delta_sigma;
}

bool Min_delta_sigma_heap::is_empty() {
  return (size == 0);
}


}

