#include <assert.h>
#include <sstream>
#include <vector>
#include "../JSONDecoder.hpp"
#include "../JSONEncoder.hpp"
#include "../JSONValue.hpp"
#include "JSONTest.hpp"

using namespace std;

void JSONTest::assertValuesEqual(JSONValue* expected, JSONValue* actual) {
    if (expected == NULL) {
        assertNull(actual, "Expected NULL but got a non-null value");
        return;
    }
    assertNotNull(actual, "Expected a non-null value but got NULL");
    assertEqual(expected->getType(), actual->getType(), "Different types");
    switch (expected->getType()) {
        case JSON_TYPE_ARRAY:
        {
            vector<JSONValue*> expectedArray = expected->getArrayValue();
            vector<JSONValue*> actualArray = actual->getArrayValue();
            assertEqual(
                expectedArray.size(),
                actualArray.size(),
                "Different array sizes");
            for (int i = 0; i < (int)expectedArray.size(); i++)
                assertValuesEqual(expectedArray.at(i), actualArray.at(i));
            break;
        }
        case JSON_TYPE_OBJECT:
        {
            map<string, JSONValue*> expectedObject = expected->getObjectValue();
            map<string, JSONValue*> actualObject = actual->getObjectValue();
            assertEqual(
                expectedObject.size(),
                actualObject.size(),
                "Different object sizes");
            for (map<string, JSONValue*>::const_iterator iterator =
                     expectedObject.begin();
                 iterator != expectedObject.end();
                 iterator++) {
                assertEqual(
                    1,
                    (int)actualObject.count(iterator->first),
                    "Different keys");
                assertValuesEqual(
                    iterator->second,
                    actualObject[iterator->first]);
            }
            break;
        }
        case JSON_TYPE_STR:
            assertEqual(
                expected->getStrValue(),
                actual->getStrValue(),
                "Different strings");
            break;
        case JSON_TYPE_NUMBER:
            assertEqual(
                expected->getDoubleValue(),
                actual->getDoubleValue(),
                "Different numbers");
            break;
        case JSON_TYPE_BOOL:
            assertEqual(
                expected->getBoolValue(),
                actual->getBoolValue(),
                "Different booleans");
            break;
        default:
            assert(!"Unhandled JSON type");
    }
}

void JSONTest::checkJSON(string str, JSONValue* expected) {
    istringstream input(str);
    JSONValue* decoded = JSONDecoder::decode(input);
    assertValuesEqual(expected, decoded);
    if (decoded != NULL) {
        delete decoded;
        ostringstream output;
        JSONEncoder encoder(output);
        encoder.appendValue(expected);
        encoder.endRoot();
        istringstream input2(output.str());
        decoded = JSONDecoder::decode(input2);
        assertValuesEqual(expected, decoded);
        delete decoded;
    }
    delete expected;
}

JSONValue* JSONTest::getArrayValue(JSONValue* element1, JSONValue* element2) {
    vector<JSONValue*> array;
    array.push_back(element1);
    array.push_back(element2);
    return new JSONValue(array);
}

JSONValue* JSONTest::getArrayValue(
    JSONValue* element1,
    JSONValue* element2,
    JSONValue* element3,
    JSONValue* element4) {
    vector<JSONValue*> array;
    array.push_back(element1);
    array.push_back(element2);
    array.push_back(element3);
    array.push_back(element4);
    return new JSONValue(array);
}

JSONValue* JSONTest::getObjectValue(
    string key1,
    JSONValue* value1,
    string key2,
    JSONValue* value2,
    string key3,
    JSONValue* value3) {
    map<string, JSONValue*> object;
    object[key1] = value1;
    object[key2] = value2;
    object[key3] = value3;
    return new JSONValue(object);
}

void JSONTest::testJSON() {
    checkJSON("", NULL);
    checkJSON("123", new JSONValue(123));
    checkJSON("false", new JSONValue(false));
    checkJSON("true", new JSONValue(true));
    checkJSON("[]", new JSONValue(vector<JSONValue*>()));
    checkJSON("{}", new JSONValue(map<string, JSONValue*>()));
    checkJSON("\"123\"", new JSONValue("123"));
    checkJSON(
        "\"12\\t3\\n4\\\\\\\"\\\\\"",
        new JSONValue("12\t3\n4\\\"\\"));
    checkJSON("[", NULL);
    checkJSON(
        "[123, 456, \"789\", false]",
        getArrayValue(
            new JSONValue(123),
            new JSONValue(456),
            new JSONValue("789"),
            new JSONValue(false)));
    checkJSON(
        "    [123,456,\"789\",  \r   \t\r\n\n\r\t  false ]       ",
        getArrayValue(
            new JSONValue(123),
            new JSONValue(456),
            new JSONValue("789"),
            new JSONValue(false)));
    checkJSON(
        "[123, 456, [\"789\", -3.14, 2.718e-8, 1.618e20], false]",
        getArrayValue(
            new JSONValue(123),
            new JSONValue(456),
            getArrayValue(
                new JSONValue("789"),
                new JSONValue(-3.14),
                new JSONValue(2.718e-8),
                new JSONValue(1.618e20)),
            new JSONValue(false)));
    checkJSON("[123, 456, \"789\", false, ]", NULL);
    checkJSON("[123, 456, \"789\", false] \"hey\"", NULL);
    checkJSON("[123, 456, \"789\", false], \"hey\"", NULL);
    checkJSON(
        "{\"abc\": 123, \"def\" : [456, false], \"true\": 1}",
        getObjectValue(
            "abc",
            new JSONValue(123),
            "def",
            getArrayValue(new JSONValue(456), new JSONValue(false)),
            "true",
            new JSONValue(1)));
    checkJSON("{\"abc\": 123, \"def\" : [456, false], 123: 1}", NULL);
    checkJSON("\"a\\u00abcd\"", new JSONValue("a\253cd"));
    checkJSON("\"a\\u00ab\"", new JSONValue("a\253"));
    
    istringstream input("null");
    assertNull(JSONDecoder::decode(input), "Decoding null failed");
    ostringstream output;
    JSONEncoder encoder(output);
    encoder.appendValue(NULL);
    encoder.endRoot();
    istringstream input2(output.str());
    assertNull(JSONDecoder::decode(input2), "Encoding null failed");
}

void JSONTest::testEncoder() {
    JSONValue* array = getArrayValue(new JSONValue("Foo"), NULL);
    ostringstream output;
    JSONEncoder encoder(output);
    encoder.startObject();
    encoder.appendObjectKey("bar");
    encoder.startArray();
    encoder.startArrayElement();
    encoder.appendDouble(1.5);
    encoder.startArrayElement();
    encoder.appendBool(true);
    encoder.startArrayElement();
    encoder.appendValue(array);
    encoder.startArrayElement();
    encoder.appendInt(5);
    encoder.endArray();
    encoder.appendObjectKey("baz");
    encoder.appendNull();
    encoder.appendObjectKey("abc");
    encoder.appendStr("def");
    encoder.endObject();
    encoder.endRoot();
    
    JSONValue* expected = getObjectValue(
        "bar",
        getArrayValue(
            new JSONValue(1.5),
            new JSONValue(true),
            array,
            new JSONValue(5)),
        "baz",
        NULL,
        "abc",
        new JSONValue("def"));
    istringstream input(output.str());
    JSONValue* actual = JSONDecoder::decode(input);
    assertValuesEqual(expected, actual);
    delete expected;
    delete actual;
}

string JSONTest::getName() {
    return "JSONTest";
}

void JSONTest::test() {
    testJSON();
    testEncoder();
}
