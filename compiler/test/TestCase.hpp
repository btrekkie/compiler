#ifndef TEST_CASE_HPP_INCLUDED
#define TEST_CASE_HPP_INCLUDED

#include <sstream>
#include <string>

class TestRunner;

/**
 * Abstract base class for unit tests.  A unit test executes a series of
 * "assertions", which must all succeed in order for the unit test to pass.
 */
class TestCase {
private:
    /**
     * The TestRunner that is executing this unit test.
     */
    TestRunner* runner;
    /**
     * Indicates a successful assertion.
     */
    void assertionPassed();
    /**
     * Indicates an unsuccessful assertion.
     * @param message a string describing the failure.
     */
    void assertionFailed(std::string message);
protected:
    /**
     * Asserts that the specified value is true.
     * @param value the value to check.
     * @param message if "value" is false, a string describing the assertion
     *     failure.
     */
    void assertTrue(bool value, std::string message);
    /**
     * Asserts that the specified value is false.
     * @param value the value to check.
     * @param message if "value" is true, a string describing the assertion
     *     failure.
     */
    void assertFalse(bool value, std::string message);
    /**
     * Asserts that the specified value is NULL.
     * @param value the value to check.
     * @param message if "value" is non-null, a string describing the assertion
     *     failure.
     */
    void assertNull(void* value, std::string message);
    /**
     * Asserts that the specified value is something other than NULL.
     * @param value the value to check.
     * @param message if "value" is NULL, a string describing the assertion
     *     failure.
     */
    void assertNotNull(void* value, std::string message);
    
    /**
     * Asserts that the specified values are equal, when compared using the ==
     * operator.
     * @param expected the expected value of "actual".
     * @param actual the value to check.
     * @param message if "actual" differs from "expected", a string describing
     *     the assertion failure.
     */
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
    /**
     * Returns a human-readable string identifying this unit test.  The
     * recommended return value of this method is the class's name.
     */
    virtual std::string getName() = 0;
    /**
     * Called by TestRunner to execute all of the test assertions.
     */
    virtual void test() = 0;
    /**
     * Called by TestRunner to indicate the TestRunner that is executing this
     * unit test.
     */
    void setTestRunner(TestRunner* runner2);
};

#endif