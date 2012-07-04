#ifndef INTERFACE_IO_TEST_HPP_INCLUDED
#define INTERFACE_IO_TEST_HPP_INCLUDED

#include <vector>
#include "TestCase.hpp"

class CFGType;
class ClassInterface;
class FieldInterface;
class MethodInterface;

/**
 * Unit test for InterfaceInput and InterfaceOutput.
 */
class InterfaceIOTest : public TestCase {
private:
    /**
     * Asserts that the specified CFGTypes are equal.
     */
    void assertTypesEqual(CFGType* expected, CFGType* actual);
    /**
     * Asserts that the specified vectors contain the same FieldInterfaces,
     * possibly in a different order.
     */
    void assertFieldInterfacesEqual(
        std::vector<FieldInterface*> expectedFields,
        std::vector<FieldInterface*> actualFields);
    /**
     * Asserts that the specified vectors contain the same MethodInterfaces,
     * possibly in a different order.  Assumes that "expectedMethods" does not
     * have multiple methods with the same identifier.
     */
    void assertMethodInterfacesEqual(
        std::vector<MethodInterface*> expectedMethods,
        std::vector<MethodInterface*> actualMethods);
    void assertClassInterfacesEqual(
        ClassInterface* expected,
        ClassInterface* actual);
    /**
     * Asserts that the ClassInterface produced by outputting and then inputting
     * the specified ClassInterface is equal to the original ClassInterface.
     */
    void checkInterface(ClassInterface* interface);
public:
    std::string getName();
    void test();
};

#endif
