#ifndef BINARY_COMPILER_HPP_INCLUDED
#define BINARY_COMPILER_HPP_INCLUDED

#include <stdio.h>
#include <string>

/**
 * Compiles source files, eventually to an executable file.
 */
class BinaryCompiler {
public:
    /**
     * Compiles the specified file and its dependencies to one or more
     * intermediate files (e.g. object files and interface files produced using
     * InterfaceOutput).  The precise files this method produces and the
     * directory structure are unspecified.
     * @param srcDir the root source file directory.
     * @param buildDir the root build directory.
     * @param filename the source file to compile, relative to the root source
     *     directory.
     */
    static bool compileFile(
        std::string srcDir,
        std::string buildDir,
        std::string filename);
    /**
     * Compiles an executable file, using the intermediate files produced in a
     * previous call to "compileFile".  To that end, this method may create or
     * alter files in "buildDir".
     * @param buildDir the root build directory.
     * @param executableFilename the filename of the executable file to produce.
     * @param className the identifier of the class containing the main method.
     * @param mainMethodName the identifier of the main method; that is, the
     *     method that is called when the program is run.
     */
    static bool compileExecutable(
        std::string buildDir,
        std::string executableFilename,
        std::string className,
        std::string mainMethodName);
};

#endif
