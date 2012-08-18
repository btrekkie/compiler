/* "compileFile" produces four intermediate files for a given class: an .int
 * file indicating the class's interface (output using InterfaceOutput), an .hpp
 * header file, a .cpp implementation file, and a .o object file.
 */

extern "C" {
    #include "grammar/ASTNode.h"
}

#include <fstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include "BinaryCompiler.hpp"
#include "CFG.hpp"
#include "Compiler.hpp"
#include "CPPCompiler.hpp"
#include "Interface.hpp"
#include "InterfaceInput.hpp"
#include "InterfaceOutput.hpp"
#include "Parser.hpp"
#include "StringUtil.hpp"

using namespace std;

// Workaround for naming conflict with BinaryCompiler::compileFile.  C++ :(
static CFGFile* (*compileFile2)(ASTNode*, wstring, wostream&) = compileFile;

wstring BinaryCompiler::compileFile(
    wstring srcDir,
    wstring buildDir,
    wstring filename,
    wostream& errorOutput) {
    // Parse and compile program
    ASTNode* node = Parser::parseFile(srcDir + L'/' + filename, errorOutput);
    if (node == NULL)
        return L"";
    CFGFile* file = compileFile2(node, filename, errorOutput);
    astFree(node);
    if (file == NULL)
        return L"";
    CFGClass* clazz = file->getClass();
    wstring identifier = clazz->getIdentifier();
    
    // Output interface file
    ClassInterface* classInterface = clazz->getInterface();
    wofstream interfaceOutputFile(
        StringUtil::asciiWstringToString(
            buildDir + L'/' + identifier + L".int").c_str());
    InterfaceOutput interfaceOutput(interfaceOutputFile);
    interfaceOutput.outputClassInterface(classInterface);
    interfaceOutputFile.close();
    delete classInterface;
    
    // Output C++ header file
    wofstream headerOutput(
        StringUtil::asciiWstringToString(
            buildDir + L"/" + identifier + L".hpp").c_str());
    outputCPPHeaderFile(file, headerOutput);
    headerOutput.close();
    
    // Output C++ implementation file
    wstring implementationFilename = buildDir + L"/" + identifier + L".cpp";
    wofstream implementationOutput(
        StringUtil::asciiWstringToString(implementationFilename).c_str());
    outputCPPImplementationFile(file, implementationOutput);
    implementationOutput.close();
    
    delete file;
    int result = system(
        StringUtil::asciiWstringToString(
            wstring() + L"c++ -c '" + implementationFilename +
            L"' -Wall -o '" + buildDir + L'/' + identifier + L".o'").c_str());
    if (result == 0)
        return identifier;
    else
        return L"";
}

bool BinaryCompiler::compileExecutable(
    wstring buildDir,
    wstring executableFilename,
    wstring className,
    wstring mainMethodName) {
    // Use a filename with a character that may not appear in a class
    // identifier, to avoid conflicts
    wstring mainFilename = buildDir + L"/temp+main.cpp";
    wofstream mainFile(StringUtil::asciiWstringToString(mainFilename).c_str());
    outputMainFile(className, mainMethodName, mainFile);
    mainFile.close();
    int result = system(
        StringUtil::asciiWstringToString(
            wstring() + L"c++ '" + buildDir + L'/' + className + L".o' '" +
            buildDir + L"/temp+main.cpp' -o '" + executableFilename +
            L'\'').c_str());
    return result == 0;
}

ClassInterface* BinaryCompiler::getClassInterface(
    wstring buildDir,
    wstring className) {
    wifstream input(
        StringUtil::asciiWstringToString(
            buildDir + L"/" + className + L".int").c_str());
    return InterfaceInput::readClassInterface(input);
}
