#include <assert.h>
#include "cfg.hpp"

using namespace std;

CFGType::CFGType(string className2, int numDimensions2) {
    className = className2;
    numDimensions = numDimensions2;
}

CFGType* CFGType::boolType() {
    return new CFGType("Bool");
}

CFGType* CFGType::intType() {
    return new CFGType("Int");
}

bool CFGType::isBool() {
    return numDimensions == 0 && className == "Bool";
}

bool CFGType::isNumeric() {
    if (numDimensions > 0)
        return false;
    else if (className == "Int")
        return true;
    else if (className == "Long")
        return true;
    else if (className == "Byte")
        return true;
    else if (className == "Float")
        return true;
    else if (className == "Double")
        return true;
    else if (className == "Char")
        return true;
    else
        return false;
}

bool CFGType::isIntegerLike() {
    if (numDimensions > 0)
        return false;
    else if (className == "Int")
        return true;
    else if (className == "Long")
        return true;
    else if (className == "Byte")
        return true;
    else
        return false;
}

int CFGType::getPromotionLevel() {
    if (className == "Byte")
        return 1;
    else if (className == "Char")
        return 2;
    else if (className == "Int")
        return 3;
    else if (className == "Long")
        return 4;
    else if (className == "Float")
        return 5;
    else if (className == "Double")
        return 6;
    else {
        assert(!"Promotion only applies to numbers");
        return 0;
    }
}

bool CFGType::isMorePromotedThan(CFGType* other) {
    return getPromotionLevel() > other->getPromotionLevel();
}

CFGOperand::CFGOperand(bool value) {
    isVar = false;
    boolValue = value;
    type = CFGType::boolType();
}

CFGOperand::CFGOperand(CFGType* type2, string identifier2, bool isField2) {
    type = type2;
    identifier = identifier2;
    isField = isField2;
    isVar = true;
}

CFGOperand::CFGOperand(int value) {
    isVar = false;
    intValue = value;
    type = CFGType::intType();
}

CFGOperand* CFGOperand::one() {
    return new CFGOperand(1);
}

CFGOperand* CFGOperand::fromBool(bool value) {
    return new CFGOperand(value);
}

CFGStatement::CFGStatement(
    CFGOperation op2,
    CFGOperand* destination2,
    CFGOperand* arg1b,
    CFGOperand* arg2b) {
    
    op = op2;
    destination = destination2;
    arg1 = arg1b;
    arg2 = arg2b;
    label = NULL;
    trueLabel = NULL;
    falseLabel = NULL;
}

CFGStatement* CFGStatement::fromLabel(CFGLabel* label2) {
    CFGStatement* statement = new CFGStatement(CFG_NOP, NULL, NULL);
    statement->label = label2;
    return statement;
}

CFGStatement* CFGStatement::jump(CFGLabel* label2) {
    CFGStatement* statement = new CFGStatement(CFG_JUMP, NULL, NULL);
    statement->trueLabel = label2;
    statement->falseLabel = label2;
    return statement;
}

void CFGClass::deleteStatements(
    vector<CFGStatement*>& statements,
    set<CFGOperand*>& operands) {
    for (vector<CFGStatement*>::const_iterator iterator = statements.begin();
         iterator != statements.end();
         iterator++) {
        operands.insert((*iterator)->destination);
        operands.insert((*iterator)->arg1);
        operands.insert((*iterator)->arg2);
        if ((*iterator)->label)
            delete (*iterator)->label;
        delete *iterator;
    }
}

CFGClass::~CFGClass() {
    set<CFGOperand*> operands;
    for (map<string, CFGMethod*>::const_iterator iterator = methods.begin();
         iterator != methods.end();
         iterator++) {
        CFGMethod* method = iterator->second;
        deleteStatements(method->statements, operands);
        for (vector<CFGOperand*>::const_iterator iterator2 =
                 method->args.begin();
             iterator2 != method->args.end();
             iterator2++)
            operands.insert(*iterator2);
        operands.insert(method->returnVar);
        delete method;
    }
    for (map<string, CFGOperand*>::const_iterator iterator = fields.begin();
         iterator != fields.end();
         iterator++)
        operands.insert(iterator->second);
    deleteStatements(initStatements, operands);
    operands.erase(NULL);
    
    set<CFGType*> types;
    for (set<CFGOperand*>::const_iterator iterator = operands.begin();
         iterator != operands.end();
         iterator++) {
        types.insert((*iterator)->type);
        delete *iterator;
    }
    for (set<CFGType*>::const_iterator iterator = types.begin();
         iterator != types.end();
         iterator++)
        delete *iterator;
}

void CFGClass::addMethod(CFGMethod* method) {
    assert(
        methods.count(method->identifier) == 0 || !"TODO method overloading");
    methods[method->identifier] = method;
}

vector<CFGMethod*> CFGClass::getMethods() {
    vector<CFGMethod*> methodsVector;
    for (map<string, CFGMethod*>::const_iterator iterator = methods.begin();
         iterator != methods.end();
         iterator++)
        methodsVector.push_back(iterator->second);
    return methodsVector;
}

CFGFile::~CFGFile() {
    delete clazz;
}
