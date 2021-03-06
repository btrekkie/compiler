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
/* We prepend prefixes to the identifiers of fields L"f_"), local variables
 * ("v_" and "e_"), and classes ("c_") to prevent ambiguity.  For example, we
 * don't want to have a method and a variable with the same identifier, or C++
 * will issue a compiler error if we reference this identifier.
 */
class CPPCompiler {
private:
    /**
     * The output stream to which we are appending C++ code.
     */
    wostream* output;
    /**
     * A map from non-field variables in the method we are currently compiling
     * to their corresponding C++ identifiers.
     */
    map<CFGOperand*, wstring> localVarIdentifiers;
    /**
     * A map from the source file identifiers of non-field variables we have
     * encountered thus far to the number of such variables we have encountered.
     * (Because of scoping rules, multiple variables can have the same
     * identifier.)
     */
    map<wstring, int> numLocalVarSuffixes;
    /**
     * The number of temporary variables we have encountered thus far.  A
     * temporary variable is a variable that does not appear in the source file,
     * but rather is an intermediate variable for an expression.
     */
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
            *output << L"    ";
    }
    
    /**
     * Outputs the C++ type corresponding to the specified reduced source code
     * type.
     */
    void outputType(CFGReducedType type) {
        switch (type) {
            case REDUCED_TYPE_BOOL:
                *output << L"bool";
                break;
            case REDUCED_TYPE_BYTE:
                *output << L"char";
                break;
            case REDUCED_TYPE_INT:
                *output << L"int";
                break;
            case REDUCED_TYPE_LONG:
                *output << L"long long";
                break;
            case REDUCED_TYPE_FLOAT:
                *output << L"float";
                break;
            case REDUCED_TYPE_DOUBLE:
                *output << L"double";
                break;
            default:
                assert(!L"TODO classes");
        }
    }
    
    /**
     * Adds the specified local variable operand to the "localVarIdentifiers"
     * map.  Assumes that it is not already present.
     */
    void createNewLocalVarDeclaration(CFGOperand* operand) {
        assert(
            (operand->getIsVar() && !operand->getIsField()) ||
                !L"Not a local variable");
        assert(
            localVarIdentifiers.count(operand) == 0 ||
            !L"Variable is already declared");
        if (operand->getIdentifier() == L"") {
            wostringstream varIdentifier;
            varIdentifier << L"e_" << numExpressionSuffixes;
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
            wostringstream varIdentifier;
            varIdentifier << L"v_" << operand->getIdentifier() << L'_' <<
                numSuffixes;
            localVarIdentifiers[operand] = varIdentifier.str();
        }
    }
    
    /**
     * Outputs C++ code declaring the specified CFGOperand local variable.
     */
    void outputVarDeclarationIfNecessary(CFGOperand* operand) {
        assert(operand->getIsVar() || !L"Not a variable");
        if (operand->getIsField() || localVarIdentifiers.count(operand) > 0)
            return;
        createNewLocalVarDeclaration(operand);
        outputIndentation(1);
        outputType(operand->getType());
        *output << L' ' << localVarIdentifiers[operand] << L";\n";
    }
    
    /**
     * Outputs the C++ code for the specified operand.
     */
    void outputOperand(CFGOperand* operand) {
        if (operand->getIsVar()) {
            if (operand->getIsField())
                *output << L"f_" << operand->getIdentifier();
            else {
                assert(
                    localVarIdentifiers.count(operand) > 0 ||
                        !L"Missing local variable declaration");
                *output << localVarIdentifiers[operand];
            }
        } else if (operand->getType() == REDUCED_TYPE_INT)
            *output << operand->getIntValue();
        else if (operand->getType() == REDUCED_TYPE_LONG)
            *output << operand->getLongValue() << L"ll";
        else if (operand->getType() == REDUCED_TYPE_DOUBLE)
            *output << operand->getDoubleValue();
        else if (operand->getType() == REDUCED_TYPE_BOOL) {
            if (operand->getBoolValue())
                *output << L"true";
            else
                *output << L"false";
        } else if (operand->getType() == REDUCED_TYPE_FLOAT) {
            wostringstream str;
            str << operand->getFloatValue();
            *output << str.str();
            if (str.str().find(L'.') == wstring::npos &&
                str.str().find(L'e') == wstring::npos)
                *output << L'.';
            *output << L'f';
        } else
            assert(!L"TODO (classes) null values");
    }
    
    /**
     * Outputs the name used in the C++ code to refer to the specified label.
     */
    void outputLabelName(CFGLabel* label) {
        if (labelIndices.count(label) == 0)
            labelIndices[label] = (int)labelIndices.size();
        *output << L"label" << labelIndices[label];
    }
    
    /**
     * Outputs the C++ code for the specified two-argument operation.
     */
    void outputBinaryOperation(CFGOperation operation) {
        switch (operation) {
            case CFG_BITWISE_AND:
                *output << L'&';
                break;
            case CFG_BITWISE_OR:
                *output << L'|';
                break;
            case CFG_DIV:
                *output << L'/';
                break;
            case CFG_EQUALS:
                *output << L"==";
                break;
            case CFG_GREATER_THAN:
                *output << L'>';
                break;
            case CFG_GREATER_THAN_OR_EQUAL_TO:
                *output << L">=";
                break;
            case CFG_LEFT_SHIFT:
                *output << L"<<";
                break;
            case CFG_LESS_THAN:
                *output << L'<';
                break;
            case CFG_LESS_THAN_OR_EQUAL_TO:
                *output << L"<=";
                break;
            case CFG_MINUS:
                *output << L'-';
                break;
            case CFG_MOD:
                *output << L'%';
                break;
            case CFG_MULT:
                *output << L'*';
                break;
            case CFG_NOT_EQUALS:
                *output << L"!=";
                break;
            case CFG_PLUS:
                *output << L'+';
                break;
            case CFG_RIGHT_SHIFT:
                *output << L">>";
                break;
            case CFG_XOR:
                *output << L'^';
                break;
            default:
                assert(!L"Unhanded binary operation");
        }
    }
    
    /**
     * Outputs the C++ code for the specified one-argument operation.
     */
    void outputUnaryOperation(CFGOperation operation) {
        switch (operation) {
            case CFG_BITWISE_INVERT:
                *output << L'~';
                break;
            case CFG_NEGATE:
                *output << L'-';
                break;
            case CFG_NOT:
                *output << L'!';
                break;
            default:
                assert(!L"Unhandled unary operation");
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
                *output << L"if (";
                outputOperand(statement->getArg1());
                *output << L")\n";
                outputIndentation(2);
                *output << L"goto ";
                outputLabelName(statement->getSwitchLabel(0));
                *output << L";\n";
                outputIndentation(1);
                *output << L"else\n";
                outputIndentation(2);
                *output << L"goto ";
                outputLabelName(statement->getSwitchLabel(1));
                *output << L";\n";
                break;
            case CFG_JUMP:
                outputIndentation(1);
                *output << L"goto ";
                outputLabelName(statement->getSwitchLabel(0));
                *output << L";\n";
                break;
            case CFG_SWITCH:
                outputIndentation(1);
                *output << L"switch (";
                outputOperand(statement->getArg1());
                *output << L") {\n";
                for (int i = 0; i < statement->getNumSwitchLabels(); i++) {
                    outputIndentation(2);
                    CFGOperand* value = statement->getSwitchValue(i);
                    if (value == NULL)
                        *output << L"default:\n";
                    else {
                        *output << L"case ";
                        outputOperand(value);
                        *output << L":\n";
                    }
                    outputIndentation(3);
                    *output << L"goto ";
                    outputLabelName(statement->getSwitchLabel(i));
                    *output << L";\n";
                }
                outputIndentation(1);
                *output << L"}\n";
                break;
            default:
                assert(!L"Unhandled jump statement");
        }
    }
    
    /**
     * Outputs a method's C++ identier.
     * @param methodIdentifier the class's identifier in the source file
     */
    void outputMethodIdentifier(wstring methodIdentifier) {
        *output << L"m_" << methodIdentifier;
    }
    
    /**
     * Outputs a class's C++ identier.
     * @param classIdentifier the class's identifier in the source file
     */
    void outputClassIdentifier(wstring classIdentifier) {
        *output << L"c_" << classIdentifier;
    }
    
    /**
     * Outputs the C++ code for the specified method call statement, excluding
     * the label name.
     */
    void outputMethodCall(CFGStatement* statement) {
        assert(
            statement->getOperation() == CFG_METHOD_CALL ||
            !L"Not a method call");
        outputIndentation(1);
        vector<CFGOperand*> args = statement->getMethodArgs();
        // TODO eventually, "print" and "println" should be real methods
        if (statement->getMethodIdentifier() == L"print" ||
            statement->getMethodIdentifier() == L"println") {
            *output << L"cout << ";
            if (!args.at(0)->getType() == REDUCED_TYPE_BOOL)
                outputOperand(args.at(0));
            else {
                *output << L'(';
                outputOperand(args.at(0));
                *output << L" ? \"true\" : \"false\")";
            }
            if (statement->getMethodIdentifier() == L"println")
                *output << L" << '\\n'";
            *output << L";\n";
        } else {
            if (statement->getDestination() != NULL) {
                outputOperand(statement->getDestination());
                *output << L" = ";
            }
            outputMethodIdentifier(statement->getMethodIdentifier());
            *output << L'(';
            for (vector<CFGOperand*>::const_iterator iterator = args.begin();
                 iterator != args.end();
                 iterator++) {
                if (iterator != args.begin())
                    *output << L", ";
                outputOperand(*iterator);
            }
            *output << L");\n";
        }
    }
    
    /**
     * Outputs the C++ code for the specified statement, exculding the label
     * name.
     */
    void outputStatement(CFGStatement* statement) {
        switch (statement->getOperation()) {
            case CFG_ARRAY_GET:
            case CFG_ARRAY_LENGTH:
            case CFG_ARRAY_SET:
                assert(!L"TODO arrays");
                break;
            case CFG_ASSIGN:
                outputIndentation(1);
                outputOperand(statement->getDestination());
                *output << L" = ";
                outputOperand(statement->getArg1());
                *output << L";\n";
                break;
            case CFG_BITWISE_INVERT:
            case CFG_NEGATE:
            case CFG_NOT:
                outputIndentation(1);
                outputOperand(statement->getDestination());
                *output << L" = ";
                outputUnaryOperation(statement->getOperation());
                outputOperand(statement->getArg1());
                *output << L";\n";
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
                *output << L" = (";
                outputType(statement->getDestination()->getType());
                *output << L")(((unsigned ";
                outputType(statement->getDestination()->getType());
                *output << L")";
                outputOperand(statement->getArg1());
                *output << L") >> ";
                outputOperand(statement->getArg2());
                *output << L");\n";
                break;
            default:
                outputIndentation(1);
                outputOperand(statement->getDestination());
                *output << L" = ";
                outputOperand(statement->getArg1());
                *output << L' ';
                outputBinaryOperation(statement->getOperation());
                *output << L' ';
                outputOperand(statement->getArg2());
                *output << L";\n";
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
                *output << L":;\n";
            }
            outputStatement(statement);
        }
    }
    
    /**
     * Outputs the specified method's prototype, excluding the semicolon.
     * @param method the method.
     * @param classIdentifier if we should also output the class name followed
     *     by two colons, the enclosing class's identifier in the source file.
     *     This is L"" if we should not output the class name.
     * @param indentation the number of indentation strings to output at the
     *     beginning of the first line.
     */
    void outputMethodSignature(
        CFGMethod* method,
        wstring classIdentifier,
        int indentation) {
        
        localVarIdentifiers.clear();
        numLocalVarSuffixes.clear();
        labelIndices.clear();
        numExpressionSuffixes = 0;
        outputIndentation(indentation);
        if (method->getReturnVar() == NULL)
            *output << L"void ";
        else {
            outputType(method->getReturnVar()->getType());
            *output << L' ';
        }
        if (classIdentifier != L"") {
            outputClassIdentifier(classIdentifier);
            *output << L"::";
        }
        outputMethodIdentifier(method->getIdentifier());
        *output << L'(';
        vector<CFGOperand*> args = method->getArgs();
        if (!args.empty()) {
            for (vector<CFGOperand*>::const_iterator iterator = args.begin();
                 iterator != args.end();
                 iterator++) {
                if (iterator != args.begin())
                    *output << L',';
                *output << L'\n';
                outputIndentation(indentation + 1);
                outputType((*iterator)->getType());
                *output << L' ';
                createNewLocalVarDeclaration(*iterator);
                outputOperand(*iterator);
            }
        }
        *output << L')';
    }
    
    /**
     * Outputs the C++ code declaring the specified class's fields.
     */
    void outputFieldDeclarations(CFGClass* clazz) {
        map<wstring, CFGOperand*> fields = clazz->getFields();
        for (map<wstring, CFGOperand*>::const_iterator iterator =
                 fields.begin();
             iterator != fields.end();
             iterator++) {
            outputIndentation(1);
            outputType(iterator->second->getType());
            *output << L' ';
            outputOperand(iterator->second);
            *output << L";\n";
        }
    }
    
    /**
     * Outputs the C++ implementation of the specified method.
     * @param method the method.
     * @param classIdentifier the enclosing class's identifier in the source
     *     file.
     */
    void outputMethod(CFGMethod* method, wstring classIdentifier) {
        outputMethodSignature(method, classIdentifier, 0);
        *output << L" {\n";
        if (method->getReturnVar() != NULL) {
            createNewLocalVarDeclaration(method->getReturnVar());
            outputIndentation(1);
            outputType(method->getReturnVar()->getType());
            *output << L' ';
            outputOperand(method->getReturnVar());
            *output << L";\n";
        }
        outputStatements(method->getStatements());
        if (method->getReturnVar() != NULL) {
            outputIndentation(1);
            *output << L"return ";
            outputOperand(method->getReturnVar());
            *output << L";\n";
        }
        *output << L"}\n";
    }
    
    /**
     * Outputs the C++ header file corresponding to the C++ source file
     * suggested by "outputClass".
     */
    void outputClassHeaderFile(CFGClass* clazz) {
        *output << L"#ifndef COMPILER_" << clazz->getIdentifier() <<
            L"_DEFINED\n" <<
            L"#define COMPILER_" << clazz->getIdentifier() << L"_DEFINED\n\n" <<
            L"class ";
        outputClassIdentifier(clazz->getIdentifier());
        *output << L" {\n"
            << L"public:\n";
        outputFieldDeclarations(clazz);
        vector<CFGMethod*> methods = clazz->getMethods();
        for (vector<CFGMethod*>::const_iterator iterator = methods.begin();
             iterator != methods.end();
             iterator++) {
            outputMethodSignature(*iterator, L"", 1);
            *output << L";\n";
        }
        outputIndentation(1);
        *output << L"void init();\n" <<
            L"};\n\n" <<
            L"#endif\n";
    }
    
    /**
     * Appends a C++ source code representation of the specified class.
     */
    void outputClass(CFGClass* clazz) {
        *output << L"#include <iostream>\n" <<
            L"#include \"" << clazz->getIdentifier() << L".hpp\"\n\n" <<
            L"using namespace std;\n\n";
        vector<CFGMethod*> methods = clazz->getMethods();
        for (vector<CFGMethod*>::const_iterator iterator = methods.begin();
             iterator != methods.end();
             iterator++) {
            outputMethod(*iterator, clazz->getIdentifier());
            *output << L"\n";
        }
        *output << L"void ";
        outputClassIdentifier(clazz->getIdentifier());
        *output << L"::init() {\n";
        outputStatements(clazz->getInitStatements());
        *output << L"}\n";
    }
