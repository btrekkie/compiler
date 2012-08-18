#include <assert.h>
#include "JSONValue.hpp"

using namespace std;

JSONValue::JSONValue(vector<JSONValue*> arrayValue2) {
    type = JSON_TYPE_ARRAY;
    arrayValue = new vector<JSONValue*>(arrayValue2);
    objectValue = NULL;
    strValue = NULL;
}

JSONValue::JSONValue(map<wstring, JSONValue*> objectValue2) {
    type = JSON_TYPE_OBJECT;
    objectValue = new map<wstring, JSONValue*>(objectValue2);
    arrayValue = NULL;
    strValue = NULL;
}

JSONValue::JSONValue(wstring strValue2) {
    type = JSON_TYPE_STR;
    strValue = new wstring(strValue2);
    arrayValue = NULL;
    objectValue = NULL;
}

JSONValue::JSONValue(const wchar_t* strValue2) {
    type = JSON_TYPE_STR;
    strValue = new wstring(strValue2);
    arrayValue = NULL;
    objectValue = NULL;
}

JSONValue::JSONValue(double numberValue2) {
    type = JSON_TYPE_NUMBER;
    numberValue = numberValue2;
    arrayValue = NULL;
    objectValue = NULL;
    strValue = NULL;
}

JSONValue::JSONValue(int numberValue2) {
    type = JSON_TYPE_NUMBER;
    numberValue = numberValue2;
    arrayValue = NULL;
    objectValue = NULL;
    strValue = NULL;
}

JSONValue::JSONValue(bool boolValue2) {
    type = JSON_TYPE_BOOL;
    boolValue = boolValue2;
    arrayValue = NULL;
    objectValue = NULL;
    strValue = NULL;
}

JSONValue::~JSONValue() {
    if (arrayValue != NULL) {
        for (vector<JSONValue*>::const_iterator iterator = arrayValue->begin();
             iterator != arrayValue->end();
             iterator++) {
            if (*iterator)
                delete *iterator;
        }
        delete arrayValue;
    } else if (objectValue != NULL) {
        for (map<wstring, JSONValue*>::const_iterator iterator =
                 objectValue->begin();
             iterator != objectValue->end();
             iterator++) {
            if (iterator->second)
                delete iterator->second;
        }
        delete objectValue;
    } else if (strValue != NULL)
        delete strValue;
}

JSONValueType JSONValue::getType() {
    return type;
}

vector<JSONValue*> JSONValue::getArrayValue() {
    assert(type == JSON_TYPE_ARRAY || !L"Not an array value");
    return *arrayValue;
}

map<wstring, JSONValue*> JSONValue::getObjectValue() {
    assert(type == JSON_TYPE_OBJECT || !L"Not an object value");
    return *objectValue;
}

wstring JSONValue::getStrValue() {
    assert(type == JSON_TYPE_STR || !L"Not a wstring value");
    return *strValue;
}

double JSONValue::getDoubleValue() {
    assert(type == JSON_TYPE_NUMBER || !L"Not a number value");
    return numberValue;
}

int JSONValue::getIntValue() {
    assert(type == JSON_TYPE_NUMBER || !L"Not a number value");
    return (int)numberValue;
}

bool JSONValue::getBoolValue() {
    assert(type == JSON_TYPE_BOOL || !L"Not a boolean value");
    return boolValue;
}

JSONValue* JSONValue::getField(wstring key) {
    assert(type == JSON_TYPE_OBJECT || !L"Not an object value");
    if (objectValue->count(key) > 0)
        return (*objectValue)[key];
    else
        return NULL;
}
