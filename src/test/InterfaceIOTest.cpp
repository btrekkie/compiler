#include <sstream>
#include "../Interface.hpp"
#include "../InterfaceInput.hpp"
#include "../InterfaceOutput.hpp"
#include "InterfaceIOTest.hpp"

using namespace std;

void InterfaceIOTest::assertTypesEqual(CFGType* expected, CFGType* actual) {
    if (expected == NULL) {
        assertNull(actual, L"Different types");
        return;
    }
    assertNotNull(actual, L"Different types");
    assertEqual(
        expected->getClassName(),
        actual->getClassName(),
        L"Different class names");
    assertEqual(
        expected->getNumDimensions(),
        actual->getNumDimensions(),
        L"Different number of dimensions");
}

void InterfaceIOTest::assertFieldInterfacesEqual(
    vector<FieldInterface*> expectedFields,
    vector<FieldInterface*> actualFields) {
    assertEqual(
        expectedFields.size(),
        actualFields.size(),
        L"Different number of fields");
    map<wstring, FieldInterface*> expectedFieldsMap;
    map<wstring, FieldInterface*> actualFieldsMap;
    for (int i = 0; i < (int)expectedFields.size(); i++) {
        expectedFieldsMap[expectedFields[i]->getIdentifier()] =
            expectedFields[i];
        actualFieldsMap[expectedFields[i]->getIdentifier()] = actualFields[i];
    }
    for (map<wstring, FieldInterface*>::const_iterator iterator =
             expectedFieldsMap.begin();
         iterator != expectedFieldsMap.end();
         iterator++) {
        assertTrue(
            actualFieldsMap.count(iterator->first) > 0,
            L"Different field identifiers");
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
        L"Different number of methods");
    map<wstring, MethodInterface*> expectedMethodsMap;
    map<wstring, MethodInterface*> actualMethodsMap;
    for (int i = 0; i < (int)expectedMethods.size(); i++) {
        expectedMethodsMap[expectedMethods[i]->getIdentifier()] =
            expectedMethods[i];
        actualMethodsMap[expectedMethods[i]->getIdentifier()] =
            actualMethods[i];
    }
    for (map<wstring, MethodInterface*>::const_iterator iterator =
             expectedMethodsMap.begin();
         iterator != expectedMethodsMap.end();
         iterator++) {
        assertTrue(
            actualMethodsMap.count(iterator->first) > 0,
            L"Different method identifiers");
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
            L"Different number of arguments");
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
        L"Different identifiers");
}

void InterfaceIOTest::checkInterface(ClassInterface* interface) {
    wostringstream output;
    InterfaceOutput interfaceOutput(output);
    interfaceOutput.outputClassInterface(interface);
    wistringstream input(output.str());
    ClassInterface* readInterface = InterfaceInput::readClassInterface(input);
    assertClassInterfacesEqual(interface, readInterface);
    delete readInterface;
}

wstring InterfaceIOTest::getName() {
    return L"InterfaceIOTest";
}

void InterfaceIOTest::test() {
    ClassInterface class1(
        vector<FieldInterface*>(),
        vector<MethodInterface*>(),
        L"Foo");
    checkInterface(&class1);
    
    vector<FieldInterface*> fields;
    fields.push_back(new FieldInterface(new CFGType(L"Int"), L"Foo"));
    fields.push_back(new FieldInterface(new CFGType(L"Int", 2), L"bar"));
    fields.push_back(new FieldInterface(new CFGType(L"Bool"), L"baz"));
    vector<MethodInterface*> methods;
    vector<CFGType*> argTypes;
    methods.push_back(
        new MethodInterface(new CFGType(L"Int"), argTypes, L"foo"));
    argTypes.push_back(new CFGType(L"Bool", 1));
    argTypes.push_back(new CFGType(L"Double"));
    methods.push_back(new MethodInterface(NULL, argTypes, L"bar"));
    argTypes.clear();
    argTypes.push_back(new CFGType(L"Int"));
    methods.push_back(
        new MethodInterface(new CFGType(L"Float", 1), argTypes, L"baz"));
    ClassInterface class2(fields, methods, L"Bar");
    checkInterface(&class2);
}
