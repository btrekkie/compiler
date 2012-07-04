#include "TestCase.hpp"
#include "TestRunner.hpp"

using namespace std;

void TestCase::assertionPassed() {
    runner->assertionPassed();
}

void TestCase::assertionFailed(string message) {
    runner->assertionFailed(message);
}

void TestCase::assertTrue(bool value, string message) {
    if (value)
        runner->assertionPassed();
    else
        runner->assertionFailed(message);
}

void TestCase::assertFalse(bool value, string message) {
    if (!value)
        runner->assertionPassed();
    else
        runner->assertionFailed(message);
}

void TestCase::assertNull(void* value, string message) {
    if (value == NULL)
        runner->assertionPassed();
    else
        runner->assertionFailed(message);
}

void TestCase::assertNotNull(void* value, string message) {
    if (value != NULL)
        runner->assertionPassed();
    else
        runner->assertionFailed(message);
}

void TestCase::setTestRunner(TestRunner* runner2) {
    runner = runner2;
}
