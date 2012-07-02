#include <exception>
#include <iostream>
#include "TestCase.hpp"
#include "TestRunner.hpp"

using namespace std;

void TestRunner::runTestCases(vector<TestCase*> testCases) {
    numAssertions = 0;
    int numTestCasesPassed = 0;
    for (vector<TestCase*>::const_iterator iterator = testCases.begin();
         iterator != testCases.end();
         iterator++) {
        try {
            (*iterator)->setTestRunner(this);
            (*iterator)->test();
            numTestCasesPassed++;
        } catch (string message) {
            cerr << "Assertion failed in " << (*iterator)->getName() << ":\n" <<
                message;
        }
    }
    cout << "Made " << numAssertions << " assertion(s).\n";
    cout << "Passed " << numTestCasesPassed << " / " << testCases.size() <<
        " test case(s).\n";
    if (numTestCasesPassed == (int)testCases.size())
        cout << "Success!\n";
}

void TestRunner::assertionPassed() {
    numAssertions++;
}

void TestRunner::assertionFailed(string message) {
    numAssertions++;
    throw message;
}
