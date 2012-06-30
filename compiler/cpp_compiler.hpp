#ifndef CPP_COMPILER_HPP_INCLUDED
#define CPP_COMPILER_HPP_INCLUDED

#include <iostream>

class CFGFile;

/**
 * Appends the C++ header file corresponding to the C++ source file suggested by
 * "outputImplementationFile" to the specified output stream.
 */
void outputCPPHeaderFile(CFGFile* file, std::ostream& output);
/**
 * Appends a C++ source code representation of the specified compiled source
 * code to the specified output stream.
 */
void outputCPPImplementationFile(CFGFile* file, std::ostream& output);

#endif
