#include <assert.h>
#include "CFG.hpp"
#include "Interface.hpp"

using namespace std;

CFGOperand::CFGOperand(bool value) {
    isVar = false;
    boolValue = value;
    type = CFGType::boolType();
}

CFGOperand::CFGOperand(CFGType* type2) {
    type = type2;
    isVar = true;
    isField = false;
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

bool CFGOperand::getIsVar() {
    return isVar;
}

bool CFGOperand::getIsField() {
    return isField;
}

CFGType* CFGOperand::getType() {
    return type;
}

string CFGOperand::getIdentifier() {
    return identifier;
}

bool CFGOperand::getBoolValue() {
    return boolValue;
}

int CFGOperand::getIntValue() {
    return intValue;
}

void CFGOperand::setType(CFGType* type2) {
    assert(type == NULL || !"Cannot set the type twice");
    type = type2;
}

CFGOperand* CFGOperand::one() {
    return new CFGOperand(1);
}

CFGOperand* CFGOperand::fromBool(bool value) {
    return new CFGOperand(value);
}

CFGStatement::CFGStatement(
    CFGOperation operation2,
    CFGOperand* destination2,
    CFGOperand* arg1b,
    CFGOperand* arg2b) {
    operation = operation2;
    destination = destination2;
    arg1 = arg1b;
    arg2 = arg2b;
    methodArgs = NULL;
    label = NULL;
    switchValues = NULL;
    switchLabels = NULL;
}

CFGStatement::~CFGStatement() {
    if (methodArgs != NULL)
        delete methodArgs;
    if (switchValues != NULL)
        delete switchValues;
    if (switchLabels != NULL)
        delete switchLabels;
}

CFGOperation CFGStatement::getOperation() {
    return operation;
}

CFGOperand* CFGStatement::getDestination() {
    return destination;
}

string CFGStatement::getMethodIdentifier() {
    assert(methodArgs != NULL || !"Have not set method args");
    return methodIdentifier;
}

vector<CFGOperand*> CFGStatement::getMethodArgs() {
    assert(methodArgs != NULL || !"Have not set method args");
    return *methodArgs;
}

CFGOperand* CFGStatement::getArg1() {
    return arg1;
}

CFGOperand* CFGStatement::getArg2() {
    return arg2;
}

CFGLabel* CFGStatement::getLabel() {
    return label;
}

void CFGStatement::setMethodIdentifierAndArgs(
    string methodIdentifier2,
    vector<CFGOperand*> methodArgs2) {
    assert(methodArgs == NULL || !"Cannot set method args twice");
    methodIdentifier = methodIdentifier2;
    methodArgs = new vector<CFGOperand*>(methodArgs2);
}

int CFGStatement::getNumSwitchLabels() {
    assert(switchLabels != NULL || !"Have not set switch labels");
    return (int)switchLabels->size();
}

CFGOperand* CFGStatement::getSwitchValue(int index) {
    assert(switchLabels != NULL || !"Have not set switch values");
    return switchValues->at(index);
}

CFGLabel* CFGStatement::getSwitchLabel(int index) {
    assert(switchLabels != NULL || !"Have not set switch labels");
    return switchLabels->at(index);
}

void CFGStatement::setSwitchValuesAndLabels(
    vector<CFGOperand*> switchValues2,
    vector<CFGLabel*> switchLabels2) {
    assert(
        switchValues2.size() == switchLabels2.size() ||
        !"Different number of values and labels");
    assert(switchLabels == NULL || !"Cannot set switch labels twice");
    switchValues = new vector<CFGOperand*>(switchValues2);
    switchLabels = new vector<CFGLabel*>(switchLabels2);
}

CFGStatement* CFGStatement::fromLabel(CFGLabel* label2) {
    CFGStatement* statement = new CFGStatement(CFG_NOP, NULL, NULL);
    statement->label = label2;
    return statement;
}

CFGStatement* CFGStatement::jump(CFGLabel* label2) {
    CFGStatement* statement = new CFGStatement(CFG_JUMP, NULL, NULL);
    statement->switchValues = new vector<CFGOperand*>();
    statement->switchLabels = new vector<CFGLabel*>();
    statement->switchValues->push_back(NULL);
    statement->switchLabels->push_back(label2);
    return statement;
}

CFGMethod::CFGMethod(
    std::string identifier2,
    CFGOperand* returnVar2,
    std::vector<CFGOperand*> args2,
    std::vector<CFGStatement*> statements2) {
    identifier = identifier2;
    returnVar = returnVar2;
    args = args2;
    statements = statements2;
}

string CFGMethod::getIdentifier() {
    return identifier;
}

CFGOperand* CFGMethod::getReturnVar() {
    return returnVar;
}

vector<CFGOperand*> CFGMethod::getArgs() {
    return args;
}

vector<CFGStatement*> CFGMethod::getStatements() {
    return statements;
}

MethodInterface* CFGMethod::getInterface() {
    CFGType* returnType;
    if (returnVar != NULL)
        returnType = new CFGType(returnVar->getType());
    else
        returnType = NULL;
    vector<CFGType*> argTypes;
    for (vector<CFGOperand*>::const_iterator iterator = args.begin();
         iterator != args.end();
         iterator++)
        argTypes.push_back(new CFGType((*iterator)->getType()));
    return new MethodInterface(returnType, argTypes, identifier);
}

CFGClass::CFGClass(
    string identifier2,
    map<string, CFGOperand*> fields2,
    vector<CFGMethod*> methods2,
    vector<CFGStatement*> initStatements2) {
    identifier = identifier2;
    fields = fields2;
    initStatements = initStatements2;
    for (vector<CFGMethod*>::const_iterator iterator = methods2.begin();
         iterator != methods2.end();
         iterator++)
        addMethod(*iterator);
}

CFGClass::~CFGClass() {
    set<CFGOperand*> operands;
    for (map<string, CFGMethod*>::const_iterator iterator = methods.begin();
         iterator != methods.end();
         iterator++) {
        CFGMethod* method = iterator->second;
        deleteStatements(method->getStatements(), operands);
        vector<CFGOperand*> args = method->getArgs();
        for (vector<CFGOperand*>::const_iterator iterator2 = args.begin();
             iterator2 != args.end();
             iterator2++)
            operands.insert(*iterator2);
        operands.insert(method->getReturnVar());
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
        types.insert((*iterator)->getType());
        delete *iterator;
    }
    for (set<CFGType*>::const_iterator iterator = types.begin();
         iterator != types.end();
         iterator++)
        delete *iterator;
}

void CFGClass::addMethod(CFGMethod* method) {
    assert(
        methods.count(method->getIdentifier()) == 0 ||
            !"TODO method overloading");
    methods[method->getIdentifier()] = method;
}

void CFGClass::deleteStatements(
    vector<CFGStatement*> statements,
    set<CFGOperand*> operands) {
    for (vector<CFGStatement*>::const_iterator iterator = statements.begin();
         iterator != statements.end();
         iterator++) {
        operands.insert((*iterator)->getDestination());
        if ((*iterator)->getLabel())
            delete (*iterator)->getLabel();
        delete *iterator;
    }
}

string CFGClass::getIdentifier() {
    return identifier;
}

map<string, CFGOperand*> CFGClass::getFields() {
    return fields;
}

vector<CFGMethod*> CFGClass::getMethods() {
    vector<CFGMethod*> methodsVector;
    for (map<string, CFGMethod*>::const_iterator iterator = methods.begin();
         iterator != methods.end();
         iterator++)
        methodsVector.push_back(iterator->second);
    return methodsVector;
}

vector<CFGStatement*> CFGClass::getInitStatements() {
    return initStatements;
}

ClassInterface* CFGClass::getInterface() {
    vector<FieldInterface*> fieldInterfaces;
    for (map<string, CFGOperand*>::const_iterator iterator = fields.begin();
         iterator != fields.end();
         iterator++) {
        CFGOperand* field = iterator->second;
        fieldInterfaces.push_back(
            new FieldInterface(
                new CFGType(field->getType()), field->getIdentifier()));
    }
    vector<MethodInterface*> methodInterfaces;
    vector<CFGMethod*> methodsVector = getMethods();
    for (map<std::string, CFGMethod*>::const_iterator iterator =
             methods.begin();
         iterator != methods.end();
         iterator++)
        methodInterfaces.push_back(iterator->second->getInterface());
    return new ClassInterface(fieldInterfaces, methodInterfaces, identifier);
}

CFGFile::CFGFile(CFGClass* clazz2) {
    clazz = clazz2;
}

CFGFile::~CFGFile() {
    delete clazz;
}

CFGClass* CFGFile::getClass() {
    return clazz;
}
