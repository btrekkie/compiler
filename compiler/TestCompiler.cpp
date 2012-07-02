/* Provides main function for running all of the unit tests.
 */

#include <vector>
#include "test/JSONTest.hpp"
#include "test/TestCase.hpp"
#include "test/TestRunner.hpp"

using namespace std;

int main() {
    vector<TestCase*> testCases;
    testCases.push_back(new JSONTest());
    
    TestRunner testRunner;
    testRunner.runTestCases(testCases);
    for (vector<TestCase*>::const_iterator iterator = testCases.begin();
         iterator != testCases.end();
         iterator++)
        delete *iterator;
    return 0;
}
