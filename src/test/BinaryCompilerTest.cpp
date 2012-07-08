#include <algorithm>
#include <assert.h>
#include <fstream>
#include <map>
#include <sstream>
#include <string>
#include "../BinaryCompiler.hpp"
#include "../FileManager.hpp"
#include "../Interface.hpp"
#include "../Process.hpp"
#include "BinaryCompilerTest.hpp"

using namespace std;

/**
 * The root directory containing the test source files.
 */
static string SRC_DIR = "../test_src";
/**
 * The root build directory to use when compiling test source files.
 */
static string BUILD_DIR = "../test_build";

/**
 * Returns a string that is the same as "str", but with any '\n' characters at
 * the end removed.
 */
static string stripTrailingNewlines(string str) {
    return str.substr(0, str.find_last_not_of('\n') + 1);
}

map<string, string> BinaryCompilerTest::readExpectedOutput(
    string expectedOutputFilename) {
    map<string, string> outputs;
    int fileSize = FileManager::getSize(expectedOutputFilename);
    assertTrue(
        fileSize >= 0,
        "Could not read expected output file " + expectedOutputFilename);
    char* lineBuffer = new char[fileSize + 1];
    ifstream input(expectedOutputFilename.c_str());
    string methodIdentifier;
    ostringstream methodOutput;
    input.getline(lineBuffer, fileSize + 1);
    while (!input.eof()) {
        string line = lineBuffer;
        if (line == "" || line.substr(line.length() - 1, 1) != ":")
            methodOutput << line << "\n";
        else {
            string methodOutputStr = stripTrailingNewlines(methodOutput.str());
            int lastChar;
            for (lastChar = (int)methodOutputStr.length() - 1;
                 lastChar >= 0 && methodOutputStr.at(lastChar) == '\n';
                 lastChar--);
            if (methodIdentifier != "")
                outputs[methodIdentifier] = methodOutputStr + '\n';
            else
                assertEqual(
                    string(""),
                    methodOutputStr,
                    "Expected output file has text before first method's "
                        "output");
            methodIdentifier = line.substr(0, line.length() - 1);
            methodOutput.str("");
            assertEqual(
                0,
                (int)outputs.count(methodIdentifier),
                "Multiple outputs for method " + methodIdentifier);
        }
        input.getline(lineBuffer, fileSize + 1);
    }
    input.close();
    delete[] lineBuffer;
    if (methodIdentifier != "")
        outputs[methodIdentifier] = stripTrailingNewlines(methodOutput.str()) +
            '\n';
    return outputs;
}

void BinaryCompilerTest::checkSourceFile(string file) {
    DirHandle* handle = DirHandle::fromDir(SRC_DIR + "/" + file);
    if (handle) {
        string subfile;
        while ((subfile = handle->getNextFilename()) != "")
            checkSourceFile(file + '/' + subfile);
        return;
    } else if (file.substr(max((int)file.length() - 4, 0), 4) == ".txt")
        return;
    
    string errorOutputFilename = FileManager::getTempFilename();
    ofstream errorOutput(errorOutputFilename.c_str());
    string classIdentifier = BinaryCompiler::compileFile(
        SRC_DIR,
        BUILD_DIR,
        file,
        errorOutput);
    remove(errorOutputFilename.c_str());
    if (file.substr(0, 17) == "/compiler_errors/") {
        assertEqual(
            string(""),
            classIdentifier,
            "Compiler did not emit any errors for the erroneous file " + file);
        return;
    }
    assertNotEqual(
        string(""),
        classIdentifier,
        "Failed to compile file " + file);
    ClassInterface* interface = BinaryCompiler::getClassInterface(
        BUILD_DIR,
        classIdentifier);
    map<string, string> expectedOutputs = readExpectedOutput(
        FileManager::getParentDir(SRC_DIR + "/" + file) + "/" +
            classIdentifier + ".txt");
    vector<MethodInterface*> methods = interface->getMethods();
    int numTestMethods = 0;
    for (vector<MethodInterface*>::const_iterator iterator = methods.begin();
         iterator != methods.end();
         iterator++) {
        MethodInterface* method = *iterator;
        string methodIdentifier = method->getIdentifier();
        if (methodIdentifier.substr(0, 4) == "test") {
            numTestMethods++;
            assertEqual(
                0,
                (int)method->getArgTypes().size(),
                "Test method may not take any arguments");
            assertEqual(
                1,
                (int)expectedOutputs.count(methodIdentifier),
                "Missing expected output for method " + classIdentifier + '.' +
                methodIdentifier);
            string executableFilename = FileManager::getTempFilename();
            bool result = BinaryCompiler::compileExecutable(
                BUILD_DIR,
                executableFilename,
                classIdentifier,
                methodIdentifier);
            assertTrue(result, "Failed to compile executable");
            
            string expectedOutput = expectedOutputs[methodIdentifier];
            Process process(executableFilename);
            process.setTimeout(5);
            process.setMaxStdOutCaptureBytes(
                2 * expectedOutput.length() + 1000);
            process.run();
            remove(executableFilename.c_str());
            assertEqual(
                expectedOutput,
                process.getStdOut(),
                "Output for method " + classIdentifier + '.' +
                    methodIdentifier + " does not match the text in the "
                    "expected output file");
        }
    }
    assertEqual(
        numTestMethods,
        (int)expectedOutputs.size(),
        "Expected output file for " + classIdentifier +
        " contains output for non-test or non-existent methods");
}

string BinaryCompilerTest::getName() {
    return "BinaryCompilerTest";
}

void BinaryCompilerTest::test() {
    checkSourceFile("");
}
