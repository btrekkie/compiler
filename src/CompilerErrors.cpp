#include "CompilerErrors.hpp"

using namespace std;

CompilerErrors::CompilerErrors(ostream& output2, string filename2) {
    output = &output2;
    filename = filename2;
    hasEmittedError = false;
}

void CompilerErrors::emitError(ASTNode* node, string error) {
    *output << "Compiler error in " << filename << " at line " <<
        node->lineNumber << ": " << error << '\n';
    hasEmittedError = true;
}

bool CompilerErrors::getHasEmittedError() {
    return hasEmittedError;
}
