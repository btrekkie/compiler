#ifndef TEST_RUNNER_HPP_INCLUDED
#define TEST_RUNNER_HPP_INCLUDED

#include <string>
#include <vector>

class TestCase;

class TestRunner {
public:
    void runTestCases(std::vector<TestCase*> testCases);
    void assertionPassed();
    void assertionFailed(std::string message);
};

#endif
