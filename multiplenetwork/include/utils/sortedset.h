/**
 * sortedset.h
 * 
 * A sorted set is a class used to store a set of objects that can be accessed:
 * 1. by id in (average) log time.
 * 2. by index (position) in constant time.
 * 3. by const iterating over its elements.
 * When several components of a multilayer network need to be accessed,
 * e.g., the neighbors of a node, a const reference to the corresponding
 * sorted_set is typically returned so that no objects are duplicated and
 * no additional memory is used.
 *
 * Here a sorted_set is implemented as a skip list.
 */

#ifndef MLNET_SORTED_SET_H_
#define MLNET_SORTED_SET_H_

#include "random.h"
#include "exceptions.h"
#include <string>
#include <map>
#include <unordered_map>
#include <set>
#include <vector>
#include <memory>
#include <cmath>

namespace mlnet {

template <class KEY, class VALUE> class sorted_set;

/**
 * An entry in a sorted set, which is implemented as a skip list.
 */
template <class KEY, class VALUE>
class sorted_set_entry {
	friend class sorted_set<KEY,VALUE>;

private:
	/** The id of the object corresponding to this entry */
	KEY id;
	/** The object corresponding to this entry */
	VALUE obj_ptr;
	/** An array of pointers to the next entry, for each level in the skip list */
    std::vector<sorted_set_entry<KEY,VALUE>*> forward; // array of pointers
    /** The number of entries before the next entry on each level, used for positional access */
    std::vector<int> link_length;

public:
    /**
     * Constructor.
     * @param level height of the entry in the skip list
     * @param id id of the object corresponding to this entry
     * @param obj the object corresponding to this entry
     */
    sorted_set_entry(int level, KEY id, VALUE obj);

    /**
     * This function is used to increase the level of the header.
     * @param number of skipped entries (to be set to the number of entries)
     */
    void increment(long skipped_entries);
};

/**
 * A sorted set is a class used to store a set of components that:
 * 1. can be accessed by id in log time.
 * 2. can be accessed by index (position) in the set in constant time.
 * 3. can be accessed using a const iterator, so that it is not necessary to duplicate them. As an example,
 * it is not necessary to duplicate nodes when the neighbors of a node are accessed: a sorted set on those
 * nodes is returned instead.
 *
 * A sorted set is implemented as a skip list.
 */
template <class KEY, class VALUE>
class sorted_set {
private:
	float P = 0.5;

    sorted_set_entry<KEY,VALUE> *header;
    /* Number of entries for which the sorted set is optimized. */
    long capacity = 1;
    /* Current number of entries. */
    long num_entries = 0;
    /* Maximum level */
	int MAX_LEVEL = 0;
    /* Current maximum level in use. */
    int level;

public:
	/**
	 * Creates a sorted set.
	 */
    sorted_set();
	/**
	 * Creates a sorted set optimized to store a pre-defined number of entries
	 * @param capacity the initial capacity for which the sorted set is optimized.
	 */
    sorted_set(long start_capacity);

