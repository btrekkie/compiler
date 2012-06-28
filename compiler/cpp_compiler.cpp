#include <assert.h>
#include <map>
#include <sstream>
#include "cpp_compiler.hpp"

using namespace std;

/**
 * A class for outputting a C++ source code representation of compiled source
 * code.
 */
/* We prepend prefixes to the identifiers of fields ("f_"), local variables
 * ("v_" and "e_"), and classes ("c_") to prevent ambiguity.  For example, we
 * don't want to have a method and a variable with the same identifier, or C++
 * will issue a compiler error if we reference this identifier.
 */
class CPPCompiler {
private:
    /**
     * The output stream to which we are appending C++ code.
     */
    ostream* output;
    /**
     * A map from non-field variables in the method we are currently compiling
     * to their corresponding C++ identifiers.
     */
    map<CFGOperand*, string> localVarIdentifiers;
    map<string, int> numLocalVarSuffixes;
    int numExpressionSuffixes;
    /**
     * A map from the labels we have encountered thus far to integers
     * identifying them.  Each integer indicates the number of elements that
     * were in "labelIndices" immediately before adding them to the map.
     */
    map<CFGLabel*, int> labelIndices;
    
    /**
     * Outputs the specified number of indentation strings.
     */
    void outputIndentation(int level) {
        for (int i = 0; i < level; i++)
            *output << "    ";
    }
    
    /**
     * Outputs the C++ type corresponding to the specified source code type.
     */
    void outputType(CFGType* type) {
        assert(type->numDimensions == 0 || !"TODO arrays");
        if (type->className == "Int")
            *output << "int";
        else if (type->className == "Long")
            *output << "long long";
        else if (type->className == "Float")
            *output << "float";
        else if (type->className == "Double")
            *output << "double";
        else if (type->className == "Bool")
            *output << "bool";
        else
            assert(!"TODO classes");
    }
    
    void createNewLocalVarDeclaration(CFGOperand* operand) {
        assert(
            (operand->isVar && !operand->isField) || !"Not a local variable");
        assert(
            localVarIdentifiers.count(operand) == 0 ||
            !"Variable is already declared");
        if (operand->identifier == "") {
            stringstream varIdentifier;
            varIdentifier << "e_" << numExpressionSuffixes;
            numExpressionSuffixes++;
            localVarIdentifiers[operand] = varIdentifier.str();
        } else {
            int numSuffixes;
            if (numLocalVarSuffixes.count(operand->identifier) == 0) {
                numSuffixes = 0;
                numLocalVarSuffixes[operand->identifier] = 1;
            } else {
                numSuffixes = numLocalVarSuffixes[operand->identifier];
                numLocalVarSuffixes[operand->identifier]++;
            }
            stringstream varIdentifier;
            varIdentifier << "v_" << operand->identifier << '_' << numSuffixes;
            localVarIdentifiers[operand] = varIdentifier.str();
        }
    }
    
    void outputVarDeclarationIfNecessary(CFGOperand* operand) {
        assert(operand->isVar || !"Not a variable");
        if (operand->isField || localVarIdentifiers.count(operand) > 0)
            return;
        createNewLocalVarDeclaration(operand);
        outputIndentation(1);
        outputType(operand->type);
        *output << ' ' << localVarIdentifiers[operand] << ";\n";
    }
    
    /**
     * Outputs the C++ code for the specified operand.
     */
    void outputOperand(CFGOperand* operand) {
        if (operand->isVar) {
            if (operand->isField)
                *output << "f_" << operand->identifier;
            else {
                assert(
                    localVarIdentifiers.count(operand) > 0 ||
                        !"Missing local variable declaration");
                *output << localVarIdentifiers[operand];
            }
        } else if (operand->type->className == "Int")
            *output << operand->intValue;
        else if (operand->type->className == "Bool") {
            if (operand->boolValue)
                *output << "true";
            else
                *output << "false";
        } else
            assert(!"TODO implement remaining primitives");
    }
    
    /**
     * Outputs the name used in the C++ code to refer to the specified label.
     */
    void outputLabelName(CFGLabel* label) {
        if (labelIndices.count(label) == 0)
            labelIndices[label] = (int)labelIndices.size();
        *output << "label" << labelIndices[label];
    }
    
