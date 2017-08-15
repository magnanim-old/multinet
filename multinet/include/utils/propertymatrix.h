/**
 * propertymatrix.h
 *
 * A property matrix is a view over a network that associates a value to each structure
 * (e.g., node, pair of nodes, ...) in each context (e.g., layer).
 *
 * Several generic summarization functions can be computed on a property matrix, e.g., to
 * obtain the amount of overlapping or the statistical correlation between different types
 * of structures in different layers.
 */

#ifndef MLNET_PROPERTY_MATRIX_H_
#define MLNET_PROPERTY_MATRIX_H_

#include "random.h"
#include "math.h"
#include <string>
#include <array>
#include <memory>
#include <cmath>
#include <algorithm>

namespace mlnet {

template <class VALUE>
    struct pm_value {
        VALUE val;
        bool is_na = false;
        pm_value<VALUE>() {};
        pm_value<VALUE>(VALUE _val, bool _is_na) : val(_val), is_na(_is_na) {};
    };
    
/**
 * A property matrix is a view over a network that associates a value to each structure
 * (e.g., node, pair of nodes, ...) in each context (e.g., layer).
 */
template <class STRUCTURE, class CONTEXT, class VALUE>
class property_matrix {

private:
	hash_set<STRUCTURE> _structures;
	hash_set<CONTEXT> _contexts;
	hash_map<CONTEXT,hash_map<STRUCTURE,pm_value<VALUE> > > data;
	VALUE default_value;
    Counter<CONTEXT> na;

public:
	/** number of structures in this matrix (e.g., actors, or edges) */
	const long num_structures;
	/** number of observation contexts, typically layers */
	const long num_contexts;

	/**
	 * Creates a property matrix with a given number of columns and rows.
	 * @param num_contexts
	 * @param num_structures
	 * @param default_value
	 */
	property_matrix(long num_structures, long num_contexts, VALUE default_value);

	/**
	 * @param s a structure
	 * @param c a context
	 * @return the value associated to the input structure in the input context
	 */
    pm_value<VALUE> get(const STRUCTURE& s, const CONTEXT& c) const;

    /**
	 * @param s a structure
	 * @param c a context
     * @param v the value to be associated to the input structure in the input context
     */
    void set(const STRUCTURE& s, const CONTEXT& c, VALUE v);

    /**
     * Sets s in c as not available (NA)
     * @param s a structure
     * @param c a context
     */
    void set_na(const STRUCTURE& s, const CONTEXT& c);
    
    /**
     * number of "not available" values in c (NA)
     * @param c a context
     */
    long num_na(const CONTEXT& c) const;
    
    
    /**
     * Changes the fields of the matrix replacing absolute values with their ranking for each context.
     * For example, a context (1.2, 5.4, 5) would be replaced by (1, 3, 2)
     */
    void rankify();

    /**
     * @return a set of all contexts in the matrix
     */
    const hash_set<CONTEXT>& contexts() const;

    /**
     * @return a set of all structures in the matrix
     */
    const hash_set<STRUCTURE>& structures() const;

    /**
     * @return the default value assigned to a structure for a context where no explicit value has been specified.
     */
    VALUE get_default() const;
};

/* TEMPLATE CODE */

template <class STRUCTURE, class CONTEXT, class VALUE>
property_matrix<STRUCTURE,CONTEXT,VALUE>::property_matrix(long num_structures, long num_contexts, VALUE default_value) :
default_value(default_value), num_structures(num_structures), num_contexts(num_contexts) {}

template <class STRUCTURE, class CONTEXT, class VALUE>
pm_value<VALUE> property_matrix<STRUCTURE,CONTEXT,VALUE>::get(const STRUCTURE& s, const CONTEXT& c) const {
	if (data.count(c)==0)
		return pm_value<VALUE>(default_value,false);
	if (data.at(c).count(s)==0)
		return pm_value<VALUE>(default_value,false);
	return data.at(c).at(s);
}

template <class STRUCTURE, class CONTEXT, class VALUE>
void property_matrix<STRUCTURE,CONTEXT,VALUE>::set(const STRUCTURE& s, const CONTEXT& c, VALUE v) {
    data[c][s] = pm_value<VALUE>(v,false);
	_contexts.insert(c); // TODO this might slow down the function significantly - check
	_structures.insert(s);
}
    
