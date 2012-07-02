#include <exception>
#include <iostream>
#include "TestCase.hpp"
#include "TestRunner.hpp"

using namespace std;

void TestRunner::runTestCases(vector<TestCase*> testCases) {
    cout << "Running " << (int)testCases.size() << " test case(s)\n";
    numAssertions = 0;
    int numTestCasesPassed = 0;
    for (vector<TestCase*>::const_iterator iterator = testCases.begin();
         iterator != testCases.end();
         iterator++) {
        try {
            (*iterator)->setTestRunner(this);
            (*iterator)->test();
            numTestCasesPassed++;
            cout << "Passed " << (*iterator)->getName() << "\n";
        } catch (string message) {
            cerr << "Assertion failed in " << (*iterator)->getName() << ":\n" <<
                message << '\n';
        }
    }
    cout << "Made " << numAssertions << " assertion(s)\n";
    cout << "Passed " << numTestCasesPassed << " / " << testCases.size() <<
        " test case(s)\n";
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
