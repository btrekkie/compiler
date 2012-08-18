#include "../Universe.hpp"
#include "../UniverseSet.hpp"
#include "UniverseSetTest.hpp"

using namespace std;

/**
 * The maximum value stored in a UniverseSet created for testing.
 */
static int MAX_TEST_VALUE = 200;

/**
 * Returns a UniverseSet consisting of the multiples of "value" from 0 to
 * MAX_TEST_VALUE.
 * @param universe the set's universe of values.
 * @param value the value whose multiples we are taking.
 * @return the UniverseSet.
 */
static UniverseSet<int>* getMultiples(Universe<int>* universe, int value) {
    UniverseSet<int>* set = new UniverseSet<int>(universe);
    for (int i = 0; i <= MAX_TEST_VALUE; i += value)
        set->add(i);
    return set;
}

wstring UniverseSetTest::getName() {
    return L"UniverseSetTest";
}

void UniverseSetTest::test() {
    Universe<int> universe1;
    UniverseSet<int>* emptySet = new UniverseSet<int>(&universe1);
    UniverseSet<int>* smallSet = getMultiples(&universe1, MAX_TEST_VALUE);
    UniverseSet<int>* multiplesOf2 = getMultiples(&universe1, 2);
    UniverseSet<int>* multiplesOf3 = getMultiples(&universe1, 3);
    UniverseSet<int>* multiplesOf5 = getMultiples(&universe1, 5);
    UniverseSet<int>* testSet = new UniverseSet<int>(*emptySet);
    bool areSetsCorrect = true;
    for (int i = 0; i <= MAX_TEST_VALUE; i++) {
        if (emptySet->contains(i) ||
            testSet->contains(i) ||
            smallSet->contains(i) != (i % MAX_TEST_VALUE == 0) ||
            multiplesOf2->contains(i) != (i % 2 == 0) ||
            multiplesOf3->contains(i) != (i % 3 == 0) ||
            multiplesOf5->contains(i) != (i % 5 == 0)) {
            areSetsCorrect = false;
            break;
        }
    }
    assertTrue(areSetsCorrect, L"Contains failed");
    
    testSet->unionWith(multiplesOf3);
    for (int i = 0; i <= MAX_TEST_VALUE; i++) {
        if (testSet->contains(i) != (i % 3 == 0) ||
            multiplesOf3->contains(i) != (i % 3 == 0)) {
            areSetsCorrect = false;
            break;
        }
    }
    assertTrue(areSetsCorrect, L"unionWith failed");
    
    testSet->intersect(multiplesOf5);
    for (int i = 0; i <= MAX_TEST_VALUE; i++) {
        if (testSet->contains(i) != (i % 3 == 0 && i % 5 == 0) ||
            multiplesOf5->contains(i) != (i % 5 == 0)) {
            areSetsCorrect = false;
            break;
        }
    }
    assertTrue(areSetsCorrect, L"intersect failed");
    
    testSet->difference(multiplesOf2);
    for (int i = 0; i <= MAX_TEST_VALUE; i++) {
        if (testSet->contains(i) != (i % 3 == 0 && i % 5 == 0 && i % 2 != 0) ||
            multiplesOf2->contains(i) != (i % 2 == 0)) {
            areSetsCorrect = false;
            break;
        }
    }
    assertTrue(areSetsCorrect, L"difference failed");
    
    testSet->unionWith(multiplesOf2);
    for (int i = 0; i <= MAX_TEST_VALUE; i++) {
        if (testSet->contains(i) !=
              ((i % 3 == 0 && i % 5 == 0) || i % 2 == 0) ||
            multiplesOf2->contains(i) != (i % 2 == 0)) {
            areSetsCorrect = false;
            break;
        }
    }
    assertTrue(areSetsCorrect, L"unionWith failed");
    
    testSet->remove(24);
    testSet->add(115);
    for (int i = 0; i <= MAX_TEST_VALUE; i++) {
        if (testSet->contains(i) !=
              ((((i % 3 == 0 && i % 5 == 0) || i % 2 == 0) && i != 24) ||
               i == 115)) {
            areSetsCorrect = false;
            break;
        }
    }
    assertTrue(areSetsCorrect, L"contains failed");
    
    // Test different-sized bit vectors
    Universe<int> universe2;
    UniverseSet<int>* testSet2 = getMultiples(&universe2, 6);
    UniverseSet<int>* multiplesOf4 = new UniverseSet<int>(&universe2);
    for (int i = 0; i <= MAX_TEST_VALUE / 4; i += 4)
        multiplesOf4->add(i);
    for (int i = 0; i <= MAX_TEST_VALUE; i++) {
        if (testSet2->contains(i) != (i % 6 == 0) ||
            multiplesOf4->contains(i) !=
                (i <= MAX_TEST_VALUE / 4 && i % 4 == 0)) {
            areSetsCorrect = false;
            break;
        }
    }
    assertTrue(areSetsCorrect, L"Contains failed");
    
    testSet2->unionWith(multiplesOf4);
    for (int i = 0; i <= MAX_TEST_VALUE; i++) {
        if (testSet2->contains(i) !=
                (i % 6 == 0 || (i <= MAX_TEST_VALUE / 4 && i % 4 == 0)) ||
            multiplesOf4->contains(i) !=
                (i <= MAX_TEST_VALUE / 4 && i % 4 == 0)) {
            areSetsCorrect = false;
            break;
        }
    }
    assertTrue(areSetsCorrect, L"unionWith failed");
    
    delete emptySet;
    delete multiplesOf2;
    delete multiplesOf3;
    delete multiplesOf4;
    delete multiplesOf5;
    delete smallSet;
    delete testSet;
    delete testSet2;
}