    template <class STRUCTURE, class CONTEXT, class VALUE>
    void property_matrix<STRUCTURE,CONTEXT,VALUE>::set_na(const STRUCTURE& s, const CONTEXT& c) {
        pm_value<VALUE> v = get(s,c);
        if (!v.is_na) na.inc(c);
        data[c][s] = pm_value<VALUE>(v.val,true);
        _contexts.insert(c); // TODO this might slow down the function significantly - check
        _structures.insert(s);
    }
    
    template <class STRUCTURE, class CONTEXT, class VALUE>
    long property_matrix<STRUCTURE,CONTEXT,VALUE>::num_na(const CONTEXT& c) const {
        return na.count(c);
    }

template <class STRUCTURE, class CONTEXT, class VALUE>
const hash_set<CONTEXT>& property_matrix<STRUCTURE,CONTEXT,VALUE>::contexts() const {
	return _contexts;
}

template <class STRUCTURE, class CONTEXT, class VALUE>
const hash_set<STRUCTURE>& property_matrix<STRUCTURE,CONTEXT,VALUE>::structures() const {
	return _structures;
}

/**
 * The result of the comparison of two binary vectors, comparing their elements at each coordinate
 * and counting the number of occurrences for all possible configurations (true-true, true-false, false-true, false-false)
 */
struct binary_vector_comparison {
public:
	/** number of coordinates where both vectors are true */
	long yy = 0;
	/** number of coordinates where the first vector is true and the second is false */
	long yn = 0;
	/** number of coordinates where the first vector is false and the second is true */
	long ny = 0;
	/** number of coordinates where both vectors are false */
	long nn = 0;
};

template <class STRUCTURE, class CONTEXT, class VALUE>
VALUE property_matrix<STRUCTURE,CONTEXT,VALUE>::get_default() const {
	return default_value;
}

/**
 * Basic context summaries
 */
    
    template <class STRUCTURE, class CONTEXT, class VALUE>
    double min(const property_matrix<STRUCTURE,CONTEXT,VALUE>& P, const CONTEXT& c) {
        double min = std::numeric_limits<double>::infinity();
        
        long checked_columns = 0;
         for (STRUCTURE s: P.structures()) {
             pm_value<VALUE> v = P.get(s,c);
             if (!v.is_na && min>v.val)
                 min = v.val;
             checked_columns++;
         }
        if ((P.num_structures>checked_columns) && min>P.get_default())
            min = P.get_default();
        return min;
    }
    
    
    template <class STRUCTURE, class CONTEXT, class VALUE>
    double max(const property_matrix<STRUCTURE,CONTEXT,VALUE>& P, const CONTEXT& c) {
        double max = -std::numeric_limits<double>::infinity();
        
        long checked_columns = 0;
        for (STRUCTURE s: P.structures()) {
            pm_value<VALUE> v = P.get(s,c);
            if (!v.is_na && max<v.val) max = v.val;
            checked_columns++;
        }
        if ((P.num_structures>checked_columns) && max<P.get_default())
            max = P.get_default();
        return max;
    }
    
    template <class STRUCTURE, class CONTEXT, class VALUE>
    double sum(const property_matrix<STRUCTURE,CONTEXT,VALUE>& P, const CONTEXT& c) {
        double sum = 0.0;
        
        long checked_columns = 0;
        
        for (STRUCTURE s: P.structures()) {
            pm_value<VALUE> v = P.get(s,c);
            if (!v.is_na) sum += (double)v.val;
            checked_columns++;
        }
        sum += P.get_default()*(P.num_structures-checked_columns);
        return sum;
    }
    
    template <class STRUCTURE, class CONTEXT, class VALUE>
    double mean(const property_matrix<STRUCTURE,CONTEXT,VALUE>& P, const CONTEXT& c) {
        double _mean = 0.0;
        
        long checked_columns = 0;
        
        for (STRUCTURE s: P.structures()) {
            pm_value<VALUE> v = P.get(s,c);
            if (!v.is_na) _mean += (double)v.val;
            checked_columns++;
        }
        _mean += P.get_default()*(P.num_structures-checked_columns);
        return _mean/(P.num_structures-P.num_na(c));
    }
    
