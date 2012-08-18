#ifndef JSON_VALUE_HPP_INCLUDED
#define JSON_VALUE_HPP_INCLUDED

#include <map>
#include <string>
#include <vector>

enum JSONValueType {
    JSON_TYPE_ARRAY,
    JSON_TYPE_OBJECT,
    JSON_TYPE_STR,
    JSON_TYPE_NUMBER,
    JSON_TYPE_BOOL
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
    std::map<std::wstring, JSONValue*>* objectValue;
    /**
     * The value of this, if this is of the JSON string type.
     */
    std::wstring* strValue;
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
    explicit JSONValue(std::map<std::wstring, JSONValue*> objectValue2);
    explicit JSONValue(double numberValue2);
    explicit JSONValue(std::wstring strValue2);
    explicit JSONValue(const wchar_t* strValue2);
    explicit JSONValue(int numberValue2);
    explicit JSONValue(bool boolValue2);
    ~JSONValue();
    JSONValueType getType();
    std::vector<JSONValue*> getArrayValue();
    std::map<std::wstring, JSONValue*> getObjectValue();
    std::wstring getStrValue();
    double getDoubleValue();
    int getIntValue();
    bool getBoolValue();
    /**
     * Returns the field value associated with the specified key.  Assumes that
     * this is a value of type JSON_TYPE_OBJECT.
     */
    JSONValue* getField(std::wstring key);
};

#endif
