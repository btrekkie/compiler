#ifndef JSON_DECODER_HPP_INCLUDED
#define JSON_DECODER_HPP_INCLUDED

#include <iostream>
#include <map>
#include <string>
#include <vector>

class JSONValue;

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
    static void deleteMap(std::map<std::wstring, JSONValue*> value);
    /**
     * Returns whether the specified string is a JSON-encoded number.
     */
    static bool isNumber(std::wstring str);
    /**
     * Returns whether the specified character is a JSON whitespace character.
     */
    static bool isWhitespace(wchar_t c);
    /**
     * Converts the specified hexadecimal character to an integer.
     * @param c the character to convert.
     * @param value a reference in which to store the results.
     * @return whether the character is a valid hexadecimal character.
     */
    static bool hexDigitToInt(wchar_t c, int& value);
    /**
     * Reads a string value from a JSON-encoded string, sans the leading
     * quotation mark.
     * @param input the input stream from which to read.
     * @param value a reference in which to store the string that this method
     *     read.
     * @return whether the input stream contained a valid JSON-encoded string.
     */
    static bool readStr(std::wistream& input, std::wstring& value);
    /**
     * Non-recursive method that reads a JSON scalar value or a "separator" like
     * L'{' or L','.
     * @param input the input stream from which to read.
     * @param value a reference in which to store the scalar value.  If the
     *     method returns something other than JSON_DECODE_TYPE_VALUE, the
     *     method does not alter "value".
     * @return a value indicating what type of thing we read.  See the comments
     *     for the return value of "readValue" for more information.
     */
    static wchar_t readScalarOrSeparator(
        std::wistream& input,
        JSONValue*& value);
    /**
     * Recursive method that reads a JSON value or a "separator" like L'{' or
     * L','.
     * @param input the input stream from which to read.
     * @param value a reference in which to store the scalar value.  If the
     *     method returns something other than JSON_DECODE_TYPE_VALUE, the
     *     method does not alter "value".
     * @return a value indicating what type of thing we read.  If we read a
     *     value, this is JSON_DECODE_TYPE_VALUE.  If we encountered an error,
     *     this is JSON_DECODE_TYPE_ERROR.  Otherwise, we read a separator, and
     *     this is a character indicating the separator that we read.
     */
    static wchar_t readValueOrSeparator(
        std::wistream& input,
        JSONValue*& value);
public:
    /**
     * Returns the value encoded in the JSON string contained in the specified
     * input stream.  Returns NULL if the input stream does not contain a JSON-
     * encoded string.  Returns NULL if there is something other than whitespace
     * at the end of the input, after a validly encoded JSON string.
     */
    static JSONValue* decode(std::wistream& input);
};

#endif
