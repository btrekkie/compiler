#ifndef JSON_ENCODER_HPP_INCLUDED
#define JSON_ENCODER_HPP_INCLUDED

#include <iostream>
#include <string>

class JSONValue;

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
     * Outputs for the end of the root objects.  This should be the last method
     * called on a given encoder.
     */
    void endRoot();
    /**
     * Outputs the specified string value.
     */
    void appendStr(std::string value);
    /**
     * Outputs the specified double value.
     */
    void appendDouble(double value);
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
    /**
     * Outputs the specified value.
     */
    void appendValue(JSONValue* value);
};

#endif
