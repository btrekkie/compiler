#ifndef CPP_COMPILER_HPP_INCLUDED
#define CPP_COMPILER_HPP_INCLUDED

#include <iostream>
#include <string>

class CFGFile;

/**
 * Appends the C++ header file corresponding to the C++ source file suggested by
 * "outputImplementationFile" to the specified output stream.
 */
void outputCPPHeaderFile(CFGFile* file, std::wostream& output);
/**
 * Appends a C++ source code representation of the specified compiled source
 * code to the specified output stream.
 */
void outputCPPImplementationFile(CFGFile* file, std::wostream& output);
/**
 * Appends a C++ source code representation of a file containing a main function
 * that calls the specified source code method to the specified output stream.
 * @param className the identifier of the class whose method we are calling.
 * @param mainMethodName the identifier of the method we are calling.
 * @param output the output stream.
 */
void outputMainFile(
    std::wstring className,
    std::wstring mainMethodName,
    std::wostream& output);

#endif
