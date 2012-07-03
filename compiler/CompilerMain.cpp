/* Compiles a source code file.  The program accepts one argument: the directory
 * in which to store the compiled files.  The program creates three files in
 * that directory: an .int file, an .hpp file, and a .cpp file.  The .int file
 * gives a description of the source class's interface, output using
 * InterfaceOutput.  The .cpp file is the C++ implementation file to which we
 * compiled the source file.  The .hpp file is the header file for the .cpp
 * file.
 */

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include "grammar/ASTNode.h"
#include "CFG.hpp"
#include "Compiler.hpp"
#include "CPPCompiler.hpp"
#include "Interface.hpp"
#include "InterfaceOutput.hpp"

using namespace std;

extern "C" {
    int yyparse();
}

/**
 * The directory in which to store the interface file, C++ header file, and C++
 * implementation file.
 */
static string outputDir;

void processFile(ASTNode* node) {
    // Compile program
    CFGFile* file = compileFile(node);
    CFGClass* clazz = file->getClass();
    
    // Output interface file
    ClassInterface* classInterface = clazz->getInterface();
    ostringstream interfaceFilename;
    interfaceFilename << outputDir << '/' << clazz->getIdentifier() << ".int";
    ofstream interfaceOutputFile(interfaceFilename.str().c_str());
    InterfaceOutput interfaceOutput(interfaceOutputFile);
    interfaceOutput.outputClassInterface(classInterface);
    interfaceOutputFile.close();
    delete classInterface;
    
    // Output C++ header file
    ostringstream headerFilename;
    headerFilename << outputDir << '/' << clazz->getIdentifier() << ".hpp";
    ofstream headerOutput(headerFilename.str().c_str());
    outputCPPHeaderFile(file, headerOutput);
    headerOutput.close();
    
    // Output C++ implementation file
    ostringstream implementationFilename;
    implementationFilename << outputDir << '/' << clazz->getIdentifier() <<
        ".cpp";
    ofstream implementationOutput(implementationFilename.str().c_str());
    outputCPPImplementationFile(file, implementationOutput);
    implementationOutput.close();
    
    delete file;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cout << "Expected exactly one argument: the directory in which to "
            "store the compiled files\n";
        return -1;
    } else {
        outputDir = argv[1];
        yyparse();
        return 0;
    }
}
