#ifndef TEST_CASE_HPP_INCLUDED
#define TEST_CASE_HPP_INCLUDED

#include <sstream>
#include <string>

class TestRunner;

class TestCase {
private:
    TestRunner* runner;
    
    void assertionPassed();
    void assertionFailed(std::string message);
protected:
    void assertTrue(bool value, std::string message);
    void assertFalse(bool value, std::string message);
    void assertNull(void* value, std::string message);
    void assertNotNull(void* value, std::string message);
    
    template<class T>
    void assertEqual(T expected, T actual, std::string message) {
        if (expected == actual)
            assertionPassed();
        else {
            std::ostringstream fullMessage;
            fullMessage << "Expected " << expected << ", but got " << actual <<
                ".\n" << message;
            assertionFailed(fullMessage.str());
        }
    }
public:
    virtual std::string getName() = 0;
    virtual void test() = 0;
    void setTestRunner(TestRunner* runner2);
};

#endif
