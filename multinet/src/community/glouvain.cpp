#include "community.h"

namespace mlnet {

    set_type possible_moves(group_index & g, int node, Eigen::SparseMatrix<double> mod){
        set_type unique_groups(g.n_groups);
        unique_groups.insert(g.nodes[node]);
        //add nodes with potential positive contribution to unique_groups
        for (int i = 0; i < mod.outerSize(); i++) {
            for (Eigen::SparseMatrix<double>::InnerIterator it(mod, i); it; ++it) {
                if (it.value() > 0) {
                    unique_groups.insert(g.nodes[it.row()]);
                }
            }
        }
        
        return unique_groups;
    }
    
    //calculates changes in modularity for sparse modularity matrix
    map_type mod_change(group_index & g, Eigen::SparseMatrix<double> mod, set_type & unique_groups, int current_node){
        int current_group = g.nodes[current_node];
        map_type mod_c;
        
        double mod_current = mod.coeff(current_node, 0);
        for(set_type::iterator it=unique_groups.begin(); it!=unique_groups.end();++it){
            mod_c[*it] = 0;
        }
        
        for (int i = 0; i < mod.outerSize(); i++) {
            for (Eigen::SparseMatrix<double>::InnerIterator it(mod, i); it; ++it) {
                if (unique_groups.count(g.nodes[it.row()])) {
                    mod_c[g.nodes[it.row()]] += it.value();
                }
            }
        }
        
        mod_c[current_group]-=mod_current;
        mod_current=mod_c[current_group];
        for (set_type::iterator it=unique_groups.begin(); it!=unique_groups.end(); ++it) {
            mod_c[*it]-= mod_current;
        }
        
        return mod_c;
    }
    
    //find moves that improve modularity
    move_list positive_moves(set_type & unique_groups, map_type & mod_c){
        move_list moves;
        for(set_type::iterator it=unique_groups.begin();it!=unique_groups.end();++it){
            if(mod_c[*it]>NUM_TOL){
                moves.first.push_back(*it);
                moves.second.push_back(mod_c[*it]);
            }
        }
        return moves;
    }
    
    //move best move
    double move(group_index & g, int node, Eigen::SparseMatrix<double> mod){
        set_type unique_groups = possible_moves(g, node, mod);
        map_type mod_c = mod_change(g, mod, unique_groups, node);
        double mod_max = 0;
        double d_step = 0;
        int group_move = g.nodes[node]; //stay in current group if no improvement
        for(set_type::iterator it=unique_groups.begin();it!=unique_groups.end();++it){
            if(mod_c[*it]>mod_max){
                mod_max=mod_c[*it];
                group_move=*it;
            }
        }
        
        //move current node to most optimal group
        if(mod_max > NUM_TOL){
            g.move(node, group_move);
            d_step+=mod_max;
        }
        return d_step;
    }
    
    // Random engine used for random movement function, moverandw
    std::default_random_engine generator((unsigned int)time(0));
    
    //move to random group with probability proportional to increase in modularity
    double moverandw(group_index & g, int node, Eigen::SparseMatrix<double> mod) {
        set_type unique_groups = possible_moves(g, node, mod);
        map_type mod_c = mod_change(g, mod, unique_groups, node);
        
        //find modularity increasing moves
        move_list mod_pos = positive_moves(unique_groups, mod_c);
        
        //move node to a random group that increases modularity with probability proportional to the increase
        double d_step=0;
        if (!mod_pos.first.empty()) {
            std::discrete_distribution<int> randindex(mod_pos.second.begin(),mod_pos.second.end());
            int randmove = randindex(generator);
            g.move(node, mod_pos.first[randmove]);
            d_step = mod_pos.second[randmove];
        }
        return d_step;
    }

