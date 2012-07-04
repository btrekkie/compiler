/* "compileFile" produces four intermediate files for a given class: an .int
 * file indicating the class's interface (output using InterfaceOutput), an .hpp
 * header file, a .cpp implementation file, and a .o object file.
 */

extern "C" {
    #include "grammar/ASTNode.h"
}

#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include "BinaryCompiler.hpp"
#include "CFG.hpp"
#include "Compiler.hpp"
#include "CPPCompiler.hpp"
#include "Interface.hpp"
#include "InterfaceOutput.hpp"
#include "Parser.hpp"

using namespace std;

// Workaround for naming conflict with BinaryCompiler::compileFile.  C++ :(
static CFGFile* (*compileFile2)(ASTNode*) = compileFile;

bool BinaryCompiler::compileFile(
    string srcDir,
    string buildDir,
    string filename) {
    // Parse and compile program
    ASTNode* node = Parser::parseFile(srcDir + '/' + filename);
    CFGFile* file = compileFile2(node);
    CFGClass* clazz = file->getClass();
    astFree(node);
    
    // Output interface file
    ClassInterface* classInterface = clazz->getInterface();
    ofstream interfaceOutputFile(
        (buildDir + '/' + clazz->getIdentifier() + ".int").c_str());
    InterfaceOutput interfaceOutput(interfaceOutputFile);
    interfaceOutput.outputClassInterface(classInterface);
    interfaceOutputFile.close();
    delete classInterface;
    
    // Output C++ header file
    ofstream headerOutput(
        (buildDir + "/" + clazz->getIdentifier() + ".hpp").c_str());
    outputCPPHeaderFile(file, headerOutput);
    headerOutput.close();
    
    // Output C++ implementation file
    string implementationFilename = buildDir + "/" + clazz->getIdentifier() +
        ".cpp";
    ofstream implementationOutput(implementationFilename.c_str());
    outputCPPImplementationFile(file, implementationOutput);
    implementationOutput.close();
    
    delete file;
    int result = system(
        (string() + "c++ -c '" + implementationFilename + "' -Wall -o '" +
         buildDir + "/" + clazz->getIdentifier() + ".o'").c_str());
    return result == 0;
}

bool BinaryCompiler::compileExecutable(
    string buildDir,
    string executableFilename,
    string className,
    string mainMethodName) {
    // Use a filename with a character that may not appear in a class
    // identifier, to avoid conflicts
    string mainFilename = buildDir + "/temp+main.cpp";
    ofstream mainFile(mainFilename.c_str());
    outputMainFile(className, mainMethodName, mainFile);
    mainFile.close();
    int result = system(
        (string() + "c++ '" + buildDir + "/" + className +
         ".o' '" + buildDir + "/temp+main.cpp' -o '" +
         executableFilename + '\'').c_str());
    return result == 0;
}
