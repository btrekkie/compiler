#include <assert.h>
#include <sstream>
#include "JSONEncoder.hpp"
#include "JSONValue.hpp"

using namespace std;

JSONEncoder::JSONEncoder(ostream& output2) {
    output = &output2;
    indentationLevel = 0;
}

void JSONEncoder::outputIndentation() {
    for (int i = 0; i < indentationLevel; i++)
        *output << "    ";
}

void JSONEncoder::outputHexChar(int value) {
    assert(value >= 0 && value < 16 || !"Invalid hex character");
    if (value < 10)
        *output << (char)(value + '0');
    else
        *output << (char)(value - 10 + 'a');
}

void JSONEncoder::startArray() {
    *output << '[';
    indentationLevel++;
    justStartedArrayOrObject = true;
}

void JSONEncoder::startArrayElement() {
    if (!justStartedArrayOrObject)
        *output << ",\n";
    else {
        *output << '\n';
        justStartedArrayOrObject = false;
    }
    outputIndentation();
}

void JSONEncoder::endArray() {
    indentationLevel--;
    if (justStartedArrayOrObject)
        justStartedArrayOrObject = false;
    else {
        *output << "\n";
        outputIndentation();
    }
    *output << ']';
}

void JSONEncoder::startObject() {
    *output << '{';
    indentationLevel++;
    justStartedArrayOrObject = true;
}

void JSONEncoder::appendObjectKey(string key) {
    if (!justStartedArrayOrObject)
        *output << ",\n";
    else {
        *output << '\n';
        justStartedArrayOrObject = false;
    }
    outputIndentation();
    appendStr(key);
    *output << ": ";
}

void JSONEncoder::endObject() {
    indentationLevel--;
    if (justStartedArrayOrObject)
        justStartedArrayOrObject = false;
    else {
        *output << "\n";
        outputIndentation();
    }
    *output << '}';
}

void JSONEncoder::endRoot() {
    *output << "\n";
}

void JSONEncoder::appendStr(string value) {
    *output << '"';
    for (string::const_iterator iterator = value.begin();
         iterator != value.end();
         iterator++) {
        char c = *iterator;
        if (c == '"' || c == '\\')
            *output << '\\' << c;
        else if (c >= ' ' && c <= '~')
            // Printable ASCII character
            *output << c;
        else {
            *output << "\\u00";
            outputHexChar(((unsigned char)c) / 16);
            outputHexChar(((unsigned char)c) % 16);
        }
    }
    *output << '"';
}

void JSONEncoder::appendDouble(double value) {
    *output << value;
}

void JSONEncoder::appendInt(int value) {
    *output << value;
}

void JSONEncoder::appendBool(bool value) {
    *output << (value ? "true" : "false");
}

void JSONEncoder::appendNull() {
    *output << "null";
}

void JSONEncoder::appendValue(JSONValue* value) {
    if (value == NULL) {
        appendNull();
        return;
    }
    switch (value->getType()) {
        case JSON_TYPE_ARRAY:
        {
            startArray();
            vector<JSONValue*> array = value->getArrayValue();
            for (vector<JSONValue*>::const_iterator iterator = array.begin();
                 iterator != array.end();
                 iterator++) {
                startArrayElement();
                appendValue(*iterator);
            }
            endArray();
            break;
        }
        case JSON_TYPE_OBJECT:
        {
            startObject();
            map<string, JSONValue*> object = value->getObjectValue();
            for (map<string, JSONValue*>::const_iterator iterator =
                     object.begin();
                 iterator != object.end();
                 iterator++) {
                appendObjectKey(iterator->first);
                appendValue(iterator->second);
            }
            endObject();
            break;
        }
        case JSON_TYPE_STR:
            appendStr(value->getStrValue());
            break;
        case JSON_TYPE_NUMBER:
            appendDouble(value->getDoubleValue());
            break;
        case JSON_TYPE_BOOL:
            appendBool(value->getBoolValue());
            break;
        default:
            assert(!"Unhanded value type");
    }
}
