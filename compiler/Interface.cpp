#include <assert.h>
#include <sstream>
#include "Interface.hpp"

using namespace std;

CFGType::CFGType(string className2, int numDimensions2) {
    className = className2;
    numDimensions = numDimensions2;
}

CFGType::CFGType(CFGType* copy) {
    className = copy->className;
    numDimensions = copy->numDimensions;
}

string CFGType::getClassName() {
    return className;
}

int CFGType::getNumDimensions() {
    return numDimensions;
}

CFGType* CFGType::boolType() {
    return new CFGType("Bool");
}

CFGType* CFGType::intType() {
    return new CFGType("Int");
}

bool CFGType::isBool() {
    return numDimensions == 0 && className == "Bool";
}

bool CFGType::isNumeric() {
    if (numDimensions > 0)
        return false;
    else if (className == "Int")
        return true;
    else if (className == "Long")
        return true;
    else if (className == "Byte")
        return true;
    else if (className == "Float")
        return true;
    else if (className == "Double")
        return true;
    else
        return false;
}

bool CFGType::isIntegerLike() {
    if (numDimensions > 0)
        return false;
    else if (className == "Int")
        return true;
    else if (className == "Long")
        return true;
    else if (className == "Byte")
        return true;
    else
        return false;
}

int CFGType::getPromotionLevel() {
    if (className == "Byte")
        return 1;
    else if (className == "Int")
        return 2;
    else if (className == "Long")
        return 3;
    else if (className == "Float")
        return 4;
    else if (className == "Double")
        return 5;
    else {
        assert(!"Promotion only applies to numbers");
        return 0;
    }
}

bool CFGType::isMorePromotedThan(CFGType* other) {
    return getPromotionLevel() > other->getPromotionLevel();
}

string CFGType::toString() {
    stringstream output;
    output << className;
    for (int i = 0; i < numDimensions; i++)
        output << "[]";
    return output.str();
}

CFGType* CFGType::fromString(string str) {
    int i;
    for (i = 0; i < (int)str.length(); i++) {
        char c = str.at(i);
        if (c == '[')
            break;
    }
    string className = str.substr(0, i);
    int numDimensions = (((int)str.length()) - i) / 2;
    while (i < (int)str.length()) {
        if (i + 1 >= (int)str.length() ||
            str.at(i) != '[' ||
            str.at(i + 1) != ']')
            return NULL;
    }
    return new CFGType(className, numDimensions);
}

FieldInterface::FieldInterface(CFGType* type2, string identifier2) {
    type = type2;
    identifier = identifier2;
}

FieldInterface::~FieldInterface() {
    delete type;
}

CFGType* FieldInterface::getType() {
    return type;
}

string FieldInterface::getIdentifier() {
    return identifier;
}

MethodInterface::MethodInterface(
    CFGType* returnType2,
    vector<CFGType*> argTypes2,
    string identifier2) {
    returnType = returnType2;
    argTypes = argTypes2;
    identifier = identifier2;
}

MethodInterface::~MethodInterface() {
    if (returnType != NULL)
        delete returnType;
    for (vector<CFGType*>::const_iterator iterator = argTypes.begin();
         iterator != argTypes.end();
         iterator++)
        delete *iterator;
}

CFGType* MethodInterface::getReturnType() {
    return returnType;
}

vector<CFGType*> MethodInterface::getArgTypes() {
    return argTypes;
}

string MethodInterface::getIdentifier() {
    return identifier;
}

ClassInterface::ClassInterface(
    vector<FieldInterface*> fields2,
    vector<MethodInterface*> methods2,
    string identifier2) {
    methods = methods2;
    identifier = identifier2;
    for (vector<FieldInterface*>::const_iterator iterator = fields2.begin();
         iterator != fields2.end();
         iterator++) {
        FieldInterface* field = *iterator;
        assert(
            fields.count(field->getIdentifier()) == 0 ||
            !"Multiple fields with the same name");
        fields[field->getIdentifier()] = field;
    }
}

ClassInterface::~ClassInterface() {
    for (map<string, FieldInterface*>::const_iterator iterator = fields.begin();
         iterator != fields.end();
         iterator++)
        delete iterator->second;
    for (vector<MethodInterface*>::const_iterator iterator = methods.begin();
         iterator != methods.end();
         iterator++)
        delete *iterator;
}

vector<FieldInterface*> ClassInterface::getFields() {
    vector<FieldInterface*> fieldsVector;
    for (map<string, FieldInterface*>::const_iterator iterator = fields.begin();
         iterator != fields.end();
         iterator++)
        fieldsVector.push_back(iterator->second);
    return fieldsVector;
}

vector<MethodInterface*> ClassInterface::getMethods() {
    return methods;
}

string ClassInterface::getIdentifier() {
    return identifier;
}
