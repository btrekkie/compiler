#include <assert.h>
#include "JSONValue.hpp"

using namespace std;

JSONValue::JSONValue(vector<JSONValue*> arrayValue2) {
    type = JSON_TYPE_ARRAY;
    arrayValue = new vector<JSONValue*>(arrayValue2);
    objectValue = NULL;
    strValue = NULL;
}

JSONValue::JSONValue(map<string, JSONValue*> objectValue2) {
    type = JSON_TYPE_OBJECT;
    objectValue = new map<string, JSONValue*>(objectValue2);
    arrayValue = NULL;
    strValue = NULL;
}

JSONValue::JSONValue(string strValue2) {
    type = JSON_TYPE_STR;
    strValue = new string(strValue2);
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
        for (map<string, JSONValue*>::const_iterator iterator =
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
    assert(type == JSON_TYPE_ARRAY || !"Not an array value");
    return *arrayValue;
}

map<string, JSONValue*> JSONValue::getObjectValue() {
    assert(type == JSON_TYPE_OBJECT || !"Not an object value");
    return *objectValue;
}

string JSONValue::getStrValue() {
    assert(type == JSON_TYPE_STR || !"Not a string value");
    return *strValue;
}

double JSONValue::getDoubleValue() {
    assert(type == JSON_TYPE_NUMBER || !"Not a number value");
    return numberValue;
}

int JSONValue::getIntValue() {
    assert(type == JSON_TYPE_NUMBER || !"Not a number value");
    return (int)numberValue;
}

bool JSONValue::getBoolValue() {
    assert(type == JSON_TYPE_NUMBER || !"Not a boolean value");
    return boolValue;
}

JSONValue* JSONValue::getField(string key) {
    assert(type == JSON_TYPE_OBJECT || !"Not an object value");
    if (objectValue->count(key) > 0)
        return (*objectValue)[key];
    else
        return NULL;
}
