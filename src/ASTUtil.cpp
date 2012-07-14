#include <assert.h>
#include "grammar/ASTNode.h"
#include "ASTUtil.hpp"
#include "Interface.hpp"

using namespace std;

int ASTUtil::hexDigitToInt(char c) {
    if (c >= '0' && c <= '9')
        return c - '0';
    else if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;
    else {
        assert(c >= 'a' && c <= 'f' || !"Not a hexadecimal character");
        return c - 'a' + 10;
    }
}

bool ASTUtil::getIntLiteralValue(string str, long long& value) {
    char lastChar = str.at(str.length() - 1);
    bool isLong;
    if (lastChar != 'l' && lastChar != 'L')
        isLong = false;
    else {
        isLong = true;
        str = str.substr(0, str.length() - 1);
    }
    bool isNegative = str.at(0) == '-';
    if (str.substr(isNegative ? 1 : 0, 2) == "0x") {
        // Hexadecimal
        if (str.length() > (isLong ? 18 : 10))
            return false;
        value = 0;
        for (int i = isNegative ? 3 : 2; i < (int)str.length(); i++) {
            value <<= 4;
            value |= hexDigitToInt(str.at(i));
        }
        if (!isLong)
            // e.g. 0xffffffff => -1
            value = (long long)((int)value);
        if (isNegative)
            value = -value;
        return true;
    } else {
        // Check bounds
        if (isLong) {
            if (isNegative) {
                if (str.length() > 20 || str > "-9223372036854775808")
                    return false;
            } else if (str.length() > 19 || str > "9223372036854775807")
                return false;
        } else if (isNegative) {
            if (str.length() > 11 ||
                (str.length() == 11 && str > "-2147483648"))
                return false;
        } else if (str.length() > 10 ||
                   (str.length() == 10 && str > "2147483647"))
            return false;
        
        value = 0;
        for (int i = (isNegative ? 1 : 0); i < (int)str.length(); i++) {
            value *= 10;
            value += str.at(i) - '0';
        }
        if (isNegative)
            value = -value;
        return true;
    }
}

CFGType* ASTUtil::getCFGType(ASTNode* node) {
    if (node->type == AST_TYPE_ARRAY) {
        CFGType* childType = getCFGType(node->child1);
        CFGType* type = new CFGType(
            childType->getClassName(),
            childType->getNumDimensions() + 1);
        delete childType;
        return type;
    } else {
        assert(node->type == AST_TYPE || !"Not a type node");
        assert(
            node->child1->type != AST_QUALIFIED_IDENTIFIER || !"TODO modules");
        return new CFGType(node->child1->tokenStr);
    }
}
