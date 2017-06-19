/**
 * sortedrandomset.h
 *
 * A sorted random map is a class used to store a set of objects that can be accessed:
 * 1. by key in (average) log time.
 * 2. by index (position) in constant time.
 * 3. by const iterating over its elements.
 * When several components of a multilayer network need to be accessed,
 * e.g., the neighbors of a node, a const reference to the corresponding
 * sorted random map is typically returned so that no objects are duplicated and
 * no additional memory is used.
 *
 * Here a sorted random map is implemented as a skip list. This is (linearly) less efficient
 * than a map, but the map from the C++ standard library cannot be modified, so I could not
 * add the random element retrieval function. A skip list was much faster to implement from
 * scratch than a red-black tree.
 *
 * NOTE: The values stored in the set must be (smart) pointers
 */

#ifndef MLNET_SORTED_SET_H_
#define MLNET_SORTED_SET_H_

#include "random.h"
#include <string>
#include <map>
#include <unordered_map>
#include <set>
#include <vector>
#include <memory>
#include <cmath>

namespace mlnet {

    template <class ELEMENT_TYPE> class sorted_random_set;
    template <class ELEMENT_TYPE> class sorted_random_set_entry;
    
    template <class ELEMENT_TYPE> using SortedRandomSetEntrySharedPtr = std::shared_ptr<sorted_random_set_entry<ELEMENT_TYPE> >;
    template <class ELEMENT_TYPE> using constSortedRandomSetEntrySharedPtr = std::shared_ptr<const sorted_random_set_entry<ELEMENT_TYPE> >;
    
/**
 * An entry in a sorted set, which is implemented as a skip list.
 */
template <class ELEMENT_TYPE>
class sorted_random_set_entry {
	friend class sorted_random_set<ELEMENT_TYPE>;

private:
	/** The object corresponding to this entry */
	ELEMENT_TYPE obj_ptr;
	/** An array of pointers to the next entry, for each level in the skip list */
    std::vector<SortedRandomSetEntrySharedPtr<ELEMENT_TYPE> > forward; // array of pointers
    /** The number of entries before the next entry on each level, used for positional access */
    std::vector<int> link_length;

public:
    /**
     * Constructor.
     * @param level height of the entry in the skip list
     * @param obj the object corresponding to this entry
     */
    sorted_random_set_entry(int level, ELEMENT_TYPE obj);

    /**
     * This function is used to increase the level of the header.
     * @param skipped_entries number of skipped entries (to be set to the number of entries)
     */
    void increment(long skipped_entries);
};

/**
 * A sorted map is a class used to store a set of components that:
 * 1. can be accessed by id in log time.
 * 2. can be accessed by index (position) in the set in constant time.
 * 3. can be accessed using a const iterator, so that it is not necessary to duplicate them. As an example,
 * it is not necessary to duplicate nodes when the neighbors of a node are accessed: a sorted set on those
 * nodes is returned instead.
 *
 * A sorted map is implemented as a skip list.
 */
template <class ELEMENT_TYPE>
class sorted_random_set {
private:
	float P = 0.5;

    SortedRandomSetEntrySharedPtr<ELEMENT_TYPE> header;
    /* Number of entries for which the sorted set is optimized. */
    size_t capacity = 1;
    /* Current number of entries. */
    size_t num_entries = 0;
    /* Maximum level */
	int MAX_LEVEL = 0;
    /* Current maximum level in use. */
    int level;

public:
	/**
	 * Creates a sorted set.
	 */
    sorted_random_set();
    
    
    /**
     * Destructs a sorted set, making sure that this does not result in a long sequence of nested calls to entry destructors that would fill in the stack
     */
    ~sorted_random_set() {
        std::vector<SortedRandomSetEntrySharedPtr<ELEMENT_TYPE> > tmp(size());
        SortedRandomSetEntrySharedPtr<ELEMENT_TYPE> current = header;
        SortedRandomSetEntrySharedPtr<ELEMENT_TYPE> next = current->forward.at(0);
        tmp.push_back(current);
        while (next) {
            tmp.push_back(next);
            for (size_t i=0; i<current->forward.size(); i++)
                current->forward[i] = nullptr;
            current = next;
            next = current->forward.at(0);
        }
    }
    
	/**
	 * Creates a sorted set optimized to store a pre-defined number of entries
	 * @param start_capacity the initial capacity for which the sorted set is optimized.
	 */
    sorted_random_set(size_t start_capacity);

