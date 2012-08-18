#include <exception>
#include <iostream>
#include "TestCase.hpp"
#include "TestRunner.hpp"

using namespace std;

void TestRunner::runTestCases(vector<TestCase*> testCases) {
    wcout << L"Running " << (int)testCases.size() << L" test case(s)\n";
    numAssertions = 0;
    int numTestCasesPassed = 0;
    for (vector<TestCase*>::const_iterator iterator = testCases.begin();
         iterator != testCases.end();
         iterator++) {
        try {
            (*iterator)->setTestRunner(this);
            (*iterator)->test();
            numTestCasesPassed++;
            wcout << L"Passed " << (*iterator)->getName() << L"\n";
        } catch (wstring message) {
            wcerr << L"Assertion failed in " << (*iterator)->getName() <<
                L":\n" << message << L'\n';
        }
    }
    wcout << L"Made " << numAssertions << L" assertion(s)\n";
    wcout << L"Passed " << numTestCasesPassed << L" / " << testCases.size() <<
        L" test case(s)\n";
    if (numTestCasesPassed == (int)testCases.size())
        wcout << L"Success!\n";
}

void TestRunner::assertionPassed() {
    numAssertions++;
}

void TestRunner::assertionFailed(wstring message) {
    numAssertions++;
    throw message;
}
