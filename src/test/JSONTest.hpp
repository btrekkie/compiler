#ifndef JSON_TEST_HPP_INCLUDED
#define JSON_TEST_HPP_INCLUDED

#include <string>
#include "TestCase.hpp"

class JSONValue;

/**
 * Unit test for JSONDecoder and JSONEncoder.
 */
class JSONTest : public TestCase {
private:
    /**
     * Asserts that the specified JSON values are equal.
     */
    void assertValuesEqual(JSONValue* expected, JSONValue* actual);
    /**
     * Performs some assertions pertaining to encoding and decoding JSON, then
     * deallocates "expected".
     * @param str a string.
     * @param expected the value for which "str" is a JSON encoding.  This is
     *     NULL if "str" is not a valid JSON encoding.
     */
    void checkJSON(std::wstring str, JSONValue* expected);
    /**
     * Returns a JSON array value with the specified elements.
     */
    JSONValue* getArrayValue(JSONValue* element1, JSONValue* element2);
    /**
     * Returns a JSON array value with the specified elements.
     */
    JSONValue* getArrayValue(
        JSONValue* element1,
        JSONValue* element2,
        JSONValue* element3,
        JSONValue* element4);
    /**
     * Returns a JSON object value with the specifier key-value pairs.
     */
    JSONValue* getObjectValue(
        std::wstring key1,
        JSONValue* value1,
        std::wstring key2,
        JSONValue* value2,
        std::wstring key3,
        JSONValue* value3);
    /**
     * Tests JSONDecoder and JSONEncoder.appendValue.
     */
    void testJSON();
    /**
     * Tests the JSONEncoder methods.
     */
    void testEncoder();
public:
    std::wstring getName();
    void test();
};

#endif