    template <class STRUCTURE, class CONTEXT, class VALUE>
    double sd(const property_matrix<STRUCTURE,CONTEXT,VALUE>& P, const CONTEXT& c) {
        double _mean = mean(P,c);
        
        double sd = 0.0;
        
        long checked_columns = 0;
        
        for (STRUCTURE s: P.structures()) {
            pm_value<VALUE> v = P.get(s,c);
            if (!v.is_na) sd += (double)pow(v.val-_mean,2);
            checked_columns++;
        }
        sd += (double)std::pow(P.get_default()-_mean,2)*(P.num_structures-checked_columns);
        return std::sqrt(sd/(P.num_structures-P.num_na(c)));
    }
    
    template <class STRUCTURE, class CONTEXT, class VALUE>
    double skew(const property_matrix<STRUCTURE,CONTEXT,VALUE>& P, const CONTEXT& c) {
        double _mean = mean(P,c);
        
        double sd = 0.0;
        double skew = 0.0;
        
        long checked_columns = 0;
        
        for (STRUCTURE s: P.structures()) {
            pm_value<VALUE> v = P.get(s,c);
            if (!v.is_na) {
                sd += (double)pow(v.val-_mean,2);
                skew += (double)pow(v.val-_mean,3);
            }
            checked_columns++;
        }
        sd += (double)std::pow(P.get_default()-_mean,2)*(P.num_structures-checked_columns);
        sd = std::sqrt(sd/(P.num_structures-P.num_na(c)));
        skew += (double)std::pow(P.get_default()-_mean,3)*(P.num_structures-checked_columns);
        return skew/std::pow(sd,3)/(P.num_structures-P.num_na(c));
    }
    
    
    template <class STRUCTURE, class CONTEXT, class VALUE>
    double kurt(const property_matrix<STRUCTURE,CONTEXT,VALUE>& P, const CONTEXT& c) {
        double _mean = mean(P,c);
        
        double sd = 0.0;
        double kurt = 0.0;
        
        long checked_columns = 0;
        
        for (STRUCTURE s: P.structures()) {
            pm_value<VALUE> v = P.get(s,c);
            if (!v.is_na) {
                sd += (double)pow(v.val-_mean,2);
                kurt += (double)pow(v.val-_mean,4);
            }
            checked_columns++;
        }
        sd += (double)std::pow(P.get_default()-_mean,2)*(P.num_structures-checked_columns);
        sd = std::sqrt(sd/(P.num_structures-P.num_na(c)));
        kurt += (double)std::pow(P.get_default()-_mean,4)*(P.num_structures-checked_columns);
        return kurt/std::pow(sd,4)/(P.num_structures-P.num_na(c));
    }
    
    template <class STRUCTURE, class CONTEXT, class VALUE>
    double entropy(const property_matrix<STRUCTURE,CONTEXT,VALUE>& P, const CONTEXT& c) {
        Counter<double> count;
        
        double entropy = 0.0;
        
        long checked_columns = 0;
        

        for (STRUCTURE s: P.structures()) {
            pm_value<VALUE> v = P.get(s,c);
            if (!v.is_na) count.inc(v.val);
            checked_columns++;
        }
        count.set(P.get_default(),count.count(P.get_default())+(P.num_structures-checked_columns));
        for (auto pair: count.map()) {
            double fr = (double)pair.second/(P.num_structures-P.num_na(c));
            if (fr!=0) entropy += -fr*std::log(fr);
        }
        return entropy;
    }
    
    template <class STRUCTURE, class CONTEXT, class VALUE>
    double CV(const property_matrix<STRUCTURE,CONTEXT,VALUE>& P, const CONTEXT& c) {
        double _mean=mean(P,c);
        return sd(P,c)/_mean;
    }
    
