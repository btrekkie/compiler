#include <sstream>
#include "JSONDecoder.hpp"
#include "JSONValue.hpp"
#include "StringUtil.hpp"

using namespace std;

void JSONDecoder::deleteVector(vector<JSONValue*> value) {
    for (vector<JSONValue*>::const_iterator iterator = value.begin();
         iterator != value.end();
         iterator++) {
        if (*iterator != NULL)
            delete *iterator;
    }
}

void JSONDecoder::deleteMap(map<wstring, JSONValue*> value) {
    for (map<wstring, JSONValue*>::const_iterator iterator = value.begin();
         iterator != value.end();
         iterator++) {
        if (iterator->second != NULL)
            delete iterator->second;
    }
}

bool JSONDecoder::isNumber(wstring str) {
    if (str == L"")
        return false;
    int offset;
    if (str.at(0) != L'-')
        offset = 0;
    else
        offset = 1;
    int length = (int)str.length();
    if (length < offset + 1)
        return false;
    if (str.at(offset) == L'0') {
        offset++;
        if (offset == length)
            return true;
        if (str.at(offset) >= L'0' && str.at(offset) <= L'9')
            return false;
    } else {
        while (offset < length &&
               str.at(offset) >= L'0' &&
               str.at(offset) <= L'9')
            offset++;
        if (offset == length)
            return true;
    }
    if (str.at(offset) == L'.') {
        offset++;
        while (offset < length &&
               str.at(offset) >= L'0' &&
               str.at(offset) <= L'9')
            offset++;
        if (offset == length)
            return true;
    }
    if (str.at(offset) != L'e' && str.at(offset) != L'E')
        return false;
    offset++;
    if (offset == length)
        return false;
    if (str.at(offset) == L'+' || str.at(offset) == L'-') {
        offset++;
        if (offset == length)
            return false;
    }
    while (offset < length) {
        if (str.at(offset) < L'0' || str.at(offset) > L'9')
            return false;
        offset++;
    }
    return true;
}

bool JSONDecoder::isWhitespace(wchar_t c) {
    switch (c) {
        case L' ':
        case L'\t':
        case L'\n':
        case L'\r':
            return true;
        default:
            return false;
    }
}

bool JSONDecoder::hexDigitToInt(wchar_t c, int& value) {
    if (c >= L'0' && c <= L'9') {
        value = c - L'0';
        return true;
    } else if (c >= L'A' && c <= L'F') {
        value = c - L'A' + 10;
        return true;
    } else if (c >= L'a' && c <= L'f') {
        value = c - L'a' + 10;
        return true;
    } else
        return false;
}

bool JSONDecoder::readStr(std::wistream& input, std::wstring& value) {
    wostringstream output;
    wchar_t c;
    while ((c = input.get()) != input.eof() && input.good()) {
        if (c == L'"') {
            value = output.str();
            return true;
        } else if (c != L'\\')
            output << c;
        else {
            c = input.get();
            if (c == input.eof() || !input.good())
                return false;
            switch (c) {
                case L'"':
                case L'\\':
                case L'/':
                    output << c;
                    break;
                case L'b':
                    output << L'\b';
                    break;
                case L'f':
                    output << L'\f';
                    break;
                case L'n':
                    output << L'\n';
                    break;
                case L'r':
                    output << L'\r';
                    break;
                case L't':
                    output << L'\t';
                    break;
                case L'u':
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
                    output << (wchar_t)code;
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
    JSON_DECODE_TYPE_VALUE = L'\0',
    JSON_DECODE_TYPE_ERROR = L'\1'
};

wchar_t JSONDecoder::readScalarOrSeparator(
    std::wistream& input,
    JSONValue*& value) {
    wchar_t c;
    for (c = input.get(); isWhitespace(c); c = input.get());
    switch (c) {
        case L'[':
        case L',':
        case L']':
        case L'{':
        case L':':
        case L'}':
            return c;
        case L'"':
        {
            wstring str;
            if (!readStr(input, str))
                return JSON_DECODE_TYPE_ERROR;
            value = new JSONValue(str);
            return JSON_DECODE_TYPE_VALUE;
        }
        default:
        {
            wostringstream token;
            token << c;
            while ((c = input.peek()) != input.eof() && input.good()) {
                if (isWhitespace(c) ||
                    c == L',' ||
                    c == L':' ||
                    c == L']' ||
                    c == L'}') {
                    break;
                }
                token << (wchar_t)input.get();
            }
            wstring tokenStr = token.str();
            if (tokenStr == L"null")
                value = NULL;
            else if (tokenStr == L"true")
                value = new JSONValue(true);
            else if (tokenStr == L"false")
                value = new JSONValue(false);
            else if (isNumber(tokenStr))
                value = new JSONValue(
                    strtod(
                        StringUtil::asciiWstringToString(tokenStr).c_str(),
                        NULL));
            else
                return JSON_DECODE_TYPE_ERROR;
            return JSON_DECODE_TYPE_VALUE;
        }
    }
    return JSON_DECODE_TYPE_ERROR;
}

wchar_t JSONDecoder::readValueOrSeparator(
    std::wistream& input,
    JSONValue*& value) {

    wchar_t type = readScalarOrSeparator(input, value);
    switch (type) {
        case JSON_DECODE_TYPE_VALUE:
            return JSON_DECODE_TYPE_VALUE;
        case JSON_DECODE_TYPE_ERROR:
            return JSON_DECODE_TYPE_ERROR;
        case L'[':
        {
            JSONValue* element;
            type = readValueOrSeparator(input, element);
            if (type != JSON_DECODE_TYPE_VALUE && type != L']')
                return JSON_DECODE_TYPE_ERROR;
            vector<JSONValue*> array;
            bool first = true;
            while (type != L']') {
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
                if (type != L',' && type != L']') {
                    if (tempValue != NULL)
                        delete tempValue;
                    deleteVector(array);
                    return JSON_DECODE_TYPE_ERROR;
                }
            }
            value = new JSONValue(array);
            return JSON_DECODE_TYPE_VALUE;
        }
        case L'{':
        {
            JSONValue* key;
            type = readValueOrSeparator(input, key);
            if (type != JSON_DECODE_TYPE_VALUE && type != L'}')
                return JSON_DECODE_TYPE_ERROR;
            map<wstring, JSONValue*> object;
            bool first = true;
            while (type != L'}') {
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
                wstring keyStr = key->getStrValue();
                delete key;
                JSONValue* tempValue = NULL;
                type = readScalarOrSeparator(input, tempValue);
                if (type != L':') {
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
                object[keyStr] = objectValue;
                type = readScalarOrSeparator(input, tempValue);
                if (type != L',' && type != L'}') {
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

JSONValue* JSONDecoder::decode(std::wistream& input) {
    JSONValue* value;
    if (readValueOrSeparator(input, value) != JSON_DECODE_TYPE_VALUE)
        return NULL;
    wchar_t c;
    while ((c = input.get()) != input.eof() && input.good()) {
        if (!isWhitespace(c)) {
            delete value;
            return NULL;
        }
    }
    return value;
        
}
