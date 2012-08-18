#include <algorithm>
#include <assert.h>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include "../BinaryCompiler.hpp"
#include "../FileManager.hpp"
#include "../Interface.hpp"
#include "../Process.hpp"
#include "../StringUtil.hpp"
#include "BinaryCompilerTest.hpp"

using namespace std;

/**
 * The root directory containing the test source files.
 */
static wstring SRC_DIR = L"../test_src";
/**
 * The root build directory to use when compiling test source files.
 */
static wstring BUILD_DIR = L"../test_build";

/**
 * Returns a wstring that is the same as "str", but with any L'\n' characters at
 * the end removed.
 */
static wstring stripTrailingNewlines(wstring str) {
    return str.substr(0, str.find_last_not_of(L'\n') + 1);
}

map<wstring, wstring> BinaryCompilerTest::readExpectedOutput(
    wstring expectedOutputFilename) {
    map<wstring, wstring> outputs;
    int fileSize = FileManager::getSize(expectedOutputFilename);
    assertTrue(
        fileSize >= 0,
        L"Could not read expected output file " + expectedOutputFilename);
    wchar_t* lineBuffer = new wchar_t[fileSize + 1];
    wifstream input(
        StringUtil::asciiWstringToString(expectedOutputFilename).c_str());
    wstring methodIdentifier;
    wostringstream methodOutput;
    input.getline(lineBuffer, fileSize + 1);
    while (!input.eof()) {
        wstring line = lineBuffer;
        if (line == L"" || line.substr(line.length() - 1, 1) != L":")
            methodOutput << line << L"\n";
        else {
            wstring methodOutputStr = stripTrailingNewlines(methodOutput.str());
            int lastChar;
            for (lastChar = (int)methodOutputStr.length() - 1;
                 lastChar >= 0 && methodOutputStr.at(lastChar) == L'\n';
                 lastChar--);
            if (methodIdentifier != L"")
                outputs[methodIdentifier] = methodOutputStr + L'\n';
            else
                assertEqual(
                    wstring(L""),
                    methodOutputStr,
                    L"Expected output file has text before first method's "
                    L"output");
            methodIdentifier = line.substr(0, line.length() - 1);
            methodOutput.str(L"");
            assertEqual(
                0,
                (int)outputs.count(methodIdentifier),
                L"Multiple outputs for method " + methodIdentifier);
        }
        input.getline(lineBuffer, fileSize + 1);
    }
    input.close();
    delete[] lineBuffer;
    if (methodIdentifier != L"")
        outputs[methodIdentifier] = stripTrailingNewlines(methodOutput.str()) +
            L'\n';
    return outputs;
}

void BinaryCompilerTest::checkSourceFile(wstring file) {
    DirHandle* handle = DirHandle::fromDir(SRC_DIR + L'/' + file);
    if (handle != NULL) {
        wstring subfile;
        while ((subfile = handle->getNextFilename()) != L"") {
            if (file != L"")
                checkSourceFile(file + L'/' + subfile);
            else
                checkSourceFile(subfile);
        }
        delete handle;
        return;
    } else if (file.substr(max((int)file.length() - 4, 0), 4) == L".txt")
        return;
    
    if (file.substr(0, 16) == L"compiler_errors/") {
        wstring errorOutputFilename = FileManager::getTempFilename();
        wofstream errorOutput(
            StringUtil::asciiWstringToString(errorOutputFilename).c_str());
        wstring classIdentifier = BinaryCompiler::compileFile(
            SRC_DIR,
            BUILD_DIR,
            file,
            errorOutput);
        remove(StringUtil::asciiWstringToString(errorOutputFilename).c_str());
        assertEqual(
            wstring(L""),
            classIdentifier,
            L"Compiler did not emit any errors for the erroneous file " + file);
        return;
    }
    
    wstring errorOutputFilename = FileManager::getTempFilename();
    wofstream errorOutput(
        StringUtil::asciiWstringToString(errorOutputFilename).c_str());
    wstring classIdentifier = BinaryCompiler::compileFile(
        SRC_DIR,
        BUILD_DIR,
        file,
        wcerr);
    remove(StringUtil::asciiWstringToString(errorOutputFilename).c_str());
    assertNotEqual(
        wstring(L""),
        classIdentifier,
        L"Failed to compile file " + file);
    ClassInterface* interface = BinaryCompiler::getClassInterface(
        BUILD_DIR,
        classIdentifier);
    map<wstring, wstring> expectedOutputs = readExpectedOutput(
        FileManager::getParentDir(SRC_DIR + L'/' + file) + L"/" +
        classIdentifier + L".txt");
    vector<MethodInterface*> methods = interface->getMethods();
    int numTestMethods = 0;
    for (vector<MethodInterface*>::const_iterator iterator = methods.begin();
         iterator != methods.end();
         iterator++) {
        MethodInterface* method = *iterator;
        wstring methodIdentifier = method->getIdentifier();
        if (methodIdentifier.substr(0, 4) == L"test") {
            numTestMethods++;
            assertEqual(
                0,
                (int)method->getArgTypes().size(),
                L"Test method may not take any arguments");
            assertEqual(
                1,
                (int)expectedOutputs.count(methodIdentifier),
                L"Missing expected output for method " + classIdentifier +
                L'.' + methodIdentifier);
            wstring executableFilename = FileManager::getTempFilename();
            bool result = BinaryCompiler::compileExecutable(
                BUILD_DIR,
                executableFilename,
                classIdentifier,
                methodIdentifier);
            assertTrue(result, L"Failed to compile executable");
            
            wstring expectedOutput = expectedOutputs[methodIdentifier];
            Process process(executableFilename);
            process.setTimeout(5);
            process.setMaxStdOutCaptureBytes(
                2 * expectedOutput.length() + 1000);
            process.run();
            remove(
                StringUtil::asciiWstringToString(executableFilename).c_str());
            assertEqual(
                expectedOutput,
                process.getStdOut(),
                L"Output for method " + classIdentifier + L'.' +
                    methodIdentifier + L" does not match the text in the "
                    L"expected output file");
        }
    }
    assertEqual(
        numTestMethods,
        (int)expectedOutputs.size(),
        L"Expected output file for " + classIdentifier +
        L" contains output for non-test or non-existent methods");
    delete interface;
}

wstring BinaryCompilerTest::getName() {
    return L"BinaryCompilerTest";
}

void BinaryCompilerTest::test() {
    checkSourceFile(L"");
}
