#include <algorithm>
#include <assert.h>
#include "../BinaryCompiler.hpp"
#include "../DirHandle.hpp"
#include "../Interface.hpp"
#include "BinaryCompilerTest.hpp"

#include <string>

using namespace std;

static string srcDir = "../test_src";
static string buildDir = "../test_build";
static string executableFilename = "/tmp/compiler_executable";

void BinaryCompilerTest::checkSourceFile(string file) {
    DirHandle* handle = DirHandle::fromDir(srcDir + "/" + file);
    if (handle) {
        string subfile;
        while ((subfile = handle->getNextFilename()) != "")
            checkSourceFile(file + '/' + subfile);
        return;
    } else if (file.substr(max((int)file.length() - 4, 0), 4) == ".txt")
        return;
    
    string identifier = BinaryCompiler::compileFile(srcDir, buildDir, file);
    assertNotEqual(string(""), identifier, "Failed to compile file");
    ClassInterface* interface = BinaryCompiler::getClassInterface(
        buildDir,
        identifier);
    vector<MethodInterface*> methods = interface->getMethods();
    for (vector<MethodInterface*>::const_iterator iterator = methods.begin();
         iterator != methods.end();
         iterator++) {
        MethodInterface* method = *iterator;
        if (method->getIdentifier().substr(0, 4) == "test") {
            assertEqual(
                0,
                (int)method->getArgTypes().size(),
                "Test method may not take any arguments");
            bool result = BinaryCompiler::compileExecutable(
                buildDir,
                executableFilename,
                identifier,
                method->getIdentifier());
            assertTrue(result, "Failed to compile executable");
            // TODO run the executable and compare its results against the
            // contents of the relevant .txt file
        }
    }
}

string BinaryCompilerTest::getName() {
    return "BinaryCompilerTest";
}

void BinaryCompilerTest::test() {
    checkSourceFile("");
}
