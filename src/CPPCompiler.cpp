#include <assert.h>
#include <map>
#include <sstream>
#include "CFG.hpp"
#include "CPPCompiler.hpp"
#include "Interface.hpp"

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
        assert(type->getNumDimensions() == 0 || !"TODO arrays");
        if (type->getClassName() == "Int")
            *output << "int";
        else if (type->getClassName() == "Long")
            *output << "long long";
        else if (type->getClassName() == "Float")
            *output << "float";
        else if (type->getClassName() == "Double")
            *output << "double";
        else if (type->getClassName() == "Bool")
            *output << "bool";
        else
            assert(!"TODO classes");
    }
    
    void createNewLocalVarDeclaration(CFGOperand* operand) {
        assert(
            (operand->getIsVar() && !operand->getIsField()) ||
                !"Not a local variable");
        assert(
            localVarIdentifiers.count(operand) == 0 ||
            !"Variable is already declared");
        if (operand->getIdentifier() == "") {
            ostringstream varIdentifier;
            varIdentifier << "e_" << numExpressionSuffixes;
            numExpressionSuffixes++;
            localVarIdentifiers[operand] = varIdentifier.str();
        } else {
            int numSuffixes;
            if (numLocalVarSuffixes.count(operand->getIdentifier()) == 0) {
                numSuffixes = 0;
                numLocalVarSuffixes[operand->getIdentifier()] = 1;
            } else {
                numSuffixes = numLocalVarSuffixes[operand->getIdentifier()];
                numLocalVarSuffixes[operand->getIdentifier()]++;
            }
            ostringstream varIdentifier;
            varIdentifier << "v_" << operand->getIdentifier() << '_' <<
                numSuffixes;
            localVarIdentifiers[operand] = varIdentifier.str();
        }
    }
    
    void outputVarDeclarationIfNecessary(CFGOperand* operand) {
        assert(operand->getIsVar() || !"Not a variable");
        if (operand->getIsField() || localVarIdentifiers.count(operand) > 0)
            return;
        createNewLocalVarDeclaration(operand);
        outputIndentation(1);
        outputType(operand->getType());
        *output << ' ' << localVarIdentifiers[operand] << ";\n";
    }
    
    /**
     * Outputs the C++ code for the specified operand.
     */
    void outputOperand(CFGOperand* operand) {
        if (operand->getIsVar()) {
            if (operand->getIsField())
                *output << "f_" << operand->getIdentifier();
            else {
                assert(
                    localVarIdentifiers.count(operand) > 0 ||
                        !"Missing local variable declaration");
                *output << localVarIdentifiers[operand];
            }
        } else if (operand->getType()->getClassName() == "Int")
            *output << operand->getIntValue();
        else if (operand->getType()->getClassName() == "Bool") {
            if (operand->getBoolValue())
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
        switch (statement->getOperation()) {
            case CFG_IF:
                outputIndentation(1);
                *output << "if (";
                outputOperand(statement->getArg1());
                *output << ")\n";
                outputIndentation(2);
                *output << "goto ";
                outputLabelName(statement->getSwitchLabel(0));
                *output << ";\n";
                outputIndentation(1);
                *output << "else\n";
                outputIndentation(2);
                *output << "goto ";
                outputLabelName(statement->getSwitchLabel(1));
                *output << ";\n";
                break;
            case CFG_JUMP:
                outputIndentation(1);
                *output << "goto ";
                outputLabelName(statement->getSwitchLabel(0));
                *output << ";\n";
                break;
            case CFG_SWITCH:
                outputIndentation(1);
                *output << "switch (";
                outputOperand(statement->getArg1());
                *output << ") {\n";
                for (int i = 0; i < statement->getNumSwitchLabels(); i++) {
                    outputIndentation(1);
                    CFGOperand* value = statement->getSwitchValue(i);
                    if (value == NULL)
                        *output << "default:\n";
                    else {
                        *output << "case ";
                        outputOperand(value);
                        *output << ":\n";
                    }
                    outputIndentation(1);
                    *output << "goto ";
                    outputLabelName(statement->getSwitchLabel(i));
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
     * Outputs a method's C++ identier.
     * @param methodIdentifier the class's identifier in the source file
     */
    void outputMethodIdentifier(string methodIdentifier) {
        *output << "m_" << methodIdentifier;
    }
    
    /**
     * Outputs a class's C++ identier.
     * @param classIdentifier the class's identifier in the source file
     */
    void outputClassIdentifier(string classIdentifier) {
        *output << "c_" << classIdentifier;
    }
    
    /**
     * Outputs the C++ code for the specified method call statement, excluding
     * the label name.
     */
    void outputMethodCall(CFGStatement* statement) {
        assert(
            statement->getOperation() == CFG_METHOD_CALL ||
            !"Not a method call");
        outputIndentation(1);
        vector<CFGOperand*> args = statement->getMethodArgs();
        // TODO eventually, "print" and "println" should be real methods
        if (statement->getMethodIdentifier() == "print") {
            *output << "cout << ";
            outputOperand(args.at(0));
            *output << ";\n";
        } else if (statement->getMethodIdentifier() == "println") {
            *output << "cout << ";
            outputOperand(args.at(0));
            *output << " << '\\n';\n";
        } else {
            if (statement->getDestination() != NULL) {
                outputOperand(statement->getDestination());
                *output << " = ";
            }
            outputMethodIdentifier(statement->getMethodIdentifier());
            *output << '(';
            for (vector<CFGOperand*>::const_iterator iterator = args.begin();
                 iterator != args.end();
                 iterator++) {
                if (iterator != args.begin())
                    *output << ", ";
                outputOperand(*iterator);
            }
            *output << ");\n";
        }
    }
    
    /**
     * Outputs the C++ code for the specified statement, exculding the label
     * name.
     */
    void outputStatement(CFGStatement* statement) {
        switch (statement->getOperation()) {
            case CFG_ASSIGN:
                outputIndentation(1);
                outputOperand(statement->getDestination());
                *output << " = ";
                outputOperand(statement->getArg1());
                *output << ";\n";
                break;
            case CFG_BITWISE_INVERT:
            case CFG_NEGATE:
            case CFG_NOT:
                outputIndentation(1);
                outputOperand(statement->getDestination());
                *output << " = ";
                outputUnaryOperation(statement->getOperation());
                outputOperand(statement->getArg1());
                *output << ";\n";
                break;
            case CFG_IF:
            case CFG_JUMP:
            case CFG_SWITCH:
                outputJumpStatement(statement);
                break;
            case CFG_METHOD_CALL:
                outputMethodCall(statement);
                break;
            case CFG_NOP:
                break;
            case CFG_UNSIGNED_RIGHT_SHIFT:
                outputIndentation(1);
                outputOperand(statement->getDestination());
                *output << " = (";
                outputType(statement->getDestination()->getType());
                *output << ")(((unsigned ";
                outputType(statement->getDestination()->getType());
                *output << ")";
                outputOperand(statement->getArg1());
                *output << ") >> ";
                outputOperand(statement->getArg2());
                *output << ");\n";
                break;
            default:
                outputIndentation(1);
                outputOperand(statement->getDestination());
                *output << " = ";
                outputOperand(statement->getArg1());
                *output << ' ';
                outputBinaryOperation(statement->getOperation());
                *output << ' ';
                outputOperand(statement->getArg2());
                *output << ";\n";
                break;
        }
    }
    
    /**
     * Outputs the C++ code for the specified sequence of statements.
     */
    void outputStatements(vector<CFGStatement*> statements) {
        set<CFGLabel*> usedLabels;
        for (vector<CFGStatement*>::const_iterator iterator =
                 statements.begin();
             iterator != statements.end();
             iterator++) {
            CFGStatement* statement = *iterator;
            if (statement->getDestination() != NULL)
                outputVarDeclarationIfNecessary(statement->getDestination());
            for (int i = 0; i < statement->getNumSwitchLabels(); i++)
                usedLabels.insert(statement->getSwitchLabel(i));
        }
        for (vector<CFGStatement*>::const_iterator iterator =
                 statements.begin();
             iterator != statements.end();
             iterator++) {
            CFGStatement* statement = *iterator;
            if (usedLabels.count(statement->getLabel()) > 0) {
                outputLabelName(statement->getLabel());
                *output << ":;\n";
            }
            outputStatement(statement);
        }
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
        numExpressionSuffixes = 0;
        outputIndentation(indentation);
        if (method->getReturnVar() == NULL)
            *output << "void ";
        else {
            outputType(method->getReturnVar()->getType());
            *output << ' ';
        }
        if (classIdentifier != "") {
            outputClassIdentifier(classIdentifier);
            *output << "::";
        }
        outputMethodIdentifier(method->getIdentifier());
        *output << '(';
        vector<CFGOperand*> args = method->getArgs();
        if (!args.empty()) {
            for (vector<CFGOperand*>::const_iterator iterator = args.begin();
                 iterator != args.end();
                 iterator++) {
                if (iterator != args.begin())
                    *output << ',';
                *output << '\n';
                outputIndentation(indentation + 1);
                outputType((*iterator)->getType());
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
        map<string, CFGOperand*> fields = clazz->getFields();
        for (map<string, CFGOperand*>::const_iterator iterator = fields.begin();
             iterator != fields.end();
             iterator++) {
            outputIndentation(1);
            outputType(iterator->second->getType());
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
        if (method->getReturnVar() != NULL) {
            createNewLocalVarDeclaration(method->getReturnVar());
            outputIndentation(1);
            outputType(method->getReturnVar()->getType());
            *output << ' ';
            outputOperand(method->getReturnVar());
            *output << ";\n";
        }
        outputStatements(method->getStatements());
        if (method->getReturnVar() != NULL) {
            outputIndentation(1);
            *output << "return ";
            outputOperand(method->getReturnVar());
            *output << ";\n";
        }
        *output << "}\n";
    }
    
    /**
     * Outputs the C++ header file corresponding to the C++ source file
     * suggested by "outputClass".
     */
    void outputClassHeaderFile(CFGClass* clazz) {
        *output << "#ifndef COMPILER_" << clazz->getIdentifier() <<
            "_DEFINED\n" <<
            "#define COMPILER_" << clazz->getIdentifier() << "_DEFINED\n\n" <<
            "class ";
        outputClassIdentifier(clazz->getIdentifier());
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
        outputIndentation(1);
        *output << "void init();\n" <<
            "};\n\n" <<
            "#endif\n";
    }
    
    /**
     * Appends a C++ source code representation of the specified class.
     */
    void outputClass(CFGClass* clazz) {
        *output << "#include <iostream>\n" <<
            "#include \"" << clazz->getIdentifier() << ".hpp\"\n\n" <<
            "using namespace std;\n\n";
        vector<CFGMethod*> methods = clazz->getMethods();
        for (vector<CFGMethod*>::const_iterator iterator = methods.begin();
             iterator != methods.end();
             iterator++) {
            outputMethod(*iterator, clazz->getIdentifier());
            *output << "\n";
        }
        *output << "void ";
        outputClassIdentifier(clazz->getIdentifier());
        *output << "::init() {\n";
        outputStatements(clazz->getInitStatements());
        *output << "}\n";
    }
public:
    CPPCompiler() {
        numExpressionSuffixes = 0;
    }
    
    void outputHeaderFile(CFGFile* file, ostream& output2) {
        output = &output2;
        outputClassHeaderFile(file->getClass());
    }
    
    void outputImplementationFile(CFGFile* file, ostream& output2) {
        output = &output2;
        outputClass(file->getClass());
    }
    
    void outputMainFile(
        string className,
        string mainMethodName,
        ostream& output2) {
        output = &output2;
        *output << "#include \"" << className << ".hpp\"\n\n" <<
            "int main() {\n";
        outputIndentation(1);
        outputClassIdentifier(className);
        *output << " mainObject;\n";
        outputIndentation(1);
        *output << "mainObject.";
        outputMethodIdentifier(mainMethodName);
        *output << "();\n";
        outputIndentation(1);
        *output << "return 0;\n" << "}\n";
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

void outputMainFile(string className, string mainMethodName, ostream& output) {
    CPPCompiler compiler;
    compiler.outputMainFile(className, mainMethodName, output);
}