    /** Iterator over the objects in this collection */
	class iterator {
	    typedef std::forward_iterator_tag iterator_category;
		public:
		iterator();
		/** Returns an iterator pointing at the input object */
		iterator(SortedRandomSetEntrySharedPtr<ELEMENT_TYPE>);
		/** Return the object pointed by this iterator */
		ELEMENT_TYPE operator*();
		/** Moves the iterator to the next object in the collection (prefix) */
		iterator operator++();
		/** Moves the iterator to the next object in the collection (postfix) */
		iterator operator++(int);
		/** Checks if this iterator equals the input one */
	    bool operator==(const sorted_random_set<ELEMENT_TYPE>::iterator& rhs);
		/** Checks if this iterator differs from the input one */
	    bool operator!=(const sorted_random_set<ELEMENT_TYPE>::iterator& rhs);
		private:
		/** Entry currently pointed to by this iterator */
		SortedRandomSetEntrySharedPtr<ELEMENT_TYPE> current;
	};
	/** Returns an iterator to the first object in the collection */
	sorted_random_set<ELEMENT_TYPE>::iterator begin() const;
	/** Returns an iterator after the last object in the collection */
	sorted_random_set<ELEMENT_TYPE>::iterator end() const;
	/** Returns the number of objects in the collection */
    size_t size() const;
	/** Returns true if an object with the input id is present in the collection */
    bool contains(ELEMENT_TYPE) const;
	/** Returns the position of the input value in the collection, or -1 */
    size_t get_index(ELEMENT_TYPE) const;
	/** Returns the object at the given position in the collection, or nullptr */
    ELEMENT_TYPE get_at_index(size_t) const;
	/** Returns a random object, uniform probability */
    ELEMENT_TYPE get_at_random() const;
	/**
	 * Inserts a new object in the collection.
	 * @return true if KEY was not already present, false otherwise (in which case the object is updated with the new value)
	 * */
    bool insert(ELEMENT_TYPE);
	/**
	 * Removes the input object from the collection.
	 * @return true if the object is removed from the collection, false if the object was not present.
	 */
    bool erase(ELEMENT_TYPE);
    //void print(int level); // only for debugging
    //void print(); // only for debugging
};

/* TEMPLATE CODE */

template <class ELEMENT_TYPE>
sorted_random_set_entry<ELEMENT_TYPE>::sorted_random_set_entry(int level, ELEMENT_TYPE obj_ptr) {
    forward.resize(level+1);
    link_length.resize(level+1);
    this->obj_ptr = obj_ptr;
}

template <class ELEMENT_TYPE>
void sorted_random_set_entry<ELEMENT_TYPE>::increment(long skipped_entries) {
	int current_size = forward.size();
	forward.resize(current_size+1,nullptr);
	link_length.resize(current_size+1,skipped_entries);
}

template <class ELEMENT_TYPE>
sorted_random_set<ELEMENT_TYPE>::sorted_random_set() {
   	header = std::make_shared<sorted_random_set_entry<ELEMENT_TYPE> >(MAX_LEVEL, nullptr);
    	level = 0;
}

template <class ELEMENT_TYPE>
sorted_random_set<ELEMENT_TYPE>::sorted_random_set(size_t start_capacity) {
	capacity = start_capacity;
	MAX_LEVEL = std::ceil(std::log2(capacity));
	header = std::make_shared<sorted_random_set_entry<ELEMENT_TYPE> >(MAX_LEVEL, nullptr);
    	level = 0;
}

template <class ELEMENT_TYPE>
typename sorted_random_set<ELEMENT_TYPE>::iterator sorted_random_set<ELEMENT_TYPE>::begin() const {
	return iterator(header->forward[0]);
}

template <class ELEMENT_TYPE>
typename sorted_random_set<ELEMENT_TYPE>::iterator sorted_random_set<ELEMENT_TYPE>::end() const {
	return iterator(nullptr);
}

template <class ELEMENT_TYPE>
ELEMENT_TYPE sorted_random_set<ELEMENT_TYPE>::iterator::operator*() {
	return current->obj_ptr;
}

template <class ELEMENT_TYPE>
sorted_random_set<ELEMENT_TYPE>::iterator::iterator(SortedRandomSetEntrySharedPtr<ELEMENT_TYPE> iter) : current(iter) {
}

template <class ELEMENT_TYPE>
typename sorted_random_set<ELEMENT_TYPE>::iterator sorted_random_set<ELEMENT_TYPE>::iterator::operator++() { // PREFIX
	current=current->forward[0];
	return *this;
}

template <class ELEMENT_TYPE>
typename sorted_random_set<ELEMENT_TYPE>::iterator sorted_random_set<ELEMENT_TYPE>::iterator::operator++(int) { // POSTFIX
	sorted_random_set<ELEMENT_TYPE>::iterator tmp(current);
	current=current->forward[0];
	return tmp;
}

template <class ELEMENT_TYPE>
bool sorted_random_set<ELEMENT_TYPE>::iterator::operator==(const sorted_random_set<ELEMENT_TYPE>::iterator& rhs) {
	return current == rhs.current;
}

template <class ELEMENT_TYPE>
bool sorted_random_set<ELEMENT_TYPE>::iterator::operator!=(const sorted_random_set<ELEMENT_TYPE>::iterator& rhs) {
	return current != rhs.current;
}

template <class ELEMENT_TYPE>
size_t sorted_random_set<ELEMENT_TYPE>::size() const {
	return num_entries;
}

template <class ELEMENT_TYPE>
bool sorted_random_set<ELEMENT_TYPE>::contains(ELEMENT_TYPE search_value) const {
    constSortedRandomSetEntrySharedPtr<ELEMENT_TYPE> x = header;
    for (int i = level; i >= 0; i--) {
        while (x->forward[i] != nullptr && x->forward[i]->obj_ptr < search_value) {
            x = x->forward[i];
        }
    }
    x = x->forward[0];
    return x != nullptr && x->obj_ptr == search_value;
}

template <class ELEMENT_TYPE>
size_t sorted_random_set<ELEMENT_TYPE>::get_index(ELEMENT_TYPE search_value) const {
    constSortedRandomSetEntrySharedPtr<ELEMENT_TYPE> x = header;
    long so_far=0;
    for (int i = level; i >= 0; i--) {
        while (x->forward[i] != nullptr && x->forward[i]->obj_ptr < search_value) {
        	so_far+= x->link_length[i];
            x = x->forward[i];
        }
    }
	so_far+= x->link_length[0];
    x = x->forward[0];
    if (x != nullptr && x->obj_ptr == search_value)
    	return so_far-1;
    else return -1;
}

template <class ELEMENT_TYPE>
ELEMENT_TYPE sorted_random_set<ELEMENT_TYPE>::get_at_index(size_t pos) const {
	if (pos < 0 || pos >= num_entries)
		throw ElementNotFoundException("Index out of bounds");
    constSortedRandomSetEntrySharedPtr<ELEMENT_TYPE> x = header;
    size_t so_far=0;
    for (int i = level; i >= 0; i--) {
        while (x->forward[i] != nullptr && x->link_length[i] + so_far <= pos + 1) {
        	so_far+= x->link_length[i];
            x = x->forward[i];
        }
    }
    return x->obj_ptr;
}

template <class ELEMENT_TYPE>
ELEMENT_TYPE sorted_random_set<ELEMENT_TYPE>::get_at_random() const {
	return get_at_index(getRandomInt(size()));
}

template <class ELEMENT_TYPE>
bool sorted_random_set<ELEMENT_TYPE>::insert(ELEMENT_TYPE value) {
	SortedRandomSetEntrySharedPtr<ELEMENT_TYPE> x = header;
    std::vector<SortedRandomSetEntrySharedPtr<ELEMENT_TYPE> > update;
    update.resize(level+1);
    std::vector<int> skipped_positions_per_level;
    int skipped_positions = 0;
    skipped_positions_per_level.resize(level+1,0);

    for (int i = level; i >= 0; i--) {
    	skipped_positions_per_level[i] = skipped_positions;
        while (x->forward[i] != nullptr && x->forward[i]->obj_ptr < value) {
        	skipped_positions_per_level[i] += x->link_length[i];
        	skipped_positions += x->link_length[i];
            x = x->forward[i];
        }
        update[i] = x;
    }
    x = x->forward[0];


    if (x == nullptr || x->obj_ptr != value) {
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

        x = std::make_shared<sorted_random_set_entry<ELEMENT_TYPE> >(lvl, value);

        for (int i = 0; i <= lvl; i++) {
        	int offset = skipped_positions-skipped_positions_per_level[i];

        	x->forward[i] = update[i]->forward[i];
        	if (update[i]->forward[i]==nullptr)
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
    	x->obj_ptr = value;
    	return false;
    }
}

template <class ELEMENT_TYPE>
bool sorted_random_set<ELEMENT_TYPE>::erase(ELEMENT_TYPE value) {
	SortedRandomSetEntrySharedPtr<ELEMENT_TYPE> x = header;
	std::vector<SortedRandomSetEntrySharedPtr<ELEMENT_TYPE> > update;
	update.resize(MAX_LEVEL+1);

	for (int i = level; i >= 0; i--) {
		while (x->forward[i] != nullptr && x->forward[i]->obj_ptr < value) {
			x = x->forward[i];
		}
		update[i] = x;
	}
	x = x->forward[0];

	if (x == nullptr) return false;

	if (x->obj_ptr == value) {
		for (int i = 0; i <= level; i++) {
			if (update[i]->forward[i] != x) {
				update[i]->link_length[i]--;
			}
			else {
				update[i]->forward[i] = x->forward[i];
				update[i]->link_length[i] += x->link_length[i]-1;
			}
		}
		//delete x;
		num_entries--;
		while (level > 0 && header->forward[level] == nullptr) {
			level--;
		}
		return true;
	}
	else return false;
}


/*
// only for debugging
template <class ELEMENT_TYPE>
void sorted_set<ELEMENT_TYPE>::print(int lev) {
    const Entry<ELEMENT_TYPE> *x = header;
    int num = 0;
    double std = 0;
    while (x!=nullptr) {
    	std += x->link_length[lev]*x->link_length[lev];
    	num++;
        x = x->forward[lev];
    }
    std::cout << "l" << lev << " e: " << num << " std: " << std/num << "\n";
}

template <class ELEMENT_TYPE>
void sorted_set<ELEMENT_TYPE>::print() {
std::cout << "cap:" << capacity << " num:" << num_entries << " lev:" << level << " MaxLevel:" << MAX_LEVEL << "\n";
   for (int i=level; i>=0; i--)
	   print(i);
}
*/

} // namespace mlnet

#endif /* MLNET_SORTED_SET_H_ */
