#ifndef INTERFACE_OUTPUT_HPP_INCLUDED
#define INTERFACE_OUTPUT_HPP_INCLUDED

#include <iostream>

class ClassInterface;
class FieldInterface;
class JSONEncoder;
class MethodInterface;

/**
 * Class for serializing a ClassInterface to an ostream.
 * TODO add InterfaceInput
 */
/* A ClassInterface is serialized using JSON.
 */
class InterfaceOutput {
private:
    /**
     * The JSONEncoder being used to serialize the ClassInterface.
     */
    JSONEncoder* output;
    
    void outputFieldInterface(FieldInterface* interface);
    void outputMethodInterface(MethodInterface* interface);
public:
    InterfaceOutput(std::ostream& output2);
    ~InterfaceOutput();
    void outputClassInterface(ClassInterface* interface);
};

#endif