    template <class STRUCTURE, class CONTEXT, class VALUE>
    double jarque_bera(const property_matrix<STRUCTURE,CONTEXT,VALUE>& P, const CONTEXT& c) {
        return (P.num_structures-P.num_na(c))/6.0*(std::pow(skew(P,c),2)+std::pow(kurt(P,c)-3,2)/4);
    }
    
    template <class STRUCTURE, class CONTEXT, class VALUE>
    double relative_difference(double (*f)(const property_matrix<STRUCTURE,CONTEXT,VALUE>& P, const CONTEXT& c), const property_matrix<STRUCTURE,CONTEXT,VALUE>& P, const CONTEXT& c1, const CONTEXT& c2) {
        double f1 = (*f)(P,c1);
        double f2 = (*f)(P,c2);
        if (std::abs(f1)+std::abs(f2)==0) return 0;
        return std::abs(f1-f2)*2/(std::abs(f1)+std::abs(f2));
    }
        
    
    /*
     * K number of bins
     
     assumes only non-negative
     
     check if empty bins
     */
    template <class STRUCTURE, class CONTEXT, class VALUE>
    std::pair<Counter<int>,Counter<int> > histograms(const property_matrix<STRUCTURE,CONTEXT,VALUE>& P, const CONTEXT& c1, const CONTEXT& c2, int K) {
        Counter<int> hist1;
        Counter<int> hist2;
        
        // build histograms
        double _min = std::min(min(P,c1),min(P,c2));
        double _max = std::max(max(P,c1),max(P,c2));
        long checked_columns = 0;
        
        for (STRUCTURE s: P.structures()) {
            pm_value<VALUE> val1 = P.get(s,c1);
            if (!val1.is_na) {
                int v1 = std::floor(((double)val1.val-_min)*K/(_max-_min));
                if (v1==K) v1=K-1;
                hist1.inc(v1);
            }
            
            pm_value<VALUE> val2 = P.get(s,c2);
            if (!val2.is_na) {
                int v2 = std::floor(((double)val2.val-_min)*K/(_max-_min));
                if (v2==K) v2=K-1;
                hist2.inc(v2);
            }
            checked_columns++;
        }
        hist1.set(P.get_default(),hist1.count(P.get_default())+(P.num_structures-checked_columns));
        hist2.set(P.get_default(),hist2.count(P.get_default())+(P.num_structures-checked_columns));
        return std::pair<Counter<int>,Counter<int> >(hist1,hist2);
    }

    
    template <class STRUCTURE, class CONTEXT, class VALUE>
    double dissimilarity_index(const property_matrix<STRUCTURE,CONTEXT,VALUE>& P, const CONTEXT& c1, const CONTEXT& c2, int K) {
        
        std::pair<Counter<int>,Counter<int> > h = histograms(P,c1,c2,K);
        
        // compare histograms
        double diss = 0;
        for (int i=0; i<K; i++) {
            double fr1 = (double)h.first.count(i)/(P.num_structures-P.num_na(c1));
            double fr2 = (double)h.second.count(i)/(P.num_structures-P.num_na(c2));
            diss += std::abs(fr1-fr2)*.5;
        }
        return diss;
    }
    
    // epsilon correction to avoid division by 0
    template <class STRUCTURE, class CONTEXT, class VALUE>
    double KL_divergence(const property_matrix<STRUCTURE,CONTEXT,VALUE>& P, const CONTEXT& c1, const CONTEXT& c2, int K) {
        
        std::pair<Counter<int>,Counter<int> > h = histograms(P,c1,c2,K);
        
        // compare histograms
        double diss = 0;
        for (int i=0; i<K; i++) {
            double fr1 = ((double)h.first.count(i)+1)/(K+P.num_structures-P.num_na(c1));
            double fr2 = ((double)h.second.count(i)+1)/(K+P.num_structures-P.num_na(c2));
            if (fr1!=0) diss += fr1*std::log(fr1/fr2);
        }
        return diss;
    }
    
