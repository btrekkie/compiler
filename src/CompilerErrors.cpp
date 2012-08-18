#include "CompilerErrors.hpp"

using namespace std;

CompilerErrors::CompilerErrors(wostream& output2, wstring filename2) {
    output = &output2;
    filename = filename2;
    hasEmittedError = false;
}

void CompilerErrors::emitError(ASTNode* node, wstring error) {
    *output << L"Compiler error in " << filename << L" at line " <<
        node->lineNumber << L": " << error << L'\n';
    hasEmittedError = true;
}

bool CompilerErrors::getHasEmittedError() {
    return hasEmittedError;
}
