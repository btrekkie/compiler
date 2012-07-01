#include <assert.h>
#include <sstream>
#include "JSON.hpp"

using namespace std;

JSONValue::JSONValue(vector<JSONValue*> arrayValue2) {
    type = JSON_TYPE_ARRAY;
    arrayValue = new vector<JSONValue*>(arrayValue2);
}

JSONValue::JSONValue(map<string, JSONValue*> objectValue2) {
    type = JSON_TYPE_OBJECT;
    objectValue = new map<string, JSONValue*>(objectValue2);
}

JSONValue::JSONValue(string strValue2) {
    type = JSON_TYPE_STR;
    strValue = new string(strValue2);
}

JSONValue::JSONValue(double numberValue2) {
    type = JSON_TYPE_NUMBER;
    numberValue = numberValue2;
}

JSONValue::JSONValue(bool boolValue2) {
    type = JSON_TYPE_BOOL;
    boolValue = boolValue2;
}

JSONValue::JSONValue() {
    type = JSON_TYPE_NULL;
}

JSONValue::~JSONValue() {
    if (arrayValue != NULL) {
        for (vector<JSONValue*>::const_iterator iterator = arrayValue->begin();
             iterator != arrayValue->end();
             iterator++)
            delete *iterator;
        delete arrayValue;
    } else if (objectValue != NULL) {
        for (map<string, JSONValue*>::const_iterator iterator =
                 objectValue->begin();
             iterator != objectValue->end();
             iterator++)
            delete iterator->second;
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

void JSONDecoder::deleteVector(vector<JSONValue*> value) {
    for (vector<JSONValue*>::const_iterator iterator = value.begin();
         iterator != value.end();
         iterator++)
        delete *iterator;
}

void JSONDecoder::deleteMap(map<string, JSONValue*> value) {
    for (map<string, JSONValue*>::const_iterator iterator = value.begin();
         iterator != value.end();
         iterator++)
        delete iterator->second;
}

bool JSONDecoder::isNumber(string str) {
    if (str == "")
        return false;
    int offset;
    if (str.at(0) != '-')
        offset = 0;
    else
        offset = 1;
    int length = (int)str.length();
    if (length < offset + 1)
        return false;
    if (str.at(offset) == '0') {
        offset++;
        if (offset == length)
            return true;
        if (str.at(offset) >= '0' && str.at(offset) <= '9')
            return false;
    } else {
        while (offset < length &&
               str.at(offset) >= '0' &&
               str.at(offset) <= '9')
            offset++;
        if (offset == length)
            return true;
    }
    if (str.at(offset) == '.') {
        while (offset < length &&
               str.at(offset) >= '0' &&
               str.at(offset) <= '9')
            offset++;
        if (offset == length)
            return true;
    }
    if (str.at(offset) != 'e' && str.at(offset) != 'E')
        return false;
    offset++;
    if (offset == length)
        return false;
    if (str.at(offset) == '+' || str.at(offset) == '-') {
        offset++;
        if (offset == length)
            return false;
    }
    while (offset < length) {
        if (str.at(offset) < '0' || str.at(offset) > '9')
            return false;
    }
    return true;
}

bool JSONDecoder::isWhitespace(char c) {
    switch (c) {
        case ' ':
        case '\t':
        case '\n':
        case '\r':
            return true;
        default:
            return false;
    }
}

bool JSONDecoder::hexDigitToInt(char c, int& value) {
    if (c >= '0' && c <= '9') {
        value = c - '0';
        return true;
    } else if (c >= 'A' && c <= 'F') {
        value = c - 'A' + 10;
        return true;
    } else if (c >= 'a' && c <= 'f') {
        return c - 'a' + 10;
        return true;
    } else
        return false;
}

bool JSONDecoder::readStr(std::istream& input, std::string& value) {
    stringstream output;
    char c;
    while ((c = input.get()) != input.eof()) {
        if (c == '"') {
            value = output.str();
            return true;
        } else if (c != '\\')
            output << c;
        else {
            c = input.get();
            if (c == input.eof())
                return false;
            switch (c) {
                case '"':
                case '\\':
                case '/':
                    output << c;
                    break;
                case 'b':
                    output << "\b";
                    break;
                case 'f':
                    output << "\f";
                    break;
                case 'n':
                    output << "\n";
                    break;
                case 'r':
                    output << "\r";
                    break;
                case 't':
                    output << "\t";
                    break;
                case 'u':
                {
                    unsigned int code = 0;
                    for (int i = 0; i < 4; i++) {
                        c = input.get();
                        if (c == input.eof())
                            return false;
                        int hexValue;
                        if (!hexDigitToInt(c, hexValue))
                            return false;
                        code = (code << 4) | hexValue;
                    }
                    if (code >= 256)
                        // Unicode characters are not supported (yet)
                        return false;
                    output << (char)code;
                    break;
                }
                default:
                    return false;
            }
        }
    }
    // Did not find closing quotes
    return false;
}

enum JSONDecodeType {
    JSON_DECODE_TYPE_VALUE = '\0',
    JSON_DECODE_TYPE_ERROR = '\1'
};

char JSONDecoder::readScalarOrSeparator(
    std::istream& input,
    JSONValue*& value) {
    char c;
    for (char c = input.get(); isWhitespace(c); c = input.get());
    switch (c) {
        case '[':
        case ',':
        case ']':
        case '{':
        case ':':
        case '}':
            return c;
        case '"':
        {
            string str;
            if (!readStr(input, str))
                return JSON_DECODE_TYPE_ERROR;
            value = new JSONValue(str);
            break;
        }
        default:
        {
            stringstream token;
            token << c;
            while ((c = input.peek()) != input.eof()) {
                if (isWhitespace(c) ||
                    c == ',' ||
                    c == ':' ||
                    c == ']' ||
                    c == '}') {
                    break;
                }
                token << input.get();
            }
            string tokenStr = token.str();
            if (tokenStr == "null")
                value = new JSONValue();
            else if (tokenStr == "true")
                value = new JSONValue(true);
            else if (tokenStr == "false")
                value = new JSONValue(false);
            else if (isNumber(tokenStr))
                value = new JSONValue(strtod(tokenStr.c_str(), NULL));
            else
                return JSON_DECODE_TYPE_ERROR;
            return JSON_DECODE_TYPE_VALUE;
        }
    }
    return JSON_DECODE_TYPE_ERROR;
}

char JSONDecoder::readValueOrSeparator(std::istream& input, JSONValue*& value) {
    switch (readScalarOrSeparator(input, value)) {
        case JSON_DECODE_TYPE_VALUE:
            return JSON_DECODE_TYPE_VALUE;
        case JSON_DECODE_TYPE_ERROR:
            return JSON_DECODE_TYPE_ERROR;
        case '[':
        {
            JSONValue* element;
            char type = readValueOrSeparator(input, element);
            if (type == JSON_DECODE_TYPE_ERROR ||
                (type != JSON_DECODE_TYPE_VALUE && type != ']'))
                return JSON_DECODE_TYPE_ERROR;
            vector<JSONValue*> array;
            bool first = true;
            while (type != ']') {
                if (!first)
                    type = readValueOrSeparator(input, element);
                first = false;
                if (type != JSON_DECODE_TYPE_VALUE) {
                    deleteVector(array);
                    return JSON_DECODE_TYPE_ERROR;
                }
                array.push_back(element);
                JSONValue* tempValue;
                type = readScalarOrSeparator(input, tempValue);
                if (type != ',' && type != ']') {
                    if (type == JSON_DECODE_TYPE_VALUE)
                        delete tempValue;
                    deleteVector(array);
                    return JSON_DECODE_TYPE_ERROR;
                }
            }
            value = new JSONValue(array);
            return JSON_DECODE_TYPE_VALUE;
        }
        case '{':
        {
            JSONValue* key;
            char type = readValueOrSeparator(input, key);
            if (type == JSON_DECODE_TYPE_ERROR ||
                (type != JSON_DECODE_TYPE_VALUE && type != '}'))
                return JSON_DECODE_TYPE_ERROR;
            map<string, JSONValue*> object;
            bool first = true;
            while (type != '}') {
                if (!first) {
                    type = readScalarOrSeparator(input, key);
                }
                first = false;
                if (type != JSON_DECODE_TYPE_VALUE ||
                    key->getType() != JSON_TYPE_STR) {
                    deleteMap(object);
                    return JSON_DECODE_TYPE_ERROR;
                }
                JSONValue* tempValue;
                type = readScalarOrSeparator(input, value);
                if (type != ':') {
                    if (tempValue)
                        delete tempValue;
                    deleteMap(object);
                    return JSON_DECODE_TYPE_ERROR;
                }
                JSONValue* value;
                type = readScalarOrSeparator(input, value);
                if (type != JSON_DECODE_TYPE_VALUE) {
                    deleteMap(object);
                    return JSON_DECODE_TYPE_ERROR;
                }
                object[key->getStrValue()] = value;
                type = readScalarOrSeparator(input, value);
                if (type != ',' && type != '}') {
                    deleteMap(object);
                    return JSON_DECODE_TYPE_ERROR;
                }
            }
            value = new JSONValue(object);
            return JSON_DECODE_TYPE_VALUE;
        }
    }
    return JSON_DECODE_TYPE_ERROR;
}

JSONValue* JSONDecoder::decode(std::istream& input) {
    JSONValue* value;
    if (readValueOrSeparator(input, value) != JSON_DECODE_TYPE_VALUE)
        return NULL;
    char c;
    while ((c = input.get()) != input.eof()) {
        if (!isWhitespace(c)) {
            delete value;
            return NULL;
        }
    }
    return value;
        
}

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
            *output << c;
        else {
            *output << "\\u";
            outputHexChar(((unsigned char)c) / 16);
            outputHexChar(((unsigned char)c) % 16);
        }
    }
    *output << '"';
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
