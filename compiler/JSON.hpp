#ifndef JSON_HPP_INCLUDED
#define JSON_HPP_INCLUDED

#include <iostream>
#include <map>
#include <string>
#include <vector>

enum JSONValueType {
    JSON_TYPE_ARRAY,
    JSON_TYPE_OBJECT,
    JSON_TYPE_STR,
    JSON_TYPE_NUMBER,
    JSON_TYPE_BOOL,
    JSON_TYPE_NULL
};

/**
 * A JSON-compatible value.
 */
class JSONValue {
private:
    JSONValueType type;
    /**
     * The value of this, if this is of the JSON array type.
     */
    std::vector<JSONValue*>* arrayValue;
    /**
     * The value of this, if this is of the JSON object type.
     */
    std::map<std::string, JSONValue*>* objectValue;
    /**
     * The value of this, if this is of the JSON string type.
     */
    std::string* strValue;
    /**
     * The value of this, if this is of the JSON number type.
     */
    double numberValue;
    /**
     * The value of this, if this is of the JSON boolean type.
     */
    bool boolValue;
public:
    explicit JSONValue(std::vector<JSONValue*> arrayValue2);
    explicit JSONValue(std::map<std::string, JSONValue*> objectValue2);
    explicit JSONValue(double numberValue2);
    explicit JSONValue(std::string strValue2);
    explicit JSONValue(bool boolValue2);
    /**
     * Constructs a new JSONValue for a NULL value.
     */
    JSONValue();
    ~JSONValue();
    JSONValueType getType();
    std::vector<JSONValue*> getArrayValue();
    std::map<std::string, JSONValue*> getObjectValue();
    std::string getStrValue();
    double getDoubleValue();
    int getIntValue();
    bool getBoolValue();
};

/**
 * A class for decoding a JSON string.
 */
class JSONDecoder {
private:
    /**
     * Deallocates each of the values in the specified vector.
     */
    static void deleteVector(std::vector<JSONValue*> value);
    /**
     * Deallocates each of the values in the specified map.
     */
    static void deleteMap(std::map<std::string, JSONValue*> value);
    /**
     * Returns whether the specified string is a JSON-encoded number.
     */
    static bool isNumber(std::string str);
    /**
     * Returns whether the specified character is a JSON whitespace character.
     */
    static bool isWhitespace(char c);
    /**
     * Converts the specified hexadecimal character to an integer.
     * @param c the character to convert.
     * @param value a reference in which to store the results.
     * @return whether the character is a valid hexadecimal character.
     */
    static bool hexDigitToInt(char c, int& value);
    /**
     * Reads a string value from a JSON-encoded string, sans the leading
     * quotation mark.
     * @param input the input stream from which to read.
     * @param value a reference in which to store the string that this method
     *     read.
     * @return whether the input stream contained a valid JSON-encoded string.
     */
    static bool readStr(std::istream& input, std::string& value);
    /**
     * Non-recursive method that reads a JSON scalar value or a "separator" like
     * '{' or ','.
     * @param input the input stream from which to read.
     * @param value a reference in which to store the scalar value.  If the
     *     method returns something other than JSON_DECODE_TYPE_VALUE, the
     *     method does not alter "value".
     * @return a value indicating what type of thing we read.  See the comments
     *     for the return value of "readValue" for more information.
     */
    static char readScalarOrSeparator(std::istream& input, JSONValue*& value);
    /**
     * Recursive method that reads a JSON value or a "separator" like '{' or
     * ','.
     * @param input the input stream from which to read.
     * @param value a reference in which to store the scalar value.  If the
     *     method returns something other than JSON_DECODE_TYPE_VALUE, the
     *     method does not alter "value".
     * @return a value indicating what type of thing we read.  If we read a
     *     value, this is JSON_DECODE_TYPE_VALUE.  If we encountered an error,
     *     this is JSON_DECODE_TYPE_ERROR.  Otherwise, we read a separator, and
     *     this is a character indicating the separator that we read.
     */
    static char readValueOrSeparator(std::istream& input, JSONValue*& value);
public:
    /**
     * Returns the value encoded in the JSON string contained in the specified
     * input stream.  Returns NULL if the input stream does not contain a JSON-
     * encoded string.  (Returns NULL if there is something other than
     * whitespace at the end of the input, after a validly encoded JSON string.)
     */
    static JSONValue* decode(std::istream& input);
};

/**
 * A class for encoding a value using JSON.  The class assumes that its methods
 * are called in a valid sequence.  See the comments for a sense of the
 * limitations on method call sequences.
 */
class JSONEncoder {
private:
    /**
     * The output stream to which to append the encoded value.
     */
    std::ostream* output;
    /**
     * The number of levels of indentation at the beginning of new lines.
     */
    int indentationLevel;
    /**
     * Whether the last call to a JSONEncoder method was a call to startArray()
     * or startObject().
     */
    bool justStartedArrayOrObject;
    
    /**
     * Outputs the indentation for the beginning of a new line.
     */
    void outputIndentation();
    /**
     * Outputs the hexadecimal character indicated by the specified integer,
     * which is assumed to be between 0 and 15.
     */
    void outputHexChar(int value);
public:
    JSONEncoder(std::ostream& output2);
    /**
     * Outputs for the beginning of a new array.  To output an array, we do the
     * following:
     * 
     * startArray();
     * startArrayElement();
     * [method calls for element]
     * startArrayElement();
     * [method calls for element]
     * ...
     * startArrayElement();
     * [method calls for element]
     * endArray();
     */
    void startArray();
    /**
     * Outputs for the beginning of another array element.  See the comments for
     * startArray() for more information.
     */
    void startArrayElement();
    /**
     * Outputs for the end of an array.  See the comments for startArray() for
     * more information.
     */
    void endArray();
    /**
     * Outputs for the beginning of an object.  To output an object, we do the
     * following:
     * 
     * startObject();
     * appendArrayKey(key1);
     * [method calls for value]
     * appendArrayKey(key2);
     * [method calls for value]
     * ...
     * appendArrayKey(keyn);
     * [method calls for key]
     * endObject();
     */
    void startObject();
    /**
     * Outputs an object's key.
     */
    void appendObjectKey(std::string key);
    /**
     * Outputs for the end of an object.
     */
    void endObject();
    /**
     * Outputs the specified string value.
     */
    void appendStr(std::string value);
    /**
     * Outputs the specified integer value.
     */
    void appendInt(int value);
    /**
     * Outputs the specified boolean value.
     */
    void appendBool(bool value);
    /**
     * Outputs a null value.
     */
    void appendNull();
};

#endif