    template <class STRUCTURE, class CONTEXT, class VALUE>
    double jeffrey_divergence(const property_matrix<STRUCTURE,CONTEXT,VALUE>& P, const CONTEXT& c1, const CONTEXT& c2, int K) {
        
        std::pair<Counter<int>,Counter<int> > h = histograms(P,c1,c2,K);
        
        // compare histograms
        double diss = 0;
        for (int i=0; i<K; i++) {
            double fr1 = (double)h.first.count(i)/(P.num_structures-P.num_na(c1));
            double fr2 = (double)h.second.count(i)/(P.num_structures-P.num_na(c2));
            double f_comp = (fr1+fr2)/2;
            if (fr1!=0) diss += fr1*std::log(fr1/f_comp);
            if (fr2!=0) diss += fr2*std::log(fr2/f_comp);
        }
        return diss;
    }
    
    
/**
 * Compares two binary vectors, comparing their elements at each coordinate
 * and counting the number of occurrences for all possible configurations (true-true, true-false, false-true, false-false)
 */
template <class STRUCTURE, class CONTEXT>
binary_vector_comparison compare(const property_matrix<STRUCTURE,CONTEXT,bool>& P, const CONTEXT& c1, const CONTEXT& c2) {
	binary_vector_comparison res;
	res.yy = 0;
	res.yn = 0;
	res.ny = 0;
	res.nn = 0;

	long checked_columns = 0;

	for (STRUCTURE s: P.structures()) {
        pm_value<bool> v1 = P.get(s,c1);
        bool p1 = v1.val && !v1.is_na;
        pm_value<bool> v2 = P.get(s,c2);
        bool p2 = v2.val && !v2.is_na;
        if (p1 && p2) res.yy++;
		else if (p1) res.yn++;
		else if (p2) res.ny++;
		else res.nn++;
		checked_columns++;
	}
	if (P.get_default()) res.yy += P.num_structures - checked_columns;
	else res.nn += P.num_structures - checked_columns;
	return res;
}

template <class STRUCTURE, class CONTEXT>
double russell_rao(const property_matrix<STRUCTURE,CONTEXT,bool>& P, const CONTEXT& c1, const CONTEXT& c2) {
	binary_vector_comparison comp = compare(P, c1, c2);
	return (double)(comp.yy)/(comp.yy+comp.ny+comp.yn+comp.nn);
}

template <class STRUCTURE, class CONTEXT>
double jaccard(const property_matrix<STRUCTURE,CONTEXT,bool>& P, const CONTEXT& c1, const CONTEXT& c2) {
    binary_vector_comparison comp = compare(P, c1, c2);
	return (double)comp.yy/(comp.yy+comp.yn+comp.ny);
}

template <class STRUCTURE, class CONTEXT>
double coverage(const property_matrix<STRUCTURE,CONTEXT,bool>& P, const CONTEXT& c1, const CONTEXT& c2) {
    binary_vector_comparison comp = compare(P, c1, c2);
	return (double)comp.yy/(comp.yy+comp.ny);
}

template <class STRUCTURE, class CONTEXT>
double kulczynski2(const property_matrix<STRUCTURE,CONTEXT,bool>& P, const CONTEXT& c1, const CONTEXT& c2) {
	binary_vector_comparison comp = compare(P, c1, c2);
	return ((double)comp.yy/(comp.yy+comp.yn)+(double)comp.yy/(comp.yy+comp.ny))/2;
}

template <class STRUCTURE, class CONTEXT>
double simple_matching(const property_matrix<STRUCTURE,CONTEXT,bool>& P, const CONTEXT& c1, const CONTEXT& c2) {
	binary_vector_comparison comp = compare(P, c1, c2);
	return (double)(comp.yy+comp.nn)/(comp.yy+comp.ny+comp.yn+comp.nn);
}
    
