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

/**
 * A property matrix is a view over a network that associates a value to each structure
 * (e.g., node, pair of nodes, ...) in each context (e.g., layer).
 */
template <class STRUCTURE, class CONTEXT, class VALUE>
class property_matrix {

private:
	hash_set<STRUCTURE> _structures;
	hash_set<CONTEXT> _contexts;
	hash_map<CONTEXT,hash_map<STRUCTURE,VALUE> > data;
	VALUE default_value;

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
    VALUE get(const STRUCTURE& s, const CONTEXT& c) const;

    /**
	 * @param s a structure
	 * @param c a context
     * @param v the value to be associated to the input structure in the input context
     */
    void set(const STRUCTURE& s, const CONTEXT& c, VALUE v);

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
VALUE property_matrix<STRUCTURE,CONTEXT,VALUE>::get(const STRUCTURE& s, const CONTEXT& c) const {
	if (data.count(c)==0)
		return default_value;
	if (data.at(c).count(s)==0)
		return default_value;
	return data.at(c).at(s);
}

template <class STRUCTURE, class CONTEXT, class VALUE>
void property_matrix<STRUCTURE,CONTEXT,VALUE>::set(const STRUCTURE& s, const CONTEXT& c, VALUE v) {
	data[c][s] = v;
	_contexts.insert(c); // TODO this might slow down the function significantly - check
	_structures.insert(s);
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
            if (min>P.get(s,c))
                min = P.get(s,c);
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
            if (max<P.get(s,c))
                max = P.get(s,c);
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
            sum += (double)P.get(s,c);
            checked_columns++;
        }
        sum += P.get_default()*(P.num_structures-checked_columns);
        return sum;
    }
    
    template <class STRUCTURE, class CONTEXT, class VALUE>
    double mean(const property_matrix<STRUCTURE,CONTEXT,VALUE>& P, const CONTEXT& c) {
        double sum = sum(P,c);;
        
        return sum/P.num_structures;
    }
    
    template <class STRUCTURE, class CONTEXT, class VALUE>
    double sd(const property_matrix<STRUCTURE,CONTEXT,VALUE>& P, const CONTEXT& c) {
        double mean = mean(P,c);
        
        double sd = 0.0;
        
        long checked_columns = 0;
        
        for (STRUCTURE s: P.structures()) {
            sd += (double)pow(P.get(s,c)-mean,2);
            checked_columns++;
        }
        sd += (double)std::pow(P.get_default()-mean,2)*(P.num_structures-checked_columns);
        return std::sqrt(sd/P.num_structures);
    }
    
    template <class STRUCTURE, class CONTEXT, class VALUE>
    double skew(const property_matrix<STRUCTURE,CONTEXT,VALUE>& P, const CONTEXT& c) {
        double sd = sd(P,c);
        
        double skew = 0.0;
        
        long checked_columns = 0;
        
        for (STRUCTURE s: P.structures()) {
            skew += (double)pow(P.get(s,c)-mean,3);
            checked_columns++;
        }
        skew += (double)std::pow(P.get_default()-mean,3)*(P.num_structures-checked_columns);
        return skew/std::pow(sd,3)/P.num_structures;
    }
    
    
    template <class STRUCTURE, class CONTEXT, class VALUE>
    double kurt(const property_matrix<STRUCTURE,CONTEXT,VALUE>& P, const CONTEXT& c) {
        double sd = sd(P,c);
        
        double kurt = 0.0;
        
        long checked_columns = 0;
        
        for (STRUCTURE s: P.structures()) {
            kurt += (double)pow(P.get(s,c)-mean,4);
            checked_columns++;
        }
        kurt += (double)std::pow(P.get_default()-mean,4)*(P.num_structures-checked_columns);
        return kurt/std::pow(sd,4)/P.num_structures;
    }
    
    template <class STRUCTURE, class CONTEXT, class VALUE>
    double entropy(const property_matrix<STRUCTURE,CONTEXT,VALUE>& P, const CONTEXT& c) {
        Counter<double> count;
        
        double entropy = 0.0;
        
        for (STRUCTURE s: P.structures()) {
            count.inc(P.get(s,c));
        }
        for (auto pair: count.map()) {
            double fr = pair.second/P.num_structures;
            entropy += fr*std::log(fr);
        }
        return entropy;
    }
    
    
    /*
     * K number of bins
     
     assumes only non-negative
     
     check if empty bins
     */
    template <class STRUCTURE, class CONTEXT, class VALUE>
    double dissimilarity_index(const property_matrix<STRUCTURE,CONTEXT,VALUE>& P, const CONTEXT& c1, const CONTEXT& c2, int K) {
        Counter<int> hist1;
        Counter<int> hist2;
        
        // build histograms
        double min = std::min(min(P,c1),min(P,c2));
        double max = std::max(max(P,c1),max(P,c2));
        
        if (min==max) ; // TODO do something...
        for (STRUCTURE s: P.structures()) {
            int v1 = std::floor((P.get(s,c1)-min)*K/(max-min));
            if (v1==K) v1=K-1;
            hist1.inc(v1);
            
            int v2 = std::floor((P.get(s,c2)-min)*K/(max-min));
            if (v2==K) v2=K-1;
            hist2.inc(v2);
        }
        // compare histograms
        double diss = 0;
        for (int i=0; i<K; i++) {
            diss += hist1.count(i)/K; // NORMALIZE TO FREQUENCY;
        }
        
        return diss*.5;
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
		bool p1 = P.get(s,c1);
		bool p2 = P.get(s,c2);
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
double pearson(const property_matrix<STRUCTURE,CONTEXT,double>& P, const CONTEXT& c1, const CONTEXT& c2) {
	double cov = 0;
	double mean1 = 0;
	double mean2 = 0;
	double std1 = 0;
	double std2 = 0;
	long checked_columns = 0;

	double default_val = P.get_default();
	for (STRUCTURE s:  P.structures()) {
		mean1 += P.get(s,c1);
		mean2 += P.get(s,c2);
		checked_columns++;
	}
	mean1 += default_val*(P.num_structures-checked_columns);
	mean2 += default_val*(P.num_structures-checked_columns);
	mean1 /= P.num_structures;
	mean2 /= P.num_structures;
	for (STRUCTURE s: P.structures()) {
		double val1 = P.get(s,c1);
		double val2 = P.get(s,c2);
		cov += (val1-mean1)*(val2-mean2);
		std1 += (val1-mean1)*(val1-mean1);
		std2 += (val2-mean2)*(val2-mean2);
		//std::cout << val2 << " " << mean2 << " " << ((val2-mean2)*(val2-mean2)) << std::endl;
	}
	cov += (default_val-mean1)*(default_val-mean2)*(P.num_structures-checked_columns);
	std1 += (default_val-mean1)*(default_val-mean1)*(P.num_structures-checked_columns);
	std2 += (default_val-mean2)*(default_val-mean2)*(P.num_structures-checked_columns);
	//std::cout << val2 << " " << mean2 << " " << ((val2-mean2)*(val2-mean2)) << std::endl;


	if (std1==0 && std2==0)
		return 1;
	else if (std1==0 || std2==0)
		return 0;
	else return cov/std::sqrt(std1)/std::sqrt(std2);
}


template <class STRUCTURE, class CONTEXT, class NUMBER>
class structure_comparison_function {
public:
	structure_comparison_function(const property_matrix<STRUCTURE,CONTEXT,NUMBER>* P, const CONTEXT* c)
	: P(P), c(c) {}
	const property_matrix<STRUCTURE,CONTEXT,NUMBER>* P;
	const CONTEXT* c;
	bool operator()(const STRUCTURE& s1, const STRUCTURE& s2) const {
		return (*P).get(s1,*c)<(*P).get(s2,*c);
	}
};

template <class STRUCTURE, class CONTEXT, class NUMBER>
void property_matrix<STRUCTURE,CONTEXT,NUMBER>::rankify() {
	for (CONTEXT c: contexts()) {
	std::vector<STRUCTURE> ranks(_structures.begin(),_structures.end());

	structure_comparison_function<STRUCTURE,CONTEXT,NUMBER> f(this,&c);
	std::sort(ranks.begin(), ranks.end(), f);

	uint i=0;
	while (i<ranks.size()) {
		NUMBER val = get(ranks[i],c);
		uint last_tie = i;
		while (i+1<ranks.size() && get(ranks[i+1],c)==val) {
			i++;
		}
		for (uint j=last_tie; j<=i; j++) {
			set(ranks[j],c,((double)last_tie+i)/2+1);
		}
		i++;
	}
	}
}

} // namespace mlnet

#endif /* MLNET_PROPERTY_MATRIX_H_ */