    ///
    
std::vector<int> glouvain::mapV2I(std::vector<int> a, std::vector<int> b) {
	std::vector<int> v(b.size());
	for (size_t i = 0; i < b.size(); i++) {
		v[i] = a[b[i]];
	}
	return v;
}

double glouvain::Q(Eigen::SparseMatrix<double> M, std::vector<int> y, double twoum) {
	Eigen::SparseMatrix<double> P(y.size(), y.size());

	std::vector<Eigen::Triplet<double>> tlist;
	tlist.reserve(y.size());

	for (size_t i = 0; i < y.size(); i++) {
		tlist.push_back(Eigen::Triplet<double>(i, i, 1));
	}

	P.setFromTriplets(tlist.begin(), tlist.end());
	return Eigen::MatrixXd((P*M).cwiseProduct(P)).sum() / twoum;
}

double glouvain::Q_handle(metanet M, std::vector<int> y, double twoum) {
	Eigen::SparseMatrix<double> P(y.size(), y.size());

	std::vector<Eigen::Triplet<double>> tlist;
	tlist.reserve(y.size());
	for (size_t i = 0; i < y.size(); i++) {
		tlist.push_back(Eigen::Triplet<double>(i, i, 1));
	}
	P.setFromTriplets(tlist.begin(), tlist.end());

	double Q = 0;
	for (size_t i = 0; i < y.size(); i++) {
		Q += Eigen::MatrixXd(Eigen::SparseMatrix<double>(P * M.get(i)).transpose() * P.col(i)).array().sum();
	}

	return Q / twoum;
}

CommunityStructureSharedPtr glouvain::fit(MLNetworkSharedPtr mnet, std::string m, double gamma, double omega, size_t limit) {
	DTRACE4(GLOUVAIN_START, mnet->name.c_str(), m.c_str(), std::to_string(gamma), std::to_string(omega));

	double (*move_func)(group_index &, int, Eigen::SparseMatrix<double>);
	if ("moverandw" == m) {
		move_func = &moverandw;
	} else {
		move_func = &move;
	}

	std::vector<int> S(mnet->get_actors()->size() * mnet->get_layers()->size());
	std::iota(S.begin(), S.end(), 0);

	std::vector<int> y, Sb;
	y = S;

	double eps = 2.2204e-12;
	double dtot = 0;
	Eigen::SparseMatrix<double> B, M;

	double twoum = 0;
	if (limit < y.size()) {
		std::vector<Eigen::SparseMatrix<double>> A = cutils::ml_network2adj_matrix(mnet);
		for (auto m: A){
			twoum += m.nonZeros();
		}
		twoum = twoum + (A[0].rows() * A.size() * (A.size() - 1) * omega);

		metanet meta(A, gamma, omega);
		meta.assign(S);

		while (Sb != S) {
			DTRACE1(GLOUVAIN_PASS, y.size());
			Sb = S;
			std::vector<int> yb;

			while (yb != y) {
				double dstep = 1.0;

				while (yb != y && (dstep/dtot > 2 * eps) && (dstep > 10 * eps)) {
					DTRACE0(GLOUVAIN_FIRST_PHASE_START);
					yb = y;
					dstep = 0;

					group_index g(y);
					for (int i: cutils::range(meta.get(0).rows(), true)) {
						double di = move_func(g, i, meta.get(i));
						dstep = dstep + di;
					}

					dtot = dtot + dstep;
					y = g.toVector();
					DTRACE3(GLOUVAIN_FIRST_PHASE_END, y.size(), std::to_string(dstep), std::to_string(dtot));
				}
				yb = y;
			}

			S = mapV2I(y, S);
			y = cutils::unique(S);

			if (Sb == S) {
				std::vector<unsigned int> partition(S.begin(), S.end());
				DTRACE4(GLOUVAIN_END, y.size(), meta.get(0).rows(), meta.get(0).cols(), std::to_string(Q_handle(meta, y, twoum)));
				return cutils::nodes2communities(mnet, partition);
			}

			meta.assign(S);

			if (y.size() < limit) {
				std::vector<Eigen::Triplet<double>> tlist;
				Eigen::SparseMatrix<double> t = meta.get(0);
				tlist.reserve(t.nonZeros() * y.size());

				for (int i = 0; i < (int) y.size(); i++) {
					Eigen::SparseMatrix<double> tmp = meta.get(i);
					for (int j = 0; j < tmp.outerSize(); ++j) {
						for (Eigen::SparseMatrix<double>::InnerIterator it(tmp, j); it; ++it) {
							tlist.push_back(Eigen::Triplet<double>(it.row(), i, it.value()));
						}
					}
				}

				B = Eigen::SparseMatrix<double>(t.rows(), y.size());
				B.setFromTriplets(tlist.begin(), tlist.end());
				M = B;
				break;
			}
		}
	} else {
		B = cutils::ng_modularity(twoum, cutils::ml_network2adj_matrix(mnet), gamma, omega);
		M = B;
	}

	std::vector<int> S2(B.rows());
	std::iota(S2.begin(), S2.end(), 0);
	Sb.clear();

	while (Sb != S2) {
		DTRACE1(GLOUVAIN_PASS, y.size());
		Sb = S2;
		std::vector<int> yb;

		while (yb != y) {
				double dstep = 1.0;

				while (yb != y && (dstep/dtot > 2 * eps) && (dstep > 10 * eps)) {
					DTRACE0(GLOUVAIN_FIRST_PHASE_START);
					yb = y;
					dstep = 0;

					group_index g(y);
					for (int i: cutils::range(M.cols(), true)) {
						double di = move_func(g, i, M.col(i));
						dstep = dstep + di;
					}

					dtot = dtot + dstep;
					y = g.toVector();
					DTRACE3(GLOUVAIN_FIRST_PHASE_END, y.size(), std::to_string(dstep), std::to_string(dtot));
				}
				yb = y;
		}

		S = mapV2I(y, S);
		S2 = mapV2I(y, S2);

		if (Sb == S2) {
			break;
		}

		M = metanetwork(B, S2);
		y = cutils::unique(S2);

	}

	std::vector<unsigned int> partition(S.begin(), S.end());
	DTRACE4(GLOUVAIN_END, y.size(), M.rows(), M.cols(), std::to_string(Q(M, y, twoum)));
	return cutils::nodes2communities(mnet, partition);
}


Eigen::SparseMatrix<double> glouvain::metanetwork(Eigen::SparseMatrix<double> B, std::vector<int> S2) {
	Eigen::SparseMatrix<double> PP(B.rows(), *std::max_element(S2.begin(), S2.end()) + 1);
	PP.reserve(B.rows());

	std::vector<Eigen::Triplet<double>> tlist;
	tlist.reserve(B.rows());

	for (size_t i = 0; i < S2.size(); i++) {
		tlist.push_back(Eigen::Triplet<double>(i, S2[i], 1));
	}

	PP.setFromTriplets(tlist.begin(), tlist.end());
	return PP.transpose() * B * PP;
}

}
