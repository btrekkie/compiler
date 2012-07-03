#include <sstream>
#include "../Interface.hpp"
#include "../InterfaceInput.hpp"
#include "../InterfaceOutput.hpp"
#include "InterfaceIOTest.hpp"

using namespace std;

void InterfaceIOTest::assertTypesEqual(CFGType* expected, CFGType* actual) {
    if (expected == NULL) {
        assertNull(actual, "Different types");
        return;
    }
    assertNotNull(actual, "Different types");
    assertEqual(
        expected->getClassName(),
        actual->getClassName(),
        "Different class names");
    assertEqual(
        expected->getNumDimensions(),
        actual->getNumDimensions(),
        "Different number of dimensions");
}

void InterfaceIOTest::assertFieldInterfacesEqual(
    vector<FieldInterface*> expectedFields,
    vector<FieldInterface*> actualFields) {
    assertEqual(
        expectedFields.size(),
        actualFields.size(),
        "Different number of fields");
    map<string, FieldInterface*> expectedFieldsMap;
    map<string, FieldInterface*> actualFieldsMap;
    for (int i = 0; i < (int)expectedFields.size(); i++) {
        expectedFieldsMap[expectedFields[i]->getIdentifier()] =
            expectedFields[i];
        actualFieldsMap[expectedFields[i]->getIdentifier()] = actualFields[i];
    }
    for (map<string, FieldInterface*>::const_iterator iterator =
             expectedFieldsMap.begin();
         iterator != expectedFieldsMap.end();
         iterator++) {
        assertTrue(
            actualFieldsMap.count(iterator->first) > 0,
            "Different field identifiers");
        assertTypesEqual(
            iterator->second->getType(),
            actualFieldsMap[iterator->first]->getType());
    }
}

void InterfaceIOTest::assertMethodInterfacesEqual(
    vector<MethodInterface*> expectedMethods,
    vector<MethodInterface*> actualMethods) {
    assertEqual(
        expectedMethods.size(),
        actualMethods.size(),
        "Different number of methods");
    map<string, MethodInterface*> expectedMethodsMap;
    map<string, MethodInterface*> actualMethodsMap;
    for (int i = 0; i < (int)expectedMethods.size(); i++) {
        expectedMethodsMap[expectedMethods[i]->getIdentifier()] =
            expectedMethods[i];
        actualMethodsMap[expectedMethods[i]->getIdentifier()] =
            actualMethods[i];
    }
    for (map<string, MethodInterface*>::const_iterator iterator =
             expectedMethodsMap.begin();
         iterator != expectedMethodsMap.end();
         iterator++) {
        assertTrue(
            actualMethodsMap.count(iterator->first) > 0,
            "Different method identifiers");
        MethodInterface* expectedMethod = iterator->second;
        MethodInterface* actualMethod = actualMethodsMap[iterator->first];
        assertTypesEqual(
            expectedMethod->getReturnType(),
            actualMethod->getReturnType());
        vector<CFGType*> expectedArgTypes = expectedMethod->getArgTypes();
        vector<CFGType*> actualArgTypes = actualMethod->getArgTypes();
        assertEqual(
            expectedArgTypes.size(),
            actualArgTypes.size(),
            "Different number of arguments");
        for (int i = 0; i < (int)expectedArgTypes.size(); i++)
            assertTypesEqual(expectedArgTypes[i], actualArgTypes[i]);
    }
}

void InterfaceIOTest::assertClassInterfacesEqual(
    ClassInterface* expected,
    ClassInterface* actual) {
    assertFieldInterfacesEqual(expected->getFields(), actual->getFields());
    assertMethodInterfacesEqual(expected->getMethods(), actual->getMethods());
    assertEqual(
        expected->getIdentifier(),
        actual->getIdentifier(),
        "Different identifiers");
}

void InterfaceIOTest::checkInterface(ClassInterface* interface) {
    ostringstream output;
    InterfaceOutput interfaceOutput(output);
    interfaceOutput.outputClassInterface(interface);
    istringstream input(output.str());
    ClassInterface* readInterface = InterfaceInput::readClassInterface(input);
    assertClassInterfacesEqual(interface, readInterface);
    delete readInterface;
}

string InterfaceIOTest::getName() {
    return "InterfaceIOTest";
}

void InterfaceIOTest::test() {
    ClassInterface class1(
        vector<FieldInterface*>(),
        vector<MethodInterface*>(),
        "Foo");
    checkInterface(&class1);
    
    vector<FieldInterface*> fields;
    fields.push_back(new FieldInterface(new CFGType("Int"), "Foo"));
    fields.push_back(new FieldInterface(new CFGType("Int", 2), "bar"));
    fields.push_back(new FieldInterface(new CFGType("Bool"), "baz"));
    vector<MethodInterface*> methods;
    vector<CFGType*> argTypes;
    methods.push_back(new MethodInterface(new CFGType("Int"), argTypes, "foo"));
    argTypes.push_back(new CFGType("Bool", 1));
    argTypes.push_back(new CFGType("Double"));
    methods.push_back(new MethodInterface(NULL, argTypes, "bar"));
    argTypes.clear();
    argTypes.push_back(new CFGType("Int"));
    methods.push_back(
        new MethodInterface(new CFGType("Float", 1), argTypes, "baz"));
    ClassInterface class2(fields, methods, "Bar");
    checkInterface(&class2);
}
