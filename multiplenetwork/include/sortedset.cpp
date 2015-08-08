template <class T>
Entry<T>::Entry(int level, object_id id, T obj_ptr) {
        forward.resize(level+1);
        link_length.resize(level+1);
        this->id = id;
        this->obj_ptr = obj_ptr;
    }

template <class T>
SortedSet<T>::SortedSet() {
        header = new Entry<T>(MAX_LEVEL, 0, NULL);
        level = 0;
    }


template <class T>
typename SortedSet<T>::iterator SortedSet<T>::begin() const {
	return iterator(header->forward[0]);
}

template <class T>
typename SortedSet<T>::iterator SortedSet<T>::end() const {
	return iterator(NULL);
}

template <class T>
T SortedSet<T>::iterator::operator*() {
	return current->obj_ptr;
}

template <class T>
SortedSet<T>::iterator::iterator(Entry<T>* iter) : current(iter) {
}

template <class T>
typename SortedSet<T>::iterator SortedSet<T>::iterator::operator++() { // PREFIX
	current=current->forward[0];
	return *this;
}

template <class T>
typename SortedSet<T>::iterator SortedSet<T>::iterator::operator++(int) { // POSTFIX
	SortedSet<T>::iterator tmp(current);
	current=current->forward[0];
	return tmp;
}

template <class T>
bool SortedSet<T>::iterator::operator==(const SortedSet<T>::iterator& rhs) {
	return current == rhs.current;
}

template <class T>
bool SortedSet<T>::iterator::operator!=(const SortedSet<T>::iterator& rhs) {
	return current != rhs.current;
}

template <class T>
long SortedSet<T>::size() const {
	return num_entries;
}

template <class T>
bool SortedSet<T>::contains(object_id search_value) const {
    const Entry<T> *x = header;
    for (int i = level; i >= 0; i--) {
        while (x->forward[i] != NULL && x->forward[i]->value < search_value) {
            x = x->forward[i];
        }
    }
    x = x->forward[0];
    return x != NULL && x->value == search_value;
}

template <class T>
T SortedSet<T>::get(object_id search_value) const {
    const Entry<T> *x = header;
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

template <class T>
T SortedSet<T>::get_at_index(long pos) const {
	if (pos < 0 || pos >= num_entries)
		throw ElementNotFoundException("ObjectStore: out of bounds");
    const Entry<T> *x = header;
    long so_far=0;
    for (int i = level; i >= 0; i--) {
        while (x->forward[i] != NULL && x->link_length[i] + so_far <= pos + 1) {
        	so_far+= x->link_length[i];
            x = x->forward[i];
        }
    }
    return x->obj_ptr;
}

// // only for debugging
//template <class T>
//void ObjectStore<T>::print(int lev) {
//    const Entry<T> *x = header;
//    while (x!=NULL) {
//    	std::cout << x->value << "(" << x->link_length[lev] << ") ";
//    	for (int i=0; i<x->link_length[lev]-1; i++) std::cout << "     ";
//        x = x->forward[lev];
//    }
//    std::cout << "X\n";
//}

template <class T>
void SortedSet<T>::insert(object_id value, T obj_ptr) {
    Entry<T> *x = header;
    std::vector<Entry<T>*> update;
    update.resize(MAX_LEVEL+1);
    std::vector<int> skipped_positions_per_level;
    int skipped_positions = 0;
    skipped_positions_per_level.resize(MAX_LEVEL+1,0);


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
        int lvl = random_utils::random_level(MAX_LEVEL,P);

        if (lvl > level) {
        	for (int i = level + 1; i <= lvl; i++) {
        		update[i] = header;
        		update[i]->link_length[i] = num_entries;
        	}
        	level = lvl;
        }
        x = new Entry<T>(lvl, value, obj_ptr);
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
    }
    else {
    	x->obj_ptr = obj_ptr;
    }
}

template <class T>
void SortedSet<T>::erase(object_id value) {
	Entry<T> *x = header;
	std::vector<Entry<T>*> update;
	update.resize(MAX_LEVEL+1);

	for (int i = level; i >= 0; i--) {
		while (x->forward[i] != NULL && x->forward[i]->id < value) {
			x = x->forward[i];
		}
		update[i] = x;
	}
	x = x->forward[0];

	if (x== NULL) return;

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
	}
}