    /**
     * Outputs the C++ code for the specified two-argument operation.
     */
    void outputBinaryOperation(CFGOperation operation) {
        switch (operation) {
            case CFG_BITWISE_AND:
                *output << '&';
                break;
            case CFG_BITWISE_OR:
                *output << '|';
                break;
            case CFG_DIV:
                *output << '/';
                break;
            case CFG_EQUALS:
                *output << "==";
                break;
            case CFG_GREATER_THAN:
                *output << '>';
                break;
            case CFG_GREATER_THAN_OR_EQUAL_TO:
                *output << ">=";
                break;
            case CFG_LEFT_SHIFT:
                *output << "<<";
                break;
            case CFG_LESS_THAN:
                *output << '<';
                break;
            case CFG_LESS_THAN_OR_EQUAL_TO:
                *output << "<=";
                break;
            case CFG_MINUS:
                *output << '-';
                break;
            case CFG_MOD:
                *output << '%';
                break;
            case CFG_MULT:
                *output << '*';
                break;
            case CFG_NOT_EQUALS:
                *output << "!=";
                break;
            case CFG_PLUS:
                *output << '+';
                break;
            case CFG_RIGHT_SHIFT:
                *output << ">>";
                break;
            case CFG_XOR:
                *output << '^';
                break;
            default:
                assert(!"Unhanded binary operation");
        }
    }
    
    /**
     * Outputs the C++ code for the specified one-argument operation.
     */
    void outputUnaryOperation(CFGOperation operation) {
        switch (operation) {
            case CFG_BITWISE_INVERT:
                *output << '~';
                break;
            case CFG_NEGATE:
                *output << '-';
                break;
            case CFG_NOT:
                *output << '!';
                break;
            default:
                assert(!"Unhandled unary operation");
        }
    }
    
    /**
     * Outputs the C++ code for the specified jump or conditional jump
     * statement, excluding the label name.
     */
    void outputJumpStatement(CFGStatement* statement) {
        switch (statement->op) {
            case CFG_IF:
                outputIndentation(1);
                *output << "if (";
                outputOperand(statement->arg1);
                *output << ")\n";
                outputIndentation(2);
                *output << "goto ";
                outputLabelName(statement->switchLabels.at(0));
                *output << ";\n";
                outputIndentation(1);
                *output << "else\n";
                outputIndentation(2);
                *output << "goto ";
                outputLabelName(statement->switchLabels.at(1));
                *output << ";\n";
                break;
            case CFG_JUMP:
                outputIndentation(1);
                *output << "goto ";
                outputLabelName(statement->switchLabels.at(0));
                *output << ";\n";
                break;
            case CFG_SWITCH:
                outputIndentation(1);
                *output << "switch (";
                outputOperand(statement->arg1);
                *output << ") {\n";
                for (int i = 0; i < (int)statement->switchValues.size(); i++) {
                    outputIndentation(1);
                    CFGOperand* value = statement->switchValues[i];
                    if (value == NULL)
                        *output << "default:\n";
                    else {
                        *output << "case ";
                        outputOperand(value);
                        *output << ":\n";
                    }
                    outputIndentation(1);
                    *output << "goto ";
                    outputLabelName(statement->switchLabels[i]);
                    *output << ";\n";
                }
                outputIndentation(1);
                *output << "}\n";
                break;
            default:
                assert(!"Unhandled jump statement");
        }
    }
    
    /**
     * Outputs the C++ code for the specified statement.
     */
    void outputStatement(CFGStatement* statement) {
        if (statement->label != NULL) {
            outputLabelName(statement->label);
            *output << ":;\n";
        }
        
        switch (statement->op) {
            case CFG_ASSIGN:
                outputIndentation(1);
                outputOperand(statement->destination);
                *output << " = ";
                outputOperand(statement->arg1);
                *output << ";\n";
                break;
            case CFG_BITWISE_INVERT:
            case CFG_NEGATE:
            case CFG_NOT:
                outputIndentation(1);
                outputOperand(statement->destination);
                *output << " = ";
                outputUnaryOperation(statement->op);
                outputOperand(statement->arg1);
                *output << ";\n";
                break;
            case CFG_IF:
            case CFG_JUMP:
            case CFG_SWITCH:
                outputJumpStatement(statement);
                break;
            case CFG_NOP:
                break;
            case CFG_UNSIGNED_RIGHT_SHIFT:
                outputIndentation(1);
                outputOperand(statement->destination);
                *output << " = (";
                outputType(statement->destination->type);
                *output << ")(((unsigned ";
                outputType(statement->destination->type);
                *output << ")";
                outputOperand(statement->arg1);
                *output << ") >> ";
                outputOperand(statement->arg2);
                *output << ");\n";
                break;
            default:
                outputIndentation(1);
                outputOperand(statement->destination);
                *output << " = ";
                outputOperand(statement->arg1);
                *output << ' ';
                outputBinaryOperation(statement->op);
                *output << ' ';
                outputOperand(statement->arg2);
                *output << ";\n";
                break;
        }
    }
    
    /**
     * Outputs the C++ code for the specified sequence of statements.
     */
    void outputStatements(vector<CFGStatement*>& statements) {
        for (vector<CFGStatement*>::const_iterator iterator =
                 statements.begin();
             iterator != statements.end();
             iterator++) {
            if ((*iterator)->destination != NULL)
                outputVarDeclarationIfNecessary((*iterator)->destination);
        }
        for (vector<CFGStatement*>::const_iterator iterator =
                 statements.begin();
             iterator != statements.end();
             iterator++)
            outputStatement(*iterator);
    }
    
