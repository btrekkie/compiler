#ifndef UNIVERSE_HPP_INCLUDED
#define UNIVERSE_HPP_INCLUDED

#include <map>

template<class T>
class UniverseSet;

/**
 * Represents the universe of a UniverseSet.  See the comments for the
 * UniverseSet class for more information.
 */
template<class T>
class Universe {
private:
    friend class UniverseSet<T>;
    
    /**
     * A map from the elements of the universe we have encountered thus far to
     * integers used to indicate their bit positions in the bit vectors used by
     * the UniverseSet class.  As a new entry is added, its index is
     * indices.count(), where 0 is the index of the first entry.
     */
    std::map<T, int> indices;
    
    /**
     * Returns an index indicating the bit position of the specified value in
     * the bit vectors used by the UniverseSet class.
     */
    int getIndex(T value) {
        if (indices.count(value) == 0)
            indices[value] = (int)indices.size();
        return indices[value];
    }
    
    /**
     * Returns the number of elements of the universe we have encountered thus
     * far.
     */
    int getSize() {
        return (int)indices.size();
    }
};

#endif
