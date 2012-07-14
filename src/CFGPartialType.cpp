#include "CFGPartialType.hpp"
#include "Interface.hpp"

CFGPartialType::CFGPartialType(CFGType* type2) {
    type = type2;
}

CFGPartialType::~CFGPartialType() {
    delete type;
}

CFGType* CFGPartialType::getType() {
    return type;
}
