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
        assertNull(actual, L"Expected NULL but got a non-null value");
        return;
    }
    assertNotNull(actual, L"Expected a non-null value but got NULL");
    assertEqual(expected->getType(), actual->getType(), L"Different types");
    switch (expected->getType()) {
        case JSON_TYPE_ARRAY:
        {
            vector<JSONValue*> expectedArray = expected->getArrayValue();
            vector<JSONValue*> actualArray = actual->getArrayValue();
            assertEqual(
                expectedArray.size(),
                actualArray.size(),
                L"Different array sizes");
            for (int i = 0; i < (int)expectedArray.size(); i++)
                assertValuesEqual(expectedArray.at(i), actualArray.at(i));
            break;
        }
        case JSON_TYPE_OBJECT:
        {
            map<wstring, JSONValue*> expectedObject =
                expected->getObjectValue();
            map<wstring, JSONValue*> actualObject = actual->getObjectValue();
            assertEqual(
                expectedObject.size(),
                actualObject.size(),
                L"Different object sizes");
            for (map<wstring, JSONValue*>::const_iterator iterator =
                     expectedObject.begin();
                 iterator != expectedObject.end();
                 iterator++) {
                assertEqual(
                    1,
                    (int)actualObject.count(iterator->first),
                    L"Different keys");
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
                L"Different strings");
            break;
        case JSON_TYPE_NUMBER:
            assertEqual(
                expected->getDoubleValue(),
                actual->getDoubleValue(),
                L"Different numbers");
            break;
        case JSON_TYPE_BOOL:
            assertEqual(
                expected->getBoolValue(),
                actual->getBoolValue(),
                L"Different booleans");
            break;
        default:
            assert(!L"Unhandled JSON type");
    }
}

void JSONTest::checkJSON(wstring str, JSONValue* expected) {
    wistringstream input(str);
    JSONValue* decoded = JSONDecoder::decode(input);
    assertValuesEqual(expected, decoded);
    if (decoded != NULL) {
        delete decoded;
        wostringstream output;
        JSONEncoder encoder(output);
        encoder.appendValue(expected);
        encoder.endRoot();
        wistringstream input2(output.str());
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
    wstring key1,
    JSONValue* value1,
    wstring key2,
    JSONValue* value2,
    wstring key3,
    JSONValue* value3) {
    map<wstring, JSONValue*> object;
    object[key1] = value1;
    object[key2] = value2;
    object[key3] = value3;
    return new JSONValue(object);
}

void JSONTest::testJSON() {
    checkJSON(L"", NULL);
    checkJSON(L"123", new JSONValue(123));
    checkJSON(L"false", new JSONValue(false));
    checkJSON(L"true", new JSONValue(true));
    checkJSON(L"[]", new JSONValue(vector<JSONValue*>()));
    checkJSON(L"{}", new JSONValue(map<wstring, JSONValue*>()));
    checkJSON(L"\"123\"", new JSONValue(L"123"));
    checkJSON(
        L"\"12\\t3\\n4\\\\\\\"\\\\\"",
        new JSONValue(L"12\t3\n4\\\"\\"));
    checkJSON(L"[", NULL);
    checkJSON(
        L"[123, 456, \"789\", false]",
        getArrayValue(
            new JSONValue(123),
            new JSONValue(456),
            new JSONValue(L"789"),
            new JSONValue(false)));
    checkJSON(
        L"    [123,456,\"789\",  \r   \t\r\n\n\r\t  false ]       ",
        getArrayValue(
            new JSONValue(123),
            new JSONValue(456),
            new JSONValue(L"789"),
            new JSONValue(false)));
    checkJSON(
        L"[123, 456, [\"789\", -3.14, 2.718e-8, 1.618e20], false]",
        getArrayValue(
            new JSONValue(123),
            new JSONValue(456),
            getArrayValue(
                new JSONValue(L"789"),
                new JSONValue(-3.14),
                new JSONValue(2.718e-8),
                new JSONValue(1.618e20)),
            new JSONValue(false)));
    checkJSON(L"[123, 456, \"789\", false, ]", NULL);
    checkJSON(L"[123, 456, \"789\", false] \"hey\"", NULL);
    checkJSON(L"[123, 456, \"789\", false], \"hey\"", NULL);
    checkJSON(
        L"{\"abc\": 123, \"def\" : [456, false], \"true\": 1}",
        getObjectValue(
            L"abc",
            new JSONValue(123),
            L"def",
            getArrayValue(new JSONValue(456), new JSONValue(false)),
            L"true",
            new JSONValue(1)));
    checkJSON(L"{\"abc\": 123, \"def\" : [456, false], 123: 1}", NULL);
    checkJSON(L"\"a\\u12abcd\"", new JSONValue(L"a\u12ab" L"cd"));
    checkJSON(L"\"a\\u12ab\"", new JSONValue(L"a\u12ab"));
    checkJSON(L"\"a\\u00abcd\"", new JSONValue(L"a\253cd"));
    checkJSON(L"\"a\\u00ab\"", new JSONValue(L"a\253"));
    
    wistringstream input(L"null");
    assertNull(JSONDecoder::decode(input), L"Decoding null failed");
    wostringstream output;
    JSONEncoder encoder(output);
    encoder.appendValue(NULL);
    encoder.endRoot();
    wistringstream input2(output.str());
    assertNull(JSONDecoder::decode(input2), L"Encoding null failed");
}

void JSONTest::testEncoder() {
    JSONValue* array = getArrayValue(new JSONValue(L"Foo"), NULL);
    wostringstream output;
    JSONEncoder encoder(output);
    encoder.startObject();
    encoder.appendObjectKey(L"bar");
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
    encoder.appendObjectKey(L"baz");
    encoder.appendNull();
    encoder.appendObjectKey(L"abc");
    encoder.appendStr(L"def");
    encoder.endObject();
    encoder.endRoot();
    
    JSONValue* expected = getObjectValue(
        L"bar",
        getArrayValue(
            new JSONValue(1.5),
            new JSONValue(true),
            array,
            new JSONValue(5)),
        L"baz",
        NULL,
        L"abc",
        new JSONValue(L"def"));
    wistringstream input(output.str());
    JSONValue* actual = JSONDecoder::decode(input);
    assertValuesEqual(expected, actual);
    delete expected;
    delete actual;
}

wstring JSONTest::getName() {
    return L"JSONTest";
}

void JSONTest::test() {
    testJSON();
    testEncoder();
}
