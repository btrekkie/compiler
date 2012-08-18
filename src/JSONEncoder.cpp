#include <assert.h>
#include <sstream>
#include "JSONEncoder.hpp"
#include "JSONValue.hpp"

using namespace std;

JSONEncoder::JSONEncoder(wostream& output2) {
    output = &output2;
    indentationLevel = 0;
}

void JSONEncoder::outputIndentation() {
    for (int i = 0; i < indentationLevel; i++)
        *output << L"    ";
}

void JSONEncoder::outputHexChar(int value) {
    assert(value >= 0 && value < 16 || !L"Invalid hex character");
    if (value < 10)
        *output << (wchar_t)(value + L'0');
    else
        *output << (wchar_t)(value - 10 + L'a');
}

void JSONEncoder::startArray() {
    *output << L'[';
    indentationLevel++;
    justStartedArrayOrObject = true;
}

void JSONEncoder::startArrayElement() {
    if (!justStartedArrayOrObject)
        *output << L",\n";
    else {
        *output << L'\n';
        justStartedArrayOrObject = false;
    }
    outputIndentation();
}

void JSONEncoder::endArray() {
    indentationLevel--;
    if (justStartedArrayOrObject)
        justStartedArrayOrObject = false;
    else {
        *output << L"\n";
        outputIndentation();
    }
    *output << L']';
}

void JSONEncoder::startObject() {
    *output << L'{';
    indentationLevel++;
    justStartedArrayOrObject = true;
}

void JSONEncoder::appendObjectKey(wstring key) {
    if (!justStartedArrayOrObject)
        *output << L",\n";
    else {
        *output << L'\n';
        justStartedArrayOrObject = false;
    }
    outputIndentation();
    appendStr(key);
    *output << L": ";
}

void JSONEncoder::endObject() {
    indentationLevel--;
    if (justStartedArrayOrObject)
        justStartedArrayOrObject = false;
    else {
        *output << L"\n";
        outputIndentation();
    }
    *output << L'}';
}

void JSONEncoder::endRoot() {
    *output << L"\n";
}

void JSONEncoder::appendStr(wstring value) {
    *output << L'"';
    for (wstring::const_iterator iterator = value.begin();
         iterator != value.end();
         iterator++) {
        wchar_t c = *iterator;
        if (c == L'"' || c == L'\\')
            *output << L'\\' << c;
        else if (c >= L' ' && c <= L'~')
            // Printable ASCII character
            *output << c;
        else {
            *output << L"\\u";
            outputHexChar((c >> 12) & 0xF);
            outputHexChar((c >> 8) & 0xF);
            outputHexChar((c >> 4) & 0xF);
            outputHexChar(c & 0xF);
        }
    }
    *output << L'"';
}

void JSONEncoder::appendDouble(double value) {
    *output << value;
}

void JSONEncoder::appendInt(int value) {
    *output << value;
}

void JSONEncoder::appendBool(bool value) {
    *output << (value ? L"true" : L"false");
}

void JSONEncoder::appendNull() {
    *output << L"null";
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
            map<wstring, JSONValue*> object = value->getObjectValue();
            for (map<wstring, JSONValue*>::const_iterator iterator =
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
            assert(!L"Unhanded value type");
    }
}