    template <class STRUCTURE, class CONTEXT>
    double hamann(const property_matrix<STRUCTURE,CONTEXT,bool>& P, const CONTEXT& c1, const CONTEXT& c2) {
        binary_vector_comparison comp = compare(P, c1, c2);
        return (double)(comp.yy+comp.nn-comp.yn-comp.ny)/(comp.yy+comp.ny+comp.yn+comp.nn);
    }

template <class STRUCTURE, class CONTEXT>
double pearson(const property_matrix<STRUCTURE,CONTEXT,double>& P, const CONTEXT& c1, const CONTEXT& c2) {
	double cov = 0;
	double mean1 = 0;
	double mean2 = 0;
    double std1 = 0;
    double std2 = 0;
    
    long checked_columns = 0;
    long num_incomplete = 0;
    double default_val = P.get_default();
    for (STRUCTURE s: P.structures()) {
        pm_value<double> v1 = P.get(s,c1);
        pm_value<double> v2 = P.get(s,c2);
        if (!v1.is_na && !v2.is_na) {
            mean1 += v1.val;
            mean2 += v2.val;
        }
        else num_incomplete++;
        checked_columns++;
    }
    mean1 += P.get_default()*(P.num_structures-checked_columns);
    mean2 += P.get_default()*(P.num_structures-checked_columns);
    mean1 /= (P.num_structures-num_incomplete);
    mean2 /= (P.num_structures-num_incomplete);
    
    checked_columns = 0;
    for (STRUCTURE s: P.structures()) {
        pm_value<double> v1 = P.get(s,c1);
        pm_value<double> v2 = P.get(s,c2);
        if (!v1.is_na && !v2.is_na) {
            cov += (v1.val-mean1)*(v2.val-mean2);
            std1 += (v1.val-mean1)*(v1.val-mean1);
            std2 += (v2.val-mean2)*(v2.val-mean2);
        }
		//if (!v1.is_na) std1 += (v1.val-_mean1)*(v1.val-_mean1);
        //if (!v2.is_na) std2 += (v2.val-_mean2)*(v2.val-_mean2);
        checked_columns++;
		//std::cout << val2 << " " << mean2 << " " << ((val2-mean2)*(val2-mean2)) << std::endl;
	}
	cov += (default_val-mean1)*(default_val-mean2)*(P.num_structures-checked_columns);
	std1 += (default_val-mean1)*(default_val-mean1)*(P.num_structures-checked_columns);
	std2 += (default_val-mean2)*(default_val-mean2)*(P.num_structures-checked_columns);
	//std::cout << val2 << " " << mean2 << " " << ((val2-mean2)*(val2-mean2)) << std::endl;

	return cov/std::sqrt(std1)/std::sqrt(std2);
}


template <class STRUCTURE, class CONTEXT, class NUMBER>
class structure_comparison_function {
public:
	structure_comparison_function(const property_matrix<STRUCTURE,CONTEXT,NUMBER>* P, const CONTEXT* c)
	: P(P), c(c) {}
	const property_matrix<STRUCTURE,CONTEXT,NUMBER>* P;
	const CONTEXT* c;
	bool operator()(const STRUCTURE& s1, const STRUCTURE& s2) const {
        pm_value<NUMBER> v1 = (*P).get(s1,*c);
        pm_value<NUMBER> v2 = (*P).get(s2,*c);
        if (v1.is_na || v2.is_na) return !v1.is_na<!v2.is_na; // TODO
		return v1.val<v2.val;
	}
};

template <class STRUCTURE, class CONTEXT, class NUMBER>
void property_matrix<STRUCTURE,CONTEXT,NUMBER>::rankify() {
	for (CONTEXT c: contexts()) {
	std::vector<STRUCTURE> ranks(_structures.begin(),_structures.end());

	structure_comparison_function<STRUCTURE,CONTEXT,NUMBER> f(this,&c);
	std::sort(ranks.begin(), ranks.end(), f);

	size_t i=0;
	while (i<ranks.size()) {
        pm_value<NUMBER> v1 = get(ranks[i],c);
        if (v1.is_na) {i++; continue;}
		size_t last_tie = i;
		while (i+1<ranks.size()) {
            pm_value<NUMBER> v2 = get(ranks[i+1],c);
            if (v1.is_na || v2.val>v1.val) {break;}
			i++;
		}
		for (size_t j=last_tie; j<=i; j++) {
			set(ranks[j],c,((double)last_tie+i)/2+1);
		}
		i++;
	}
	}
}

} // namespace mlnet

#endif /* MLNET_PROPERTY_MATRIX_H_ */