    /** Iterator over the objects in this collection */
	class iterator {
	    typedef std::forward_iterator_tag iterator_category;
		public:
		iterator();
		/** Returns an iterator pointing at the input object */
		iterator(sorted_set_entry<KEY,VALUE>* iter);
		/** Return the object pointed by this iterator */
		VALUE operator*();
		/** Moves the iterator to the next object in the collection (prefix) */
		iterator operator++();
		/** Moves the iterator to the next object in the collection (postfix) */
		iterator operator++(int);
		/** Checks if this iterator equals the input one */
	    bool operator==(const sorted_set<KEY,VALUE>::iterator& rhs);
		/** Checks if this iterator differs from the input one */
	    bool operator!=(const sorted_set<KEY,VALUE>::iterator& rhs);
		private:
		/** Entry currently pointed to by this iterator */
		sorted_set_entry<KEY,VALUE>* current;
	};
	/** Returns an iterator to the first object in the collection */
	sorted_set<KEY,VALUE>::iterator begin() const;
	/** Returns an iterator after the last object in the collection */
	sorted_set<KEY,VALUE>::iterator end() const;
	/** Returns the number of objects in the collection */
    long size() const;
	/** Returns true if an object with the input id is present in the collection */
    bool contains(KEY) const;
	/** Returns the object with the input id if it is present in the collection, or NULL */
    VALUE get(KEY) const;
	/** Returns the object at the given position in the collection, or NULL */
    VALUE get_at_index(long) const;
	/** Returns a random object, uniform probability */
    VALUE get_at_random() const;
	/**
	 * Inserts a new object in the collection.
	 * @return true if KEY was not already present, false otherwise (in which case the object is updated with the new value)
	 * */
    bool insert(KEY,VALUE);
	/**
	 * Removes the input object from the collection.
	 * @return true if the object is removed from the collection, false if the object was not present.
	 */
    bool erase(KEY);
    //void print(int level); // only for debugging
    //void print(); // only for debugging
};

/* TEMPLATE CODE */

template <class KEY, class VALUE>
sorted_set_entry<KEY,VALUE>::sorted_set_entry(int level, KEY id, VALUE obj_ptr) {
    forward.resize(level+1);
    link_length.resize(level+1);
    this->id = id;
    this->obj_ptr = obj_ptr;
}

template <class KEY, class VALUE>
void sorted_set_entry<KEY,VALUE>::increment(long skipped_entries) {
	int current_size = forward.size();
	forward.resize(current_size+1,NULL);
	link_length.resize(current_size+1,skipped_entries);
}

template <class KEY, class VALUE>
sorted_set<KEY,VALUE>::sorted_set() {
	header = new sorted_set_entry<KEY,VALUE>(MAX_LEVEL, -1, NULL);
    level = 0;
}

template <class KEY, class VALUE>
sorted_set<KEY,VALUE>::sorted_set(long start_capacity) {
	capacity = start_capacity;
	MAX_LEVEL = std::ceil(std::log2(capacity));
	header = new sorted_set_entry<KEY,VALUE>(MAX_LEVEL, -1, NULL);
    level = 0;
}

template <class KEY, class VALUE>
typename sorted_set<KEY,VALUE>::iterator sorted_set<KEY,VALUE>::begin() const {
	return iterator(header->forward[0]);
}

template <class KEY, class VALUE>
typename sorted_set<KEY,VALUE>::iterator sorted_set<KEY,VALUE>::end() const {
	return iterator(NULL);
}

template <class KEY, class VALUE>
VALUE sorted_set<KEY,VALUE>::iterator::operator*() {
	return current->obj_ptr;
}

template <class KEY, class VALUE>
sorted_set<KEY,VALUE>::iterator::iterator(sorted_set_entry<KEY,VALUE>* iter) : current(iter) {
}

template <class KEY, class VALUE>
typename sorted_set<KEY,VALUE>::iterator sorted_set<KEY,VALUE>::iterator::operator++() { // PREFIX
	current=current->forward[0];
	return *this;
}

template <class KEY, class VALUE>
typename sorted_set<KEY,VALUE>::iterator sorted_set<KEY,VALUE>::iterator::operator++(int) { // POSTFIX
	sorted_set<KEY,VALUE>::iterator tmp(current);
	current=current->forward[0];
	return tmp;
}

template <class KEY, class VALUE>
bool sorted_set<KEY,VALUE>::iterator::operator==(const sorted_set<KEY,VALUE>::iterator& rhs) {
	return current == rhs.current;
}

template <class KEY, class VALUE>
bool sorted_set<KEY,VALUE>::iterator::operator!=(const sorted_set<KEY,VALUE>::iterator& rhs) {
	return current != rhs.current;
}

template <class KEY, class VALUE>
long sorted_set<KEY,VALUE>::size() const {
	return num_entries;
}

template <class KEY, class VALUE>
bool sorted_set<KEY,VALUE>::contains(KEY search_value) const {
    const sorted_set_entry<KEY,VALUE> *x = header;
    for (int i = level; i >= 0; i--) {
        while (x->forward[i] != NULL && x->forward[i]->id < search_value) {
            x = x->forward[i];
        }
    }
    x = x->forward[0];
    return x != NULL && x->id == search_value;
}

template <class KEY, class VALUE>
VALUE sorted_set<KEY,VALUE>::get(KEY search_value) const {
    const sorted_set_entry<KEY,VALUE> *x = header;
    for (int i = level; i >= 0; i--) {
        while (x->forward[i] != NULL && x->forward[i]->id < search_value) {
            x = x->forward[i];
        }
    }
    x = x->forward[0];
    if (x != NULL && x->id == search_value)
    	return x->obj_ptr;
    else return NULL;
}

template <class KEY, class VALUE>
VALUE sorted_set<KEY,VALUE>::get_at_index(long pos) const {
	if (pos < 0 || pos >= num_entries)
		throw ElementNotFoundException("Index out of bounds");
    const sorted_set_entry<KEY,VALUE> *x = header;
    long so_far=0;
    for (int i = level; i >= 0; i--) {
        while (x->forward[i] != NULL && x->link_length[i] + so_far <= pos + 1) {
        	so_far+= x->link_length[i];
            x = x->forward[i];
        }
    }
    return x->obj_ptr;
}

template <class KEY, class VALUE>
VALUE sorted_set<KEY,VALUE>::get_at_random() const {
	return get_at_index(getRandomInt(size()));
}

template <class KEY, class VALUE>
bool sorted_set<KEY,VALUE>::insert(KEY value, VALUE obj_ptr) {
	sorted_set_entry<KEY,VALUE> *x = header;
    std::vector<sorted_set_entry<KEY,VALUE>*> update;
    update.resize(level+1);
    std::vector<int> skipped_positions_per_level;
    int skipped_positions = 0;
    skipped_positions_per_level.resize(level+1,0);

    for (int i = level; i >= 0; i--) {
    	skipped_positions_per_level[i] = skipped_positions;
        while (x->forward[i] != NULL && x->forward[i]->id < value) {
        	skipped_positions_per_level[i] += x->link_length[i];
        	skipped_positions += x->link_length[i];
            x = x->forward[i];
        }
        update[i] = x;
    }
    x = x->forward[0];

    if (x == NULL || x->id != value) {
     	num_entries++;
    	if (num_entries>capacity) {
    		// resize the sorted list
    		capacity *= 2;
    		MAX_LEVEL++;
    		header->increment(num_entries);
    	}

        int lvl = random_level(MAX_LEVEL,P);

        if (lvl > level) {
            update.resize(lvl+1);
            skipped_positions_per_level.resize(lvl+1,0);
        	for (int i = level + 1; i <= lvl; i++) {
        		update[i] = header;
        		update[i]->link_length[i] = num_entries;
        	}
        	level = lvl;
        }
        x = new sorted_set_entry<KEY,VALUE>(lvl, value, obj_ptr);
        for (int i = 0; i <= lvl; i++) {
        	int offset = skipped_positions-skipped_positions_per_level[i];

        	x->forward[i] = update[i]->forward[i];
        	if (update[i]->forward[i]==NULL)
        		x->link_length[i] = num_entries - skipped_positions;
        	else {
        		x->link_length[i] = update[i]->link_length[i]-offset;
        	}

        	update[i]->forward[i] = x;
        	update[i]->link_length[i] = offset+1;
        }
        for (int i = lvl+1; i <= level; i++) {
        	update[i]->link_length[i]++;
        }
        return true;
    }
    else {
    	x->obj_ptr = obj_ptr;
    	return false;
    }
}

template <class KEY, class VALUE>
bool sorted_set<KEY,VALUE>::erase(KEY value) {
	sorted_set_entry<KEY,VALUE> *x = header;
	std::vector<sorted_set_entry<KEY,VALUE>*> update;
	update.resize(MAX_LEVEL+1);

	for (int i = level; i >= 0; i--) {
		while (x->forward[i] != NULL && x->forward[i]->id < value) {
			x = x->forward[i];
		}
		update[i] = x;
	}
	x = x->forward[0];

	if (x == NULL) return false;

	if (x->id == value) {
		for (int i = 0; i <= level; i++) {
			if (update[i]->forward[i] != x) {
				update[i]->link_length[i]--;
			}
			else {
				update[i]->forward[i] = x->forward[i];
				update[i]->link_length[i] += x->link_length[i]-1;
			}
		}
		delete x;
		num_entries--;
		while (level > 0 && header->forward[level] == NULL) {
			level--;
		}
		return true;
	}
	else return false;
}


/*
// only for debugging
template <class KEY, class VALUE>
void sorted_set<KEY,VALUE>::print(int lev) {
    const Entry<KEY,VALUE> *x = header;
    int num = 0;
    double std = 0;
    while (x!=NULL) {
    	std += x->link_length[lev]*x->link_length[lev];
    	num++;
        x = x->forward[lev];
    }
    std::cout << "l" << lev << " e: " << num << " std: " << std/num << "\n";
}

template <class KEY, class VALUE>
void sorted_set<KEY,VALUE>::print() {
std::cout << "cap:" << capacity << " num:" << num_entries << " lev:" << level << " MaxLevel:" << MAX_LEVEL << "\n";
   for (int i=level; i>=0; i--)
	   print(i);
}
*/

} // namespace mlnet

#endif /* MLNET_SORTED_SET_H_ */
