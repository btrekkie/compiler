#include <exception>
#include <iostream>
#include "TestCase.hpp"
#include "TestRunner.hpp"

using namespace std;

void TestRunner::runTestCases(vector<TestCase*> testCases) {
    int numTestCasesPassed = 0;
    for (vector<TestCase*>::const_iterator iterator = testCases.begin();
         iterator != testCases.end();
         iterator++) {
        try {
            (*iterator)->test();
            numTestCasesPassed++;
        } catch (string message) {
            cerr << "Assertion failed in " << (*iterator)->getName() << ":\n" <<
                message;
        }
    }
    if (numTestCasesPassed == (int)testCases.size())
        cout << "Success!  Passed all " << numTestCasesPassed <<
            " test cases.\n";
    else
        cout << "Passed " << numTestCasesPassed << " / " << testCases.size() <<
            " test cases\n";
}

void TestRunner::assertionPassed() {
    
}

void TestRunner::assertionFailed(string message) {
    throw message;
}
