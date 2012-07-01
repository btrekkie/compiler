#include <memory>
#include <vector>
#include "Interface.hpp"
#include "InterfaceInput.hpp"
#include "JSONDecoder.hpp"

using namespace std;

/**
 * Deallocates each of the values in the specified vector.
 */
template<class T>
static void deleteVector(vector<T*> values) {
    for (int i = 0; i < (int)values.size(); i++)
        delete values.at(i);
}

FieldInterface* InterfaceInput::readFieldInterface(JSONValue* value) {
    if (value == NULL || value->getType() != JSON_TYPE_OBJECT)
        return NULL;
    JSONValue* identifierValue = value->getField("identifier");
    if (identifierValue == NULL || identifierValue->getType() != JSON_TYPE_STR)
        return NULL;
    JSONValue* typeValue = value->getField("type");
    if (typeValue == NULL || typeValue->getType() != JSON_TYPE_STR)
        return NULL;
    CFGType* type = CFGType::fromString(typeValue->getStrValue());
    if (type == NULL)
        return NULL;
    return new FieldInterface(type, identifierValue->getStrValue());
}

MethodInterface* InterfaceInput::readMethodInterface(JSONValue* value) {
    if (value == NULL || value->getType() != JSON_TYPE_OBJECT)
        return NULL;
    JSONValue* identifierValue = value->getField("identifier");
    if (identifierValue == NULL || identifierValue->getType() != JSON_TYPE_STR)
        return NULL;
    JSONValue* returnTypeValue = value->getField("returnType");
    if (returnTypeValue == NULL || returnTypeValue->getType() != JSON_TYPE_STR)
        return NULL;
    CFGType* returnType;
    if (returnTypeValue->getStrValue() == "void")
        returnType = NULL;
    else {
        returnType = CFGType::fromString(returnTypeValue->getStrValue());
        if (returnType == NULL)
            return NULL;
    }
    JSONValue* argTypesValue = value->getField("argTypes");
    if (argTypesValue == NULL || argTypesValue->getType() != JSON_TYPE_ARRAY)
        return NULL;
    vector<JSONValue*> argTypesValues = argTypesValue->getArrayValue();
    vector<CFGType*> argTypes;
    for (vector<JSONValue*>::const_iterator iterator = argTypesValues.begin();
         iterator != argTypesValues.end();
         iterator++) {
        JSONValue* argTypeValue = *iterator;
        if (argTypeValue == NULL || argTypeValue->getType() != JSON_TYPE_STR) {
            deleteVector(argTypes);
            return NULL;
        }
        CFGType* argType = CFGType::fromString(argTypeValue->getStrValue());
        if (argType == NULL) {
            deleteVector(argTypes);
            return NULL;
        }
        argTypes.push_back(argType);
    }
    return new MethodInterface(
        returnType,
        argTypes,
        identifierValue->getStrValue());
}

ClassInterface* InterfaceInput::readClassInterface(istream& input) {
    auto_ptr<JSONValue> value(JSONDecoder::decode(input));
    if (value.get() == NULL || value->getType() != JSON_TYPE_OBJECT)
        return NULL;
    JSONValue* identifierValue = value->getField("identifier");
    if (identifierValue == NULL || identifierValue->getType() != JSON_TYPE_STR)
        return NULL;
    
    // Read fields
    JSONValue* fieldsValue = value->getField("fields");
    if (fieldsValue == NULL || fieldsValue->getType() != JSON_TYPE_ARRAY)
        return NULL;
    vector<JSONValue*> fieldsValues = fieldsValue->getArrayValue();
    vector<FieldInterface*> fieldInterfaces;
    for (vector<JSONValue*>::const_iterator iterator = fieldsValues.begin();
         iterator != fieldsValues.end();
         iterator++) {
        FieldInterface* fieldInterface = readFieldInterface(*iterator);
        if (fieldInterface == NULL) {
            deleteVector(fieldInterfaces);
            return NULL;
        }
        fieldInterfaces.push_back(fieldInterface);
    }
    
    // Read methods
    JSONValue* methodsValue = value->getField("methods");
    if (methodsValue == NULL || methodsValue->getType() != JSON_TYPE_ARRAY)
        return NULL;
    vector<JSONValue*> methodsValues = methodsValue->getArrayValue();
    vector<MethodInterface*> methodInterfaces;
    for (vector<JSONValue*>::const_iterator iterator = methodsValues.begin();
         iterator != methodsValues.end();
         iterator++) {
        MethodInterface* methodInterface = readMethodInterface(*iterator);
        if (methodInterface == NULL) {
            deleteVector(fieldInterfaces);
            deleteVector(methodInterfaces);
            return NULL;
        }
        methodInterfaces.push_back(methodInterface);
    }
    return new ClassInterface(
        fieldInterfaces,
        methodInterfaces,
        identifierValue->getStrValue());
}
