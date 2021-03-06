#include "Interface.hpp"
#include "InterfaceOutput.hpp"
#include "JSONEncoder.hpp"

using namespace std;

InterfaceOutput::InterfaceOutput(wostream& output2) {
    output = new JSONEncoder(output2);
}

InterfaceOutput::~InterfaceOutput() {
    delete output;
}

void InterfaceOutput::outputFieldInterface(FieldInterface* interface) {
    output->startObject();
    output->appendObjectKey(L"identifier");
    output->appendStr(interface->getIdentifier());
    output->appendObjectKey(L"type");
    output->appendStr(interface->getType()->toString());
    output->endObject();
}

void InterfaceOutput::outputMethodInterface(MethodInterface* interface) {
    output->startObject();
    output->appendObjectKey(L"identifier");
    output->appendStr(interface->getIdentifier());
    output->appendObjectKey(L"returnType");
    if (interface->getReturnType() != NULL)
        output->appendStr(interface->getReturnType()->toString());
    else
        output->appendStr(L"void");
    output->appendObjectKey(L"argTypes");
    output->startArray();
    vector<CFGType*> argTypes = interface->getArgTypes();
    for (vector<CFGType*>::const_iterator iterator = argTypes.begin();
         iterator != argTypes.end();
         iterator++) {
        output->startArrayElement();
        output->appendStr((*iterator)->toString());
    }
    output->endArray();
    output->endObject();
}

void InterfaceOutput::outputClassInterface(ClassInterface* interface) {
    output->startObject();
    output->appendObjectKey(L"identifier");
    output->appendStr(interface->getIdentifier());
    output->appendObjectKey(L"fields");
    output->startArray();
    vector<FieldInterface*> fields = interface->getFields();
    for (vector<FieldInterface*>::const_iterator iterator = fields.begin();
         iterator != fields.end();
         iterator++) {
        output->startArrayElement();
        outputFieldInterface(*iterator);
    }
    output->endArray();
    output->appendObjectKey(L"methods");
    output->startArray();
    vector<MethodInterface*> methods = interface->getMethods();
    for (vector<MethodInterface*>::const_iterator iterator = methods.begin();
         iterator != methods.end();
         iterator++) {
        output->startArrayElement();
        outputMethodInterface(*iterator);
    }
    output->endArray();
    output->endObject();
    output->endRoot();
}
