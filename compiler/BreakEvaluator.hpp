#ifndef BREAK_EVALUATOR_HPP_INCLUDED
#define BREAK_EVALUATOR_HPP_INCLUDED

#include <map>
#include <vector>
#include "grammar/ASTNode.h"

class CFGLabel;
class CFGOperand;

/**
 * Maintains compiler state pertaining to control flow statements: break
 * statements, continue statements, and return statements.
 */
class BreakEvaluator {
private:
    /**
     * A stack of the labels that are the targets of any break statements we
     * encounter.
     */
    std::vector<CFGLabel*> breakLabels;
    /**
     * A stack of the labels that are the targets of any continue statements we
     * encounter.
     */
    std::vector<CFGLabel*> continueLabels;
    /**
     * A stack of the "break levels" associated the break labels.  The vector is
     * parallel to "breakLabels".  A "break level" is defined to be the num of
     * the number of loops or switch statements we can break and the number of
     * loops we can continue.  In other words, when we add a value to
     * "breakLevels", that value is breakLabels.size() + continueLabels.size().
     * 0 is the lowest permissible break level.
     */
    std::vector<int> breakLevels;
    /**
     * A stack of the "break levels" associated the continue labels.  The vector
     * is parallel to "continueLabels".  See the comments for "breakLevels" for
     * a definition of "break level".
     */
    std::vector<int> continueLevels;
    /**
     * A map from AST nodes to the cached return values of
     * "computeMaxBreakLevel".
     */
    std::map<ASTNode*, int> maxBreakLevels;
    /**
     * The variable in which to store the return value of the method we are
     * currently compiling, or NULL if the method has no return value.
     */
    CFGOperand* returnVar;
    /**
     * The label indicating the end of method we are currently compiling.
     */
    CFGLabel* returnLabel;
    
    /**
     * Returns whether the specified case list node (the "caseList" rule in
     * grammar.y) includes a node of type AST_CASE_LABEL_DEFAULT.
     */
    bool hasDefaultLabel(ASTNode* node);
    /**
     * Returns the "maximum break level" of the specified statement list (the
     * "statementList" rule in grammar.y) or statement node.  In the case of a
     * statement list, this indicates the minimum "distance" we will break if we
     * execute the list.  In the case of a statement, this indicates the minimum
     * distance we will break if we execute the statement list that contains it,
     * executing only up to that statement.  See the comments for "breakLevels"
     * for a definition of "break level".
     * 
     * For example, consider the following code:
     * 
     * void foo() {
     *     for (int i = 0; i < 10; i++) {
     *         for (int j = 0; j < 10; j++) {
     *             for (int k = 0; k < 10; k++) {
     *                 bar(i + j + k);
     *                 if (i == 0)
     *                     break 2;
     *                 else
     *                     continue 3;
     *             }
     *         }
     *     }
     * }
     * 
     * The break level of the block containing the call to "bar" is 6.  The
     * maximum break level of that block is 2.  This is because if we execute
     * this block, at minimum, we will break out of two loops to the body of the
     * first for loop, which has break level 2.
     * 
     * The return value of this method is necessarily greater than or equal to
     * the current break level (breakLevels.size() + continueLevels.size()).
     * The implementation of this method will temporarily alter "breakLevels"
     * and "continueLevels" to reflect the state of the node we are currently
     * checking.
     */
    int computeMaxBreakLevel(ASTNode* node);
public:
    BreakEvaluator(CFGOperand* returnVar2, CFGLabel* returnLabel2);
    /**
     * Pushes the specified label onto the stack of the labels that are the
     * targets of any break statements we encounter.
     */
    void pushBreakLabel(CFGLabel* label);
    /**
     * Pushes the specified label onto the stack of the labels that are the
     * targets of any continue statements we encounter.
     */
    void pushContinueLabel(CFGLabel* label);
    /**
     * Pops the top label from the stack of the labels that are the targets of
     * any break statements we encounter.
     */
    void popBreakLabel();
    /**
     * Pops the top label from the stack of the labels that are the targets of
     * any continue statements we encounter.
     */
    void popContinueLabel();
    /**
     * Returns the label indicating where to jump in order to break out of the
     * specified number of loops or switch statements.
     */
    CFGLabel* getBreakLabel(int numLoops);
    /**
     * Returns the label indicating where to jump in order to continue out of
     * the specified number of loops.
     */
    CFGLabel* getContinueLabel(int numLoops);
    /**
     * Returns whether we always break if we execute the specified statement
     * list (the "statementList" rule in grammar.y).  Put another way, this
     * returns whether if we added a statement to the end of the list, it would
     * be possible for this statement to be executed (assuming it is possible to
     * for the statement list to be executed).
     */
    bool alwaysBreaks(ASTNode* node);
    CFGLabel* getReturnLabel();
    CFGOperand* getReturnVar();
};

#endif
