#include <assert.h>
#include "CFG.hpp"
#include "Interface.hpp"

using namespace std;

CFGOperand::CFGOperand(bool value) {
    isVar = false;
    boolValue = value;
    type = REDUCED_TYPE_BOOL;
}

CFGOperand::CFGOperand(CFGReducedType type2) {
    type = type2;
    isVar = true;
    isField = false;
}

CFGOperand::CFGOperand(
    CFGReducedType type2,
    wstring identifier2,
    bool isField2) {
    type = type2;
    identifier = identifier2;
    isField = isField2;
    isVar = true;
}

CFGOperand::CFGOperand(int value) {
    isVar = false;
    intValue = value;
    type = REDUCED_TYPE_INT;
}

CFGOperand::CFGOperand(long long value) {
    isVar = false;
    longValue = value;
    type = REDUCED_TYPE_LONG;
}

CFGOperand::CFGOperand(float value) {
    isVar = false;
    floatValue = value;
    type = REDUCED_TYPE_FLOAT;
}

CFGOperand::CFGOperand(double value) {
    isVar = false;
    doubleValue = value;
    type = REDUCED_TYPE_DOUBLE;
}

bool CFGOperand::getIsVar() {
    return isVar;
}

bool CFGOperand::getIsField() {
    return isField;
}

CFGReducedType CFGOperand::getType() {
    return type;
}

wstring CFGOperand::getIdentifier() {
    return identifier;
}

bool CFGOperand::getBoolValue() {
    return boolValue;
}

int CFGOperand::getIntValue() {
    return intValue;
}

long long CFGOperand::getLongValue() {
    return longValue;
}

float CFGOperand::getFloatValue() {
    return floatValue;
}

double CFGOperand::getDoubleValue() {
    return doubleValue;
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

wstring CFGStatement::getMethodIdentifier() {
    assert(methodArgs != NULL || !L"Have not set method args");
    return methodIdentifier;
}

vector<CFGOperand*> CFGStatement::getMethodArgs() {
    assert(methodArgs != NULL || !L"Have not set method args");
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
    wstring methodIdentifier2,
    vector<CFGOperand*> methodArgs2) {
    assert(methodArgs == NULL || !L"Cannot set method args twice");
    methodIdentifier = methodIdentifier2;
    methodArgs = new vector<CFGOperand*>(methodArgs2);
}

int CFGStatement::getNumSwitchLabels() {
    if (switchLabels != NULL)
        return (int)switchLabels->size();
    else
        return 0;
}

CFGOperand* CFGStatement::getSwitchValue(int index) {
    assert(switchLabels != NULL || !L"Have not set switch values");
    return switchValues->at(index);
}

CFGLabel* CFGStatement::getSwitchLabel(int index) {
    assert(switchLabels != NULL || !L"Have not set switch labels");
    return switchLabels->at(index);
}

void CFGStatement::setSwitchValuesAndLabels(
    vector<CFGOperand*> switchValues2,
    vector<CFGLabel*> switchLabels2) {
    assert(
        switchValues2.size() == switchLabels2.size() ||
        !L"Different number of values and labels");
    assert(switchLabels == NULL || !L"Cannot set switch labels twice");
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
    wstring identifier2,
    CFGOperand* returnVar2,
    CFGType* returnType2,
    vector<CFGOperand*> args2,
    vector<CFGType*> argTypes2,
    vector<CFGStatement*> statements2) {
    identifier = identifier2;
    returnVar = returnVar2;
    returnType = returnType2;
    args = args2;
    argTypes = argTypes2;
    statements = statements2;
}

CFGMethod::~CFGMethod() {
    if (returnType != NULL)
        delete returnType;
    for (vector<CFGType*>::const_iterator iterator = argTypes.begin();
         iterator != argTypes.end();
         iterator++)
        delete *iterator;
}

wstring CFGMethod::getIdentifier() {
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
    CFGType* returnTypeCopy;
    if (returnType != NULL)
        returnTypeCopy = new CFGType(returnType);
    else
        returnTypeCopy = NULL;
    vector<CFGType*> argTypesCopy;
    for (vector<CFGType*>::const_iterator iterator = argTypes.begin();
         iterator != argTypes.end();
         iterator++)
        argTypesCopy.push_back(new CFGType(*iterator));
    return new MethodInterface(
        returnTypeCopy,
        argTypesCopy,
        identifier);
}

CFGClass::CFGClass(
    wstring identifier2,
    map<wstring, CFGOperand*> fields2,
    map<wstring, CFGType*> fieldTypes2,
    vector<CFGMethod*> methods2,
    vector<CFGStatement*> initStatements2) {
    identifier = identifier2;
    fields = fields2;
    fieldTypes = fieldTypes2;
    initStatements = initStatements2;
    for (vector<CFGMethod*>::const_iterator iterator = methods2.begin();
         iterator != methods2.end();
         iterator++)
        addMethod(*iterator);
}

CFGClass::~CFGClass() {
    set<CFGOperand*> operands;
    for (map<wstring, CFGMethod*>::const_iterator iterator = methods.begin();
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
    for (map<wstring, CFGOperand*>::const_iterator iterator = fields.begin();
         iterator != fields.end();
         iterator++)
        operands.insert(iterator->second);
    deleteStatements(initStatements, operands);
    operands.erase(NULL);
    for (set<CFGOperand*>::const_iterator iterator = operands.begin();
         iterator != operands.end();
         iterator++)
        delete *iterator;
}

void CFGClass::addMethod(CFGMethod* method) {
    assert(
        methods.count(method->getIdentifier()) == 0 ||
            !L"TODO method overloading");
    methods[method->getIdentifier()] = method;
}

void CFGClass::deleteStatements(
    vector<CFGStatement*> statements,
    set<CFGOperand*>& operands) {
    for (vector<CFGStatement*>::const_iterator iterator = statements.begin();
         iterator != statements.end();
         iterator++) {
        CFGStatement* statement = *iterator;
        operands.insert(statement->getDestination());
        operands.insert(statement->getArg1());
        operands.insert(statement->getArg2());
        switch (statement->getOperation()) {
            case CFG_IF:
            case CFG_SWITCH:
                for (int i = 0; i < statement->getNumSwitchLabels(); i++)
                    operands.insert(statement->getSwitchValue(i));
                break;
            case CFG_METHOD_CALL:
            {
                vector<CFGOperand*> methodArgs = statement->getMethodArgs();
                for (vector<CFGOperand*>::const_iterator iterator =
                         methodArgs.begin();
                     iterator != methodArgs.end();
                     iterator++)
                    operands.insert(*iterator);
                break;
            }
            default:;
        }
        if (statement->getLabel())
            delete statement->getLabel();
        delete statement;
    }
}

wstring CFGClass::getIdentifier() {
    return identifier;
}

map<wstring, CFGOperand*> CFGClass::getFields() {
    return fields;
}

vector<CFGMethod*> CFGClass::getMethods() {
    vector<CFGMethod*> methodsVector;
    for (map<wstring, CFGMethod*>::const_iterator iterator = methods.begin();
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
    for (map<wstring, CFGOperand*>::const_iterator iterator = fields.begin();
         iterator != fields.end();
         iterator++) {
        assert(fieldTypes.count(iterator->first) > 0 || !L"Missing field type");
        CFGOperand* field = iterator->second;
        fieldInterfaces.push_back(
            new FieldInterface(
                fieldTypes[iterator->first],
                field->getIdentifier()));
    }
    vector<MethodInterface*> methodInterfaces;
    vector<CFGMethod*> methodsVector = getMethods();
    for (map<std::wstring, CFGMethod*>::const_iterator iterator =
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
