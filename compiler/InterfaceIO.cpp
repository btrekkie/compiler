#include "Interface.hpp"
#include "InterfaceIO.hpp"
#include "JSON.hpp"

using namespace std;

InterfaceOutput::InterfaceOutput(ostream& output2) {
    output = new JSONEncoder(output2);
}

InterfaceOutput::~InterfaceOutput() {
    delete output;
}

void InterfaceOutput::outputFieldInterface(FieldInterface* interface) {
    output->startObject();
    output->appendObjectKey("identifier");
    output->appendStr(interface->getIdentifier());
    output->appendObjectKey("type");
    output->appendStr(interface->getType()->toString());
    output->endObject();
}

void InterfaceOutput::outputMethodInterface(MethodInterface* interface) {
    output->startObject();
    output->appendObjectKey("identifier");
    output->appendStr(interface->getIdentifier());
    output->appendObjectKey("returnType");
    if (interface->getReturnType() != NULL)
        output->appendStr(interface->getReturnType()->toString());
    else
        output->appendStr("void");
    output->appendObjectKey("argTypes");
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
    output->appendObjectKey("identifier");
    output->appendStr(interface->getIdentifier());
    output->appendObjectKey("fields");
    output->startArray();
    vector<FieldInterface*> fields = interface->getFields();
    for (vector<FieldInterface*>::const_iterator iterator = fields.begin();
         iterator != fields.end();
         iterator++) {
        output->startArrayElement();
        outputFieldInterface(*iterator);
    }
    output->endArray();
    output->appendObjectKey("methods");
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
