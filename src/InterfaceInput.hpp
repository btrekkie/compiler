#ifndef INTERFACE_INPUT_HPP_INCLUDED
#define INTERFACE_INPUT_HPP_INCLUDED

#include <iostream>
#include <vector>

class ClassInterface;
class FieldInterface;
class JSONValue;
class MethodInterface;

/**
 * Class for deserializing a ClassInterface from an istream.  The ClassInterface
 * should have been serialized using InterfaceOutput.
 */
class InterfaceInput {
private:
    /**
     * Returns the FieldInterface represented by the specified JSON value.
     */
    static FieldInterface* readFieldInterface(JSONValue* value);
    /**
     * Returns the MethodInterface represented by the specified JSON value.
     */
    static MethodInterface* readMethodInterface(JSONValue* value);
public:
    /**
     * Reads a serialized ClassInterface from the specified wistream.  Returns
     * NULL if the input stream does not contain a serialized ClassInterface.
     * Returns NULL if there is something else at the end of the input, after a
     * validly encoded ClassInterface.
     */
    static ClassInterface* readClassInterface(std::wistream& input);
};

#endif
