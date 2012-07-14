#include <algorithm>
#include <assert.h>
#include "grammar/ASTNode.h"
#include "ASTUtil.hpp"
#include "BreakEvaluator.hpp"

using namespace std;

BreakEvaluator::BreakEvaluator(CFGOperand* returnVar2, CFGLabel* returnLabel2) {
    returnVar = returnVar2;
    returnLabel = returnLabel2;
}

void BreakEvaluator::pushBreakLabel(CFGLabel* label) {
    breakLabels.push_back(label);
    breakLevels.push_back((int)(breakLevels.size() + continueLevels.size()));
}

void BreakEvaluator::pushContinueLabel(CFGLabel* label) {
    continueLabels.push_back(label);
    continueLevels.push_back((int)(breakLevels.size() + continueLevels.size()));
}

void BreakEvaluator::popBreakLabel() {
    breakLabels.pop_back();
    breakLevels.pop_back();
}

void BreakEvaluator::popContinueLabel() {
    continueLabels.pop_back();
    continueLevels.pop_back();
}

CFGLabel* BreakEvaluator::getBreakLabel(int numLoops) {
    if (numLoops > 0 && numLoops <= (int)breakLabels.size())
        return breakLabels.at((int)breakLabels.size() - numLoops);
    else
        return NULL;
}

CFGLabel* BreakEvaluator::getContinueLabel(int numLoops) {
    if (numLoops > 0 && numLoops <= (int)continueLabels.size())
        return continueLabels.at((int)continueLabels.size() - numLoops);
    else
        return NULL;
}

bool BreakEvaluator::hasDefaultLabel(ASTNode* node) {
    if (node->type == AST_EMPTY_CASE_LIST)
        return false;
    assert(node->type == AST_CASE_LIST || !"Not a switch statement body");
    return node->child2->type == AST_CASE_LABEL_DEFAULT ||
        hasDefaultLabel(node->child1);
}

int BreakEvaluator::getNumJumpLoops(ASTNode* node) {
    if (node->child1 == NULL)
        return 1;
    else {
        long long numLoops = 0;
        ASTUtil::getIntLiteralValue(node->child1->tokenStr, numLoops);
        if (numLoops > INT_MAX)
            return INT_MAX;
        else if (numLoops < INT_MIN)
            return INT_MIN;
        else
            return (int)numLoops;
    }
}

int BreakEvaluator::computeMaxBreakLevel(ASTNode* node) {
    if (maxBreakLevels.count(node) > 0)
        return maxBreakLevels[node];
    int breakLevel = breakLevels.size() + continueLevels.size();
    int maxBreakLevel;
    switch (node->type) {
        case AST_BLOCK:
            maxBreakLevel = computeMaxBreakLevel(node->child1);
            break;
        case AST_BREAK:
        {
            int numLoops = getNumJumpLoops(node);
            if (numLoops > 0 && numLoops <= (int)breakLevels.size())
                maxBreakLevel = breakLevels.at(numLoops - 1);
            else
                maxBreakLevel = 0;
            break;
        }
        case AST_CASE_LIST:
            if (node->child3->type == AST_EMPTY_STATEMENT_LIST)
                maxBreakLevel = computeMaxBreakLevel(node->child1);
            else
                maxBreakLevel = max(
                    computeMaxBreakLevel(node->child1),
                    computeMaxBreakLevel(node->child3));
            break;
        case AST_CONTINUE:
        {
            int numLoops = getNumJumpLoops(node);
            if (numLoops > 0 && numLoops <= (int)continueLevels.size())
                maxBreakLevel = continueLevels.at(numLoops - 1);
            else
                maxBreakLevel = 0;
            break;
        }
        case AST_DO_WHILE:
        case AST_FOR:
        case AST_FOR_IN:
        case AST_WHILE:
            breakLevels.push_back(
                (int)(breakLevels.size() + continueLevels.size()));
            continueLevels.push_back(
                (int)(breakLevels.size() + continueLevels.size()));
            if (node->type == AST_DO_WHILE)
                maxBreakLevel = computeMaxBreakLevel(node->child1);
            else if (node->type == AST_WHILE)
                maxBreakLevel = computeMaxBreakLevel(node->child2);
            else
                maxBreakLevel = computeMaxBreakLevel(node->child4);
            breakLevels.pop_back();
            continueLevels.pop_back();
            maxBreakLevel = min(maxBreakLevel, breakLevel);
            break;
        case AST_EMPTY_CASE_LIST:
            maxBreakLevel = -1;
            break;
        case AST_IF_ELSE:
            maxBreakLevel = max(
                computeMaxBreakLevel(node->child2),
                computeMaxBreakLevel(node->child3));
            break;
        case AST_RETURN:
            maxBreakLevel = -1;
            break;
        case AST_STATEMENT_LIST:
            maxBreakLevel = computeMaxBreakLevel(node->child1);
            if (maxBreakLevel >= breakLevel)
                maxBreakLevel = computeMaxBreakLevel(node->child2);
            break;
        case AST_SWITCH:
            breakLevels.push_back(
                (int)breakLevels.size() + (int)continueLevels.size());
            if (!hasDefaultLabel(node->child2))
                maxBreakLevel = breakLevel;
            else
                maxBreakLevel = min(
                    computeMaxBreakLevel(node->child2),
                    breakLevel);
            breakLabels.pop_back();
            break;
        default:
            maxBreakLevel = breakLevel;
    }
    maxBreakLevels[node] = maxBreakLevel;
    return maxBreakLevel;
}

bool BreakEvaluator::alwaysBreaks(ASTNode* node) {
    int breakLevel = breakLevels.size() + continueLevels.size();
    return computeMaxBreakLevel(node) < breakLevel;
}

CFGLabel* BreakEvaluator::getReturnLabel() {
    return returnLabel;
}

CFGOperand* BreakEvaluator::getReturnVar() {
    return returnVar;
}