public:
    CPPCompiler() {
        numExpressionSuffixes = 0;
    }
    
    void outputHeaderFile(CFGFile* file, wostream& output2) {
        output = &output2;
        outputClassHeaderFile(file->getClass());
    }
    
    void outputImplementationFile(CFGFile* file, wostream& output2) {
        output = &output2;
        outputClass(file->getClass());
    }
    
    void outputMainFile(
        wstring className,
        wstring mainMethodName,
        wostream& output2) {
        output = &output2;
        *output << L"#include \"" << className << L".hpp\"\n\n" <<
            L"int main() {\n";
        outputIndentation(1);
        outputClassIdentifier(className);
        *output << L" mainObject;\n";
        outputIndentation(1);
        *output << L"mainObject.";
        outputMethodIdentifier(mainMethodName);
        *output << L"();\n";
        outputIndentation(1);
        *output << L"return 0;\n" << L"}\n";
    }
};

void outputCPPHeaderFile(CFGFile* file, wostream& output) {
    CPPCompiler compiler;
    compiler.outputHeaderFile(file, output);
}

void outputCPPImplementationFile(CFGFile* file, wostream& output) {
    CPPCompiler compiler;
    compiler.outputImplementationFile(file, output);
}

void outputMainFile(
    wstring className,
    wstring mainMethodName,
    wostream& output) {

    CPPCompiler compiler;
    compiler.outputMainFile(className, mainMethodName, output);
}
