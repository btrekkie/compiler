/* Compiles a source code file.  The program accepts one argument: the directory
 * in which to store the compiled files.  The program creates three files in
 * that directory: an .int file, an .hpp file, and a .cpp file.  The .int file
 * gives a description of the source class's interface, output using
 * InterfaceOutput.  The .cpp file is the C++ implementation file to which we
 * compiled the source file.  The .hpp file is the header file for the .cpp
 * file.
 */

#include <iostream>
#include "BinaryCompiler.hpp"

using namespace std;

int main(int argc, char* argv[]) {
    if (argc != 4) {
        cout << "Expected exactly three arguments: the root source file "
            "directory, the root build directory, and the source file, "
            "relative to the root source file directory.\n";
        return -1;
    } else {
        BinaryCompiler::compileFile(argv[1], argv[2], argv[3], cerr);
        return 0;
    }
}
