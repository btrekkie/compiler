#include <assert.h>
#include <sstream>
#include "Interface.hpp"

using namespace std;

CFGType::CFGType(wstring className2, int numDimensions2) {
    className = className2;
    numDimensions = numDimensions2;
}

CFGType::CFGType(CFGType* copy) {
    className = copy->className;
    numDimensions = copy->numDimensions;
}

wstring CFGType::getClassName() {
    return className;
}

int CFGType::getNumDimensions() {
    return numDimensions;
}

CFGType* CFGType::boolType() {
    return new CFGType(L"Bool");
}

CFGType* CFGType::intType() {
    return new CFGType(L"Int");
}

bool CFGType::isBool() {
    return numDimensions == 0 && className == L"Bool";
}

bool CFGType::isNumeric() {
    if (numDimensions > 0)
        return false;
    else if (className == L"Int")
        return true;
    else if (className == L"Long")
        return true;
    else if (className == L"Byte")
        return true;
    else if (className == L"Float")
        return true;
    else if (className == L"Double")
        return true;
    else
        return false;
}

bool CFGType::isIntegerLike() {
    if (numDimensions > 0)
        return false;
    else if (className == L"Int")
        return true;
    else if (className == L"Long")
        return true;
    else if (className == L"Byte")
        return true;
    else
        return false;
}

int CFGType::getPromotionLevel() {
    if (className == L"Byte")
        return 1;
    else if (className == L"Int")
        return 2;
    else if (className == L"Long")
        return 3;
    else if (className == L"Float")
        return 4;
    else if (className == L"Double")
        return 5;
    else {
        assert(!L"Promotion only applies to numbers");
        return 0;
    }
}

bool CFGType::isMorePromotedThan(CFGType* other) {
    return getPromotionLevel() > other->getPromotionLevel();
}

wstring CFGType::toString() {
    wostringstream output;
    output << className;
    for (int i = 0; i < numDimensions; i++)
        output << L"[]";
    return output.str();
}

CFGType* CFGType::fromString(wstring str) {
    int i;
    for (i = 0; i < (int)str.length(); i++) {
        wchar_t c = str.at(i);
        if (c == L'[')
            break;
    }
    wstring className = str.substr(0, i);
    int numDimensions = (((int)str.length()) - i) / 2;
    while (i < (int)str.length()) {
        if (i + 1 >= (int)str.length() ||
            str.at(i) != L'[' ||
            str.at(i + 1) != L']')
            return NULL;
        i += 2;
    }
    return new CFGType(className, numDimensions);
}

CFGReducedType CFGType::getReducedType() {
    if (numDimensions > 0)
        return REDUCED_TYPE_OBJECT;
    else if (className == L"Bool")
        return REDUCED_TYPE_BOOL;
    else if (className == L"Byte")
        return REDUCED_TYPE_BYTE;
    else if (className == L"Int")
        return REDUCED_TYPE_INT;
    else if (className == L"Long")
        return REDUCED_TYPE_LONG;
    else if (className == L"Float")
        return REDUCED_TYPE_FLOAT;
    else if (className == L"Double")
        return REDUCED_TYPE_DOUBLE;
    else
        return REDUCED_TYPE_OBJECT;
}

FieldInterface::FieldInterface(CFGType* type2, wstring identifier2) {
    type = type2;
    identifier = identifier2;
}

FieldInterface::~FieldInterface() {
    delete type;
}

CFGType* FieldInterface::getType() {
    return type;
}

wstring FieldInterface::getIdentifier() {
    return identifier;
}

MethodInterface::MethodInterface(
    CFGType* returnType2,
    vector<CFGType*> argTypes2,
    wstring identifier2) {
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

wstring MethodInterface::getIdentifier() {
    return identifier;
}

ClassInterface::ClassInterface(
    vector<FieldInterface*> fields2,
    vector<MethodInterface*> methods2,
    wstring identifier2) {
    methods = methods2;
    identifier = identifier2;
    for (vector<FieldInterface*>::const_iterator iterator = fields2.begin();
         iterator != fields2.end();
         iterator++) {
        FieldInterface* field = *iterator;
        assert(
            fields.count(field->getIdentifier()) == 0 ||
            !L"Multiple fields with the same name");
        fields[field->getIdentifier()] = field;
    }
}

ClassInterface::~ClassInterface() {
    for (map<wstring, FieldInterface*>::const_iterator iterator =
             fields.begin();
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
    for (map<wstring, FieldInterface*>::const_iterator iterator =
             fields.begin();
         iterator != fields.end();
         iterator++)
        fieldsVector.push_back(iterator->second);
    return fieldsVector;
}

vector<MethodInterface*> ClassInterface::getMethods() {
    return methods;
}

wstring ClassInterface::getIdentifier() {
    return identifier;
}
