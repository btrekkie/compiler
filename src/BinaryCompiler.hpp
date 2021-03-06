#ifndef BINARY_COMPILER_HPP_INCLUDED
#define BINARY_COMPILER_HPP_INCLUDED

#include <stdio.h>
#include <string>

class ClassInterface;

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
     * @param errorOutput an ostream to which to output compiler errors.
     * @return the identifier of the class we compiled, or L"" if the operation
     *     was unsuccessful.
     */
    static std::wstring compileFile(
        std::wstring srcDir,
        std::wstring buildDir,
        std::wstring filename,
        std::wostream& errorOutput);
    /**
     * Compiles an executable file, using the intermediate files produced for
     * the class in a previous call to "compileFile".  To that end, this method
     * may create or alter files in "buildDir".
     * @param buildDir the root build directory.
     * @param executableFilename the filename of the executable file to produce.
     * @param className the identifier of the class containing the main method.
     * @param mainMethodName the identifier of the main method; that is, the
     *     method that is called when the program is run.  The method should
     *     take no arguments.
     * @return whether the operation was successful.
     */
    static bool compileExecutable(
        std::wstring buildDir,
        std::wstring executableFilename,
        std::wstring className,
        std::wstring mainMethodName);
    /**
     * Returns the ClassInterface for the class with the specified name.  This
     * method may use intermediate files produced for the class in a previous
     * call to "compileFile".
     * @param buildDir the root build directory.
     * @param className the class's identifier.
     */
    static ClassInterface* getClassInterface(
        std::wstring buildDir,
        std::wstring className);
};

#endif
