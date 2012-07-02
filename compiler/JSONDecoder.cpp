#include <sstream>
#include "JSONDecoder.hpp"
#include "JSONValue.hpp"

using namespace std;

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
        offset++;
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
        offset++;
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
        value = c - 'a' + 10;
        return true;
    } else
        return false;
}

bool JSONDecoder::readStr(std::istream& input, std::string& value) {
    ostringstream output;
    char c;
    while ((c = input.get()) != input.eof() && input.good()) {
        if (c == '"') {
            value = output.str();
            return true;
        } else if (c != '\\')
            output << c;
        else {
            c = input.get();
            if (c == input.eof() || !input.good())
                return false;
            switch (c) {
                case '"':
                case '\\':
                case '/':
                    output << c;
                    break;
                case 'b':
                    output << '\b';
                    break;
                case 'f':
                    output << '\f';
                    break;
                case 'n':
                    output << '\n';
                    break;
                case 'r':
                    output << '\r';
                    break;
                case 't':
                    output << '\t';
                    break;
                case 'u':
                {
                    unsigned int code = 0;
                    for (int i = 0; i < 4; i++) {
                        c = input.get();
                        if (c == input.eof() || !input.good())
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
    for (c = input.get(); isWhitespace(c); c = input.get());
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
            return JSON_DECODE_TYPE_VALUE;
        }
        default:
        {
            ostringstream token;
            token << c;
            while ((c = input.peek()) != input.eof() && input.good()) {
                if (isWhitespace(c) ||
                    c == ',' ||
                    c == ':' ||
                    c == ']' ||
                    c == '}') {
                    break;
                }
                token << (char)input.get();
            }
            string tokenStr = token.str();
            if (tokenStr == "null")
                value = NULL;
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
    char type = readScalarOrSeparator(input, value);
    switch (type) {
        case JSON_DECODE_TYPE_VALUE:
            return JSON_DECODE_TYPE_VALUE;
        case JSON_DECODE_TYPE_ERROR:
            return JSON_DECODE_TYPE_ERROR;
        case '[':
        {
            JSONValue* element;
            type = readValueOrSeparator(input, element);
            if (type != JSON_DECODE_TYPE_VALUE && type != ']')
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
                JSONValue* tempValue = NULL;
                type = readScalarOrSeparator(input, tempValue);
                if (type != ',' && type != ']') {
                    if (tempValue != NULL)
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
            type = readValueOrSeparator(input, key);
            if (type != JSON_DECODE_TYPE_VALUE && type != '}')
                return JSON_DECODE_TYPE_ERROR;
            map<string, JSONValue*> object;
            bool first = true;
            while (type != '}') {
                if (!first)
                    type = readScalarOrSeparator(input, key);
                first = false;
                if (type != JSON_DECODE_TYPE_VALUE ||
                    key == NULL ||
                    key->getType() != JSON_TYPE_STR) {
                    if (key != NULL)
                        delete key;
                    deleteMap(object);
                    return JSON_DECODE_TYPE_ERROR;
                }
                JSONValue* tempValue = NULL;
                type = readScalarOrSeparator(input, tempValue);
                if (type != ':') {
                    if (tempValue != NULL)
                        delete tempValue;
                    deleteMap(object);
                    return JSON_DECODE_TYPE_ERROR;
                }
                JSONValue* objectValue;
                type = readValueOrSeparator(input, objectValue);
                if (type != JSON_DECODE_TYPE_VALUE) {
                    deleteMap(object);
                    return JSON_DECODE_TYPE_ERROR;
                }
                object[key->getStrValue()] = objectValue;
                type = readScalarOrSeparator(input, tempValue);
                if (type != ',' && type != '}') {
                    if (tempValue != NULL)
                        delete tempValue;
                    deleteMap(object);
                    return JSON_DECODE_TYPE_ERROR;
                }
            }
            value = new JSONValue(object);
            return JSON_DECODE_TYPE_VALUE;
        }
    }
    return type;
}

JSONValue* JSONDecoder::decode(std::istream& input) {
    JSONValue* value;
    if (readValueOrSeparator(input, value) != JSON_DECODE_TYPE_VALUE)
        return NULL;
    char c;
    while ((c = input.get()) != input.eof() && input.good()) {
        if (!isWhitespace(c)) {
            delete value;
            return NULL;
        }
    }
    return value;
        
}