    void outputMethodIdentifier(CFGMethod* method) {
        *output << "m_" << method->identifier;
    }
    
    /**
     * Outputs a class's C++ identier.
     * @param classIdentifier the class's identifier in the source file
     */
    void outputClassIdentifier(string classIdentifier) {
        *output << "c_" << classIdentifier;
    }
    
    /**
     * Outputs the specified method's prototype, excluding the semicolon.
     * @param method the method.
     * @param classIdentifier if we should also output the class name followed
     *     by two colons, the enclosing class's identifier in the source file.
     *     This is "" if we should not output the class name.
     * @param indentation the number of indentation strings to output at the
     *     beginning of the first line.
     */
    void outputMethodSignature(
        CFGMethod* method,
        string classIdentifier,
        int indentation) {
        
        localVarIdentifiers.clear();
        numLocalVarSuffixes.clear();
        labelIndices.clear();
        outputIndentation(indentation);
        if (method->returnVar == NULL)
            *output << "void ";
        else {
            outputType(method->returnVar->type);
            *output << ' ';
        }
        if (classIdentifier != "") {
            outputClassIdentifier(classIdentifier);
            *output << "::";
        }
        outputMethodIdentifier(method);
        *output << '(';
        if (!method->args.empty()) {
            for (vector<CFGOperand*>::const_iterator iterator =
                     method->args.begin();
                 iterator != method->args.end();
                 iterator++) {
                if (iterator != method->args.begin())
                    *output << ',';
                *output << '\n';
                outputIndentation(indentation + 1);
                outputType((*iterator)->type);
                *output << ' ';
                createNewLocalVarDeclaration(*iterator);
                outputOperand(*iterator);
            }
        }
        *output << ')';
    }
    
    /**
     * Outputs the C++ code declaring the specified class's fields.
     */
    void outputFieldDeclarations(CFGClass* clazz) {
        for (map<string, CFGOperand*>::const_iterator iterator =
                 clazz->fields.begin();
             iterator != clazz->fields.end();
             iterator++) {
            outputIndentation(1);
            outputType(iterator->second->type);
            *output << " ";
            outputOperand(iterator->second);
            *output << ";\n";
        }
    }
    
    /**
     * Outputs the C++ implementation of the specified method.
     * @param method the method.
     * @param classIdentifier the enclosing class's identifier in the source
     *     file.
     */
    void outputMethod(CFGMethod* method, string classIdentifier) {
        outputMethodSignature(method, classIdentifier, 0);
        *output << " {\n";
        if (method->returnVar != NULL) {
            createNewLocalVarDeclaration(method->returnVar);
            outputIndentation(1);
            outputType(method->returnVar->type);
            *output << ' ';
            outputOperand(method->returnVar);
            *output << ";\n";
        }
        outputStatements(method->statements);
        if (method->returnVar != NULL) {
            outputIndentation(1);
            *output << "return ";
            outputOperand(method->returnVar);
            *output << ";\n";
        }
        *output << "}\n";
    }
    
    /**
     * Outputs the C++ header file corresponding to the C++ source file
     * suggested by "outputClass".
     */
    void outputClassHeaderFile(CFGClass* clazz) {
        *output << "class ";
        outputClassIdentifier(clazz->identifier);
        *output << " {\n"
            << "public:\n";
        outputFieldDeclarations(clazz);
        vector<CFGMethod*> methods = clazz->getMethods();
        for (vector<CFGMethod*>::const_iterator iterator = methods.begin();
             iterator != methods.end();
             iterator++) {
            outputMethodSignature(*iterator, "", 1);
            *output << ";\n";
        }
        *output << "}\n";
    }
    
    /**
     * Appends a C++ source code representation of the specified class.
     */
    void outputClass(CFGClass* clazz) {
        vector<CFGMethod*> methods = clazz->getMethods();
        for (vector<CFGMethod*>::const_iterator iterator = methods.begin();
             iterator != methods.end();
             iterator++) {
            outputMethod(*iterator, clazz->identifier);
            *output << "\n";
        }
        *output << "void ";
        outputClassIdentifier(clazz->identifier);
        *output << "::init() {\n";
        outputStatements(clazz->initStatements);
        *output << "}\n";
    }
public:
    CPPCompiler() {
        numExpressionSuffixes = 0;
    }
    
    void outputHeaderFile(CFGFile* file, ostream& output2) {
        output = &output2;
        outputClassHeaderFile(file->clazz);
    }
    
    void outputImplementationFile(CFGFile* file, ostream& output2) {
        output = &output2;
        outputClass(file->clazz);
    }
};

void outputCPPHeaderFile(CFGFile* file, ostream& output) {
    CPPCompiler compiler;
    compiler.outputHeaderFile(file, output);
}

void outputCPPImplementationFile(CFGFile* file, ostream& output) {
    CPPCompiler compiler;
    compiler.outputImplementationFile(file, output);
}
