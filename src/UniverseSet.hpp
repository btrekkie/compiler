#ifndef UNIVERSE_SET_HPP_INCLUDED
#define UNIVERSE_SET_HPP_INCLUDED

#include <algorithm>
#include <assert.h>
#include "Universe.hpp"

/**
 * A set class optimized for storing sets that are dense in some universe of
 * values.  In particular, the "intersect", "unionWith", and "difference"
 * operations should be fast with such sets.  You can think of this class as a
 * bit vector, although such an implementation is not guaranteed.  For example,
 * this class might attempt special optimizations with nearly empty or nearly
 * full sets.
 * 
 * There is no need to specify the members of the universe in advance.  The
 * universe is automatically determined on the fly based on the calls to "add",
 * "remove", and "contains".
 */
/* UniverseSet is presently implemented as a bit vector.
 */
template<class T>
class UniverseSet {
private:
    /**
     * The universe of possible elements of the set.
     */
    Universe<T>* universe;
    /**
     * A bit vector indicating the elements of the set.  The value with index i
     * is present in the set iff i < 8 * numBytes and the (i % 8)th least
     * significant bit is set in bytes[i / 8].  The "bytes" array may need to be
     * periodically reallocated due to additions to the universe.
     */
    char* bytes;
    /**
     * The number of elements in "bytes".
     */
    int numBytes;
    
    /**
     * Increases the size of "bytes" so that it has at least "numBytes"
     * elements.  This method does not alter the set indicated by "bytes"; in
     * other words, it copies the old elements into the new array and sets the
     * new elements to 0.  Assumes that minBytes > numBytes.
     */
    void increaseSize(int minBytes) {
        int newNumBytes = max(minBytes, (universe->getSize() + 7) / 8);
        char* newBytes = new char[newNumBytes];
        memcpy(newBytes, bytes, numBytes);
        memset(newBytes + numBytes, 0, newNumBytes - numBytes);
        delete[] bytes;
        bytes = newBytes;
        numBytes = newNumBytes;
    }
public:
    /**
     * Returns a new UniverseSet whose elements are in the specified universe.
     */
    explicit UniverseSet(Universe<T>* universe2) {
        universe = universe2;
        numBytes = max((universe->getSize() + 7) / 8, 1);
        bytes = new char[numBytes];
        memset(bytes, 0, numBytes);
    }
    
    UniverseSet(UniverseSet<T>& other) {
        universe = other.universe;
        numBytes = max((universe->getSize() + 7) / 8, 1);
        bytes = new char[numBytes];
        if (numBytes <= other.numBytes)
            memcpy(bytes, other.bytes, numBytes);
        else {
            memcpy(bytes, other.bytes, other.numBytes);
            memset(bytes + other.numBytes, 0, numBytes - other.numBytes);
        }
    }
    
    ~UniverseSet() {
        delete[] bytes;
    }
    
    /**
     * Adds the specified value to this set if it is not already present.
     */
    void add(T value) {
        int index = universe->getIndex(value);
        if (index >= 8 * numBytes)
            increaseSize(2 * numBytes);
        bytes[index / 8] |= (1 << (index % 8));
    }
    
    /**
     * Removes the specified value from this set if it is present.
     */
    void remove(T value) {
        int index = universe->getIndex(value);
        if (index < 8 * numBytes)
            bytes[index / 8] &= ~(1 << (index % 8));
    }
    
    /**
     * Returns whether the specified element is in this set.
     */
    bool contains(T value) {
        int index = universe->getIndex(value);
        return index < 8 * numBytes &&
            (bytes[index / 8] & (1 << (index % 8))) != 0;
    }
    
    /**
     * Alters this set so that it contains the elements that are in both this
     * and "set".  Assumes that this has the same universe as "set".
     */
    void intersect(UniverseSet<T>* set) {
        assert(
            universe == set->universe ||
            !L"Cannot intersect sets with different universes");
        for (int i = 0; i < min(numBytes, set->numBytes); i++)
            bytes[i] &= set->bytes[i];
    }
    
    /**
     * Alters this set so that it contains the elements that are in either this
     * or "set".  Assumes that this has the same universe as "set".
     */
    void unionWith(UniverseSet<T>* set) {
        assert(
            universe == set->universe ||
            !L"Cannot take the union of sets with different universes");
        if (numBytes < set->numBytes)
            increaseSize(set->numBytes);
        for (int i = 0; i < set->numBytes; i++)
            bytes[i] |= set->bytes[i];
    }
    
    /**
     * Alters this set so that it contains the elements that are in either this,
     * but not "set".  Assumes that this has the same universe as "set".
     */
    void difference(UniverseSet<T>* set) {
        assert(
            universe == set->universe ||
            !L"Cannot take the difference of sets with different universes");
        for (int i = 0; i < min(numBytes, set->numBytes); i++)
            bytes[i] &= ~(set->bytes[i]);
    }
};

#endif
