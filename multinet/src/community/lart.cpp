#include "community.h"


template <typename T>
void ignore(T &&)
{ }

namespace mlnet {


hash_set<ActorSharedPtr> lart::get_ml_community(MLNetworkSharedPtr mnet, uint32_t t, float eps, float gamma) {
	ignore(t); ignore(eps); ignore(gamma);
	hash_set<ActorSharedPtr> actors;

	std::vector<boost::numeric::ublas::matrix<int>> a = ml_network2adj_matrix(mnet);



	return actors;
}

std::vector<boost::numeric::ublas::matrix<int>> lart::ml_network2adj_matrix(MLNetworkSharedPtr mnet) {

	size_t N = mnet->get_layers()->size();
	size_t M = mnet->get_nodes()->size();

	std::vector<boost::numeric::ublas::matrix<int>> adj(N);

	for (LayerSharedPtr l: *mnet->get_layers()) {
		boost::numeric::ublas::matrix<int> m = boost::numeric::ublas::zero_matrix<int>(M);

		std::cout << l->id - 1 << std::endl;
		for (EdgeSharedPtr e: *mnet->get_edges(l, l)) {
			m(e->v1->id - 1, e->v2->id - 1) = 1;
			m(e->v2->id - 1, e->v1->id - 1) = 1;
		}
		std::cout << "---------" << std::endl;

		adj[l->id - 1] = m;
	}

	return adj;
}

}