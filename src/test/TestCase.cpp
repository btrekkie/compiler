#include "TestCase.hpp"
#include "TestRunner.hpp"

using namespace std;

void TestCase::assertionPassed() {
    runner->assertionPassed();
}

void TestCase::assertionFailed(wstring message) {
    runner->assertionFailed(message);
}

void TestCase::assertTrue(bool value, wstring message) {
    if (value)
        runner->assertionPassed();
    else
        runner->assertionFailed(message);
}

void TestCase::assertFalse(bool value, wstring message) {
    if (!value)
        runner->assertionPassed();
    else
        runner->assertionFailed(message);
}

void TestCase::assertNull(void* value, wstring message) {
    if (value == NULL)
        runner->assertionPassed();
    else
        runner->assertionFailed(message);
}

void TestCase::assertNotNull(void* value, wstring message) {
    if (value != NULL)
        runner->assertionPassed();
    else
        runner->assertionFailed(message);
}

void TestCase::setTestRunner(TestRunner* runner2) {
    runner = runner2;
}
