/* The basic principles behind the implementation of TypeEvaluator are
 * relatively simple.  The type of any expression is given by the types of its
 * operands (e.g. Int + Float produces Float).  The type of a variable is
 * computed likewise.  We compute the types of the expressions by going through
 * the AST linearly.
 * 
 * Where there is branching, the type of a variable is the least common type of 
 * the types it assumes in each of the branches.  For example, consider the
 * following method:
 * 
 * void foo(Bool condition) {
 *     var foo;
 *     if (condition)
 *         foo = 2;
 *     else
 *         foo = 2.5;
 *     println(foo);
 * }
 * 
 * The type of "foo" on the last line of code is Double, which is the least
 * common type of Int and Double.
 * 
 * While the principles are simple, the implementation must be rather
 * complicated in order to correctly support control flow statements (like break
 * statements) and loops.
 * 
 * One particularly interesting quirk is loops that require multiple iterations
 * to determine variable types.  For example, consider this method:
 * 
 * void foo(Int numIterations) {
 *     var a = 1;
 *     var b = 1;
 *     var c = 1;
 *     var d = 1;
 *     var e = 1;
 *     for (var i = 0; i < numIterations; i++) {
 *         a += b;
 *         b += c;
 *         c += d;
 *         d += e;
 *         e += 1.5;
 *     }
 *     println(a);
 * }
 * 
 * The type of "a" on the last line of code is Double.  However, a single pass
 * through the loop does not reveal this information.  The first pass only
 * indicates that the type of "e" is Double.  It takes two passes to determine
 * that "d" is a Double, three to determine that "c" is a Double, and so on.  To
 * compute the types of the values in a for loop, we repeatedly visit the
 * statements of the loop, updating variable types, until we perform an
 * iteration that does not alter any of the variable types.  When we reach such
 * a fixed point, we have computed all of the relevant types.
 * 
 * I believe that the running time of "evaluateTypes" is
 * O(n^2 d (n + log(nd) + log(f)) + 1), where n is the size of the method, d is
 * the maximum depth of any class hierarchy, and f is the number of fields in
 * the enclosing class.  The nd term is the maximum number of times we may need
 * to update expressions' types.  An additional n comes from the maximum number
 * of nodes we visit between updates.  (Such worst-case behavior is realized in
 * loop-like situations as in the above example.)  Another n term comes from the
 * run time of the popBranch() method.
 */

#include <algorithm>
#include <assert.h>
#include <set>
#include <vector>
#include "ASTUtil.hpp"
#include "CFGPartialType.hpp"
#include "CompilerErrors.hpp"
#include "Interface.hpp"
#include "LinkedList.hpp"
#include "TypeEvaluator.hpp"

using namespace std;

/**
 * A class containing the implementations of the TypeEvaluator methods.
 */
class TypeEvaluatorImpl {
private:
    map<string, CFGType*>* fieldTypes;
    map<ASTNode*, int> varIDs;
    map<string, MethodInterface*>* methodInterfaces;
    CompilerErrors* errors;
    /**
     * A map from the nodes for the expressions to their types.
     */
    map<ASTNode*, CFGPartialType*> nodeTypes;
    /**
     * A map from the method's arguments' identifiers to their types.
     */
    map<string, CFGType*> argTypes;
    /**
     * The type of the return value of the method, or NULL if there is no return
     * value.
     */
    CFGType* returnType;
    /**
     * A stack of the "break levels" associated the break labels.  The vector is
     * parallel to "breakLabels".  A "break level" is defined to be the num of
     * the number of loops or switch statements we can break and the number of
     * loops we can continue.  In other words, when we add a value to
     * "breakLevels", that value is breakLabels.size() + continueLabels.size().
     * 0 is the break level of the first line of a method.
     */
    vector<int> breakLevels;
    /**
     * A stack of the "break levels" associated with the continue labels.  The
     * vector is parallel to "continueLabels".  See the comments for
     * "breakLevels" for a definition of "break level".
     */
    vector<int> continueLevels;
    /**
     * A map from the ids of the local variables that have necessarily been
     * initialized to their compile-time types at the current point of
     * computation.
     */
    map<int, CFGPartialType*> allVarTypes;
    /**
     * A stack of maps from the ids of the variables whose types were altered in
     * each branch to their former types.  If we are in an "if" branch, the top
     * entry would be a map from the ids that were set in the branch to their
     * types immediately before the if statement.  A null value indicates that
     * the variable was not necessarily initialized at the point immediately
     * before the if statement.  A null map indicates that it is impossible to
     * reach the relevant point of computation.
     * 
     * Here is an example method and the values of "reverseVarTypesStack" and
     * "allVarTypes" immediately after executing each statement:
     * 
     *                         allVarTypes:       reverseVarTypesStack:
     * void foo(Int input) {
     *    var i = 1;           {i: Int}           [{i: NULL}]
     *    var j;               {i: Int}           [{i: NULL}]
     *    if (input > 0) {                        
     *       if (input == 1) {                    
     *          j = 2;         {i: Int, j: Int}   [{i: NULL}, {j: NULL}]
     *       } else {                             
     *          j = 3;         {i: Int, j: Int}   [{i: NULL}, {j: NULL}]
     *       }                                    
     *    }                                       
     *    println(i);          {i: Int}           [{i: NULL}]
     *    if (input > 0) {                        
     *       if (input == 1) {                    
     *          i = 2L;        {i: Long}          [{i: NULL}, {i: Int}]
     *          j = 4;         {i: Long, j: Int}  [{i: NULL}, {i: Int, j: NULL}]
     *          return;        {i: Int}           [{i: NULL}, NULL]
     *          j = 5.0;       {i: Int}           [{i: NULL}, NULL]
     *       } else                               
     *          j = 6L;        {i: Int, j: Long}  [{i: NULL}, {j: NULL}]
     *    } else                                  
     *       j = 7;            {i: Int, j: Int}   [{i: NULL}, {j: NULL}]
     *    println(j);          {i: Long, j: Long} [{i: NULL, j: NULL}]
     * }
     */
    vector<map<int, CFGPartialType*>*> reverseVarTypesStack;
    /**
     * A stack indicating the values of varTypesLinkedListStack.back() at the
     * (reachable) break and continue statements that target the different break
     * levels.  The stack does not contain any NULL values.
     * 
     * For example, say there are two (reachable) break statements that break to
     * level 3.  Say that at the point of the first break statement, the
     * variables "i" and "j" were of type Int, and at the point of the second
     * break statement, the variable "i" was of type Long and the variable "j"
     * was uninitialized.  Then incomingVarTypesStack[3] will be a linked list
     * indicating the following mappings:
     * 
     * [{i: Int, j: Int}, {i: Long}]
     */
    vector<vector<LinkedList<pair<int, CFGPartialType*> >*>*>
        incomingVarTypesStack;
    /**
     * A stack of maps from indices in "incomingVarTypesStack" to the sizes of
     * the suggested "incomingVarTypesStack" elements prior to executing each of
     * the loops that contain the current point of execution.  (Each map only
     * contains entries for elements whose sizes have changed, excluding those
     * whose sizes changed in the nested child loop.)
     * 
     * We use the map in order to remove and deallocate the linked lists added
     * to "incomingVarTypesStack" during loop "trial runs", so as to achieve
     * memory usage of O(n).  See the comments for "isCheckingLoopStack" for
     * more information regardig trial runs.
     */
    vector<map<int, int>*> prevIncomingSizesStack;
    /**
     * A vector of linked lists that we may deallocate, because they are no
     * longer in use.  Such lists are added when visiting nodes of type
     * AST_RETURN; hence the field's name.
     */
    vector<LinkedList<pair<int, CFGPartialType*> >*> returnNodesToDelete;
    /**
     * A stack of linked lists indicating the entries of "allVarTypes" in the
     * order in which they were added for each branch.  Each node contains a
     * variable id and the type associated with that id, or NULL if no type is
     * associated with the id.  If a linked list contains multiple entries for a
     * given variable id, the type of the id is the type indicated by the
     * earlier node.  If it is impossible to reach the relevant point of
     * computation, the appropriate linked list is NULL.
     * 
     * The usage of such linked lists (as compared to maps) allows us to store
     * "incomingVarTypesStack" using linked lists, which enables good worst-case
     * performance.
     * 
     * Here is an example method and the values of "varTypesLinkedListStack"
     * immediately after executing each statement:
     * 
     *                         varTypesLinkedListStack:
     * void foo(Int input) {
     *    var i = 1;           [[i, Int]]
     *    var j;               [[i, Int]]
     *    if (input > 0) {     
     *       if (input == 1) { 
     *          j = 2;         [[i, Int], [j, Int] => [i, Int]]
     *       } else {          
     *          j = 3;         [[i, Int], [j, Int] => [i, Int]]
     *       }                 
     *    }                    
     *    println(i);          [[i, Int]]
     *    if (input > 0) {     
     *       if (input == 1) { 
     *          i = 2L;        [[i, Int], [i, Long] => [i, Int]]
     *          j = 4;         [[i, Int], [j, Int] => [i, Long] => [i, Int]]
     *          return;        [[i, Int], NULL]
     *          j = 5.0;       [[i, Int], NULL]
     *       } else            
     *          j = 6L;        [[i, Int], [j, Long] => [i, Int]]
     *    } else               
     *       j = 7;            [[i, Int], [j, Int] => [i, Int]]
     *    println(j);          [[j, Long] => [i, Int]]
     * }
     */
    vector<LinkedList<pair<int, CFGPartialType*> >*> varTypesLinkedListStack;
    /**
     * A set of the LinkedList nodes previously added to
     * "varTypesLinkedListStack" that have not yet been deallocated.
     */
    set<LinkedList<pair<int, CFGPartialType*> >*> linkedListNodes;
    /**
     * A stack indicating whether we are doing a "trial run" though an iteration
     * of each loop.  During such trial runs, we do not emit any errors.  We
     * wait until all of the trial runs are finished and we are executing the
     * real run.
     * 
     * See the comments at the top of the file regarding why we make multiple
     * passes through each loop.  Each pass we perform before reaching a fixed
     * point in which none of the variable types changes is considered a "trail
     * run".  The purpose of suppressing errors during the trial run is to avoid
     * emitting the same error multiple times.
     * 
     * This is a stack because we may currently be in a nested loop.  Each
     * element of the stack provides information about one of the nested loops.
     */
    vector<bool> isCheckingLoopStack;
    /**
     * A stack indicating whether we have altered any of the variable types in
     * the current iteration of each loop.  See the comments at the top of the
     * file regarding why we make multiple passes through each loop.  Once we
     * perform a pass through a loop that does not alter any variable's type, we
     * know that we have the correct expression types.
     */
    vector<bool> hasChangedStack;
    /**
     * A map from the CFGPartialTypes we allocated to the current number of
     * references to them in linked lists and "nodeTypes".  For every other
     * CFGPartialTypes reference stored in a field, there is guaranteed to be a
     * corresponding reference in a linked list.  When a type's reference count
     * reaches 0, we deallocate it.
     */
    map<CFGPartialType*, int> typeReferenceCounts;
    
    /**
     * Emits an error if the top entry of "isCheckingLoopStack" is false.  This
     * is the method we call whenever we encounter a compilation error.  See the
     * comments for "isCheckingLoopStack" for more information.
     */
    void emitError(ASTNode* node, string error) {
        if (!isCheckingLoopStack.back())
            errors->emitError(node, error);
    }
    
    /**
     * Returns the strongest type to which both "type1" and "type2" can be
     * promoted.  In other words, this returns the type of the ternary
     * expression "condition ? type1 : type2".
     */
    CFGPartialType* getLeastCommonType(
        CFGPartialType* type1,
        CFGPartialType* type2) {
        CFGType* fullType1 = type1->getType();
        CFGType* fullType2 = type2->getType();
        if (fullType1->getNumDimensions() == fullType2->getNumDimensions() &&
            fullType1->getClassName() == fullType2->getClassName())
            return type1;
        else if (fullType1->getNumDimensions() > 0 ||
                 fullType2->getNumDimensions() > 0)
            return new CFGPartialType(new CFGType("Object"));
        else if (fullType1->isNumeric() && fullType2->isNumeric()) {
            if (fullType1->isMorePromotedThan(fullType2))
                return type1;
            else
                return type2;
        } else
            return new CFGPartialType(new CFGType("Object"));
    }
    
    /**
     * Returns whether "type" can be promoted to "candidateType".
     */
    bool isSubtype(CFGPartialType* type, CFGType* candidateType) {
        CFGPartialType* partialCandidateType = new CFGPartialType(
            new CFGType(candidateType));
        CFGPartialType* commonType = getLeastCommonType(
            type,
            partialCandidateType);
        bool result = commonType->getType()->getClassName() ==
                candidateType->getClassName() &&
            commonType->getType()->getNumDimensions() ==
                candidateType->getNumDimensions();
        if (commonType != type && commonType != partialCandidateType)
            delete commonType;
        delete partialCandidateType;
        return result;
    }
    
    /**
     * Returns whether "type1" and "type2" are the same.
     */
    bool isEqual(CFGPartialType* type1, CFGPartialType* type2) {
        return type1->getType()->getClassName() ==
                type2->getType()->getClassName() &&
            type1->getType()->getNumDimensions() ==
                type2->getType()->getNumDimensions();
    }
    
    /**
     * Increments typeReferenceCounts[type].
     */
    void incrementReferenceCount(CFGPartialType* type) {
        if (typeReferenceCounts.count(type) > 0)
            typeReferenceCounts[type]++;
        else
            typeReferenceCounts[type] = 1;
    }
    
    /**
     * Decrements typeReferenceCounts[type].  Deallocates "type" if this causes
     * the count to go to 0.
     */
    void decrementReferenceCount(CFGPartialType* type) {
        assert(
            typeReferenceCounts.count(type) > 0 ||
            !"Cannot decrement reference count below 0");
        if (typeReferenceCounts[type] > 1)
            typeReferenceCounts[type]--;
        else {
            typeReferenceCounts.erase(type);
            delete type;
        }
    }
    
    /**
     * Updates "allVarTypes" and "varTypesLinkedListStack" so as to alter the
     * type of the variable with the specified id.  A NULL type indicates that
     * the variable no longer has a type (that is, the variable is not
     * guaranteed to be initialized).
     */
    void setVarType(int varID, CFGPartialType* type) {
        if (type != NULL)
            allVarTypes[varID] = type;
        else
            allVarTypes.erase(varID);
        LinkedList<pair<int, CFGPartialType*> >* linkedList =
            new LinkedList<pair<int, CFGPartialType*> >(
                pair<int, CFGPartialType*>(varID, type),
                varTypesLinkedListStack.back());
        varTypesLinkedListStack[varTypesLinkedListStack.size() - 1] =
            linkedList;
        linkedListNodes.insert(linkedList);
        if (type != NULL)
            incrementReferenceCount(type);
    }
    
    /**
     * Pushes new branch entries to "reverseVarTypesStack" and
     * "varTypesLinkedListStack".
     */
    void pushBranch() {
        if (!reverseVarTypesStack.empty() &&
            reverseVarTypesStack.back() == NULL)
            reverseVarTypesStack.push_back(NULL);
        else
            reverseVarTypesStack.push_back(new map<int, CFGPartialType*>());
        varTypesLinkedListStack.push_back(varTypesLinkedListStack.back());
    }
    
    /**
     * Pops branch entries from "reverseVarTypesStack" and
     * "varTypesLinkedListStack".  This has the effect of updating "allVarTypes"
     * and "varTypesLinkedListStack" to reflect the fact that the branch may (or
     * may not) have been executed.  For example, consider the following method:
     * 
     * void foo(Bool condition) {
     *     var i = 1.0;
     *     var j;
     *     var k = 1;
     *     if (condition) {
     *         i = 2;
     *         j = 3;
     *     }
     *     println(i);
     * }
     * 
     * Immediately before popping the entry for the if statement, "allVarTypes"
     * is {i: Int, k: Int}, "reverseVarTypesStack" is
     * [{i: NULL, k: NULL}, {i: Double, j: NULL}], and "varTypesLinkedListStack"
     * is [[k, Int] => [i, Double],
     * [j, Int] => [i, Int] => [k, Int] => [i, Double]].  After popping the
     * entry, "allVarTypes" is {i: Double, k: Int}, "reverseVarTypesStack" is
     * [{i: NULL, k: NULL}], and "varTypesLinkedListStack" is
     * [[i, Int] => [k, Int] => [i, Double]].
     * 
     * @return the LinkedList that was formerly the top entry of
     *     "varTypesLinkedListStack".
     */
    LinkedList<pair<int, CFGPartialType*> >* popBranch() {
        map<int, CFGPartialType*>* reverseVarTypes =
            reverseVarTypesStack.back();
        reverseVarTypesStack.pop_back();
        if (reverseVarTypes != NULL) {
            // Undo "reverseVarTypes"
            for (map<int, CFGPartialType*>::const_iterator iterator =
                     reverseVarTypes->begin();
                 iterator != reverseVarTypes->end();
                 iterator++) {
                int varID = iterator->first;
                if (iterator->second == NULL)
                    allVarTypes.erase(varID);
                else if (allVarTypes.count(varID) > 0)
                    allVarTypes[varID] = getLeastCommonType(
                        allVarTypes[varID],
                        iterator->second);
            }
            
            delete reverseVarTypes;
        }
        LinkedList<pair<int, CFGPartialType*> >* varTypes =
            varTypesLinkedListStack.back();
        varTypesLinkedListStack.pop_back();
        return varTypes;
    }
    
    /**
     * Pushes entries onto "breakLevels" and "incomingVarTypesStack".  We call
     * this method when we start a loop or switch statement, out of which we
     * can break.
     */
    void pushBreakLevel() {
        breakLevels.push_back(
            (int)(breakLevels.size() + (int)continueLevels.size()));
        incomingVarTypesStack.push_back(
            new vector<LinkedList<pair<int, CFGPartialType*> >*>());
    }
    
    /**
     * Pushes entries onto "continueLevels" and "incomingVarTypesStack".  We
     * call this method when we start a loop, which we can continue.
     */
    void pushContinueLevel() {
        continueLevels.push_back(
            (int)(breakLevels.size() + (int)continueLevels.size()));
        incomingVarTypesStack.push_back(
            new vector<LinkedList<pair<int, CFGPartialType*> >*>());
    }
    
    /**
     * Returns the earliest LinkedList node that is in both "first" and
     * "second".  Return NULL if there is no common node.
     */
    template<class T>
    LinkedList<T>* getLeastCommonAncestor(
        LinkedList<T>* first,
        LinkedList<T>* second) {
        set<LinkedList<T>*> nodes;
        while (first != NULL || second != NULL) {
            if (first != NULL) {
                if (nodes.count(first) > 0)
                    return first;
                nodes.insert(first);
                first = first->getNext();
            }
            if (second != NULL) {
                if (nodes.count(second) > 0)
                    return second;
                nodes.insert(second);
                second = second->getNext();
            }
        }
        return NULL;
    }
    
    /**
     * Returns the earliest LinkedList node that is in each of the specified
     * lists.  Return NULL if there is no common node.
     */
    template<class T>
    LinkedList<T>* getLeastCommonAncestor(vector<LinkedList<T>*>& lists) {
        LinkedList<T>* ancestor = lists.at(0);
        for (int i = 1; i < (int)lists.size(); i++)
            ancestor = getLeastCommonAncestor(ancestor, lists.at(i));
        return ancestor;
    }
    
    /**
     * Returns a map of all of the mappings in "start" that precede the node
     * "end".  A NULL value indicates that the returned map should have no
     * mapping for the corresponding integer.  If a given integer appears in
     * multiple entries, the earlier one takes precedence.
     */
    map<int, CFGPartialType*> linkedListToMap(
        LinkedList<pair<int, CFGPartialType*> >* end,
        LinkedList<pair<int, CFGPartialType*> >* start) {
        vector<LinkedList<pair<int, CFGPartialType*> >*> nodes;
        for (LinkedList<pair<int, CFGPartialType*> >* node = start;
             node != end;
             node = node->getNext())
            nodes.push_back(node);
        map<int, CFGPartialType*> varTypes;
        for (int i = (int)nodes.size() - 1; i >= 0; i--) {
            if (nodes[i]->getValue().second != NULL)
                varTypes[nodes[i]->getValue().first] =
                    nodes[i]->getValue().second;
            else
                varTypes.erase(nodes[i]->getValue().first);
        }
        return varTypes;
    }
    
    /**
     * Updates "allVarTypes" and the last elements of "reverseVarTypesStack" and
     * "varTypesLinkedListStack" to reflect the fact that the current point of
     * computation can (only) be reached from branches whose variable types are
     * indicated by the specified LinkedList.  Each LinkedList in
     * "incomingVarTypes" must be a predecessor of
     * varTypesLinkedListStack.back() or equal to
     * varTypesLinkedListStack.back().  See the comments for
     * "varTypesLinkedListStack" for more information regarding the linked list
     * representation of types.
     * 
     * For example, say we have the following method:
     * 
     * void foo(Int input) {
     *     var i = 1;
     *     var j;
     *     var k;
     *     switch (input) {
     *         case 0:
     *             i = 2L;
     *             j = 3;
     *             break;
     *         case 1:
     *             return;
     *         default:
     *             i = 4.0;
     *             j = 5L;
     *             k = 6;
     *     }
     *     println(i + j + k);
     * }
     * 
     * We will call "mergeIncomingBranches" once: for the end of the switch
     * statement.  At that point, "allVarTypes" is {i: Int},
     * "reverseVarTypesStack" is [{i: NULL}], and "varTypesLinkedListStack" is
     * [[i, Int]].  The argument to the call is
     * [[j, Int] => [i, Long] => [i, Int],
     * [k, Int] => [j, Long] => [i, Double] => [i, Int]].  After we call the
     * method, "allVarTypes" is {i: Double, j: Long}, "reverseVarTypesStack" is
     * [{i: NULL, j: NULL}], and "varTypesLinkedListStack" is
     * [[j, Long] => [i, Double] => [i, Int]].
     */
    void mergeIncomingBranches(
        vector<LinkedList<pair<int, CFGPartialType*> >*>& incomingVarTypes) {
        map<int, CFGPartialType*>* reverseVarTypes =
            reverseVarTypesStack.back();
        
        // The current point of computation may now be reachable
        if (reverseVarTypes == NULL) {
            reverseVarTypes = new map<int, CFGPartialType*>();
            reverseVarTypesStack.pop_back();
            reverseVarTypesStack.push_back(reverseVarTypes);
        }
        
        if (incomingVarTypes.size() == 0) {
            // The current point of computation is unreachable
            for (map<int, CFGPartialType*>::const_iterator iterator =
                     reverseVarTypes->begin();
                 iterator != reverseVarTypes->end();
                 iterator++)
                setVarType(iterator->first, iterator->second);
            delete reverseVarTypes;
            reverseVarTypesStack.pop_back();
            reverseVarTypesStack.push_back(NULL);
            varTypesLinkedListStack.pop_back();
            varTypesLinkedListStack.push_back(NULL);
            return;
        }
        
        // Compute "initializedVars", the set of variables that were not
        // initialized in "allVarTypes", but should be now.  "initializedVars"
        // consists of each variable id that is in all of the maps, but not in
        // "allVarTypes".
        LinkedList<pair<int, CFGPartialType*> >* ancestor;
        if (incomingVarTypes.size() > 1)
            ancestor = getLeastCommonAncestor(incomingVarTypes);
        else
            ancestor = varTypesLinkedListStack.back();
        set<int> initializedVars;
        for (vector<LinkedList<pair<int, CFGPartialType*> >*>::const_iterator
                 iterator = incomingVarTypes.begin();
             iterator != incomingVarTypes.end();
             iterator++) {
            map<int, CFGPartialType*> varTypes = linkedListToMap(
                ancestor,
                *iterator);
            if (iterator == incomingVarTypes.begin()) {
                for (map<int, CFGPartialType*>::const_iterator iterator2 =
                         varTypes.begin();
                     iterator2 != varTypes.end();
                     iterator2++) {
                    int varID = iterator2->first;
                    if (allVarTypes.count(varID) == 0)
                        initializedVars.insert(iterator2->first);
                }
            } else {
                set<int> newInitializedVars;
                for (set<int>::const_iterator iterator2 =
                         initializedVars.begin();
                     iterator2 != initializedVars.end();
                     iterator2++) {
                    if (varTypes.count(*iterator2) > 0)
                        newInitializedVars.insert(*iterator2);
                }
                initializedVars = newInitializedVars;
            }
        }
        
        // Update "allVarTypes" and "reverseVarTypes"
        for (vector<LinkedList<pair<int, CFGPartialType*> >*>::const_iterator
                 iterator = incomingVarTypes.begin();
             iterator != incomingVarTypes.end();
             iterator++) {
            map<int, CFGPartialType*> varTypes = linkedListToMap(
                ancestor,
                *iterator);
            for (map<int, CFGPartialType*>::const_iterator iterator2 =
                     varTypes.begin();
                 iterator2 != varTypes.end();
                 iterator2++) {
                int varID = iterator2->first;
                if (reverseVarTypes->count(varID) == 0) {
                    if (allVarTypes.count(varID) > 0)
                        (*reverseVarTypes)[varID] = allVarTypes[varID];
                    else
                        (*reverseVarTypes)[varID] = NULL;
                }
                if (allVarTypes.count(varID) > 0) {
                    setVarType(
                        varID, 
                        getLeastCommonType(
                            allVarTypes[varID],
                            iterator2->second));
                } else if (initializedVars.count(varID) > 0)
                    setVarType(varID, iterator2->second);
            }
        }
    }
    
    /**
     * Pops an entry from "incomingVarTypesStack", passes it to
     * "mergeIncomingBranches", and deallocates it.  We call this method when
     * we reach the end of a loop or switch statement, which may be targets of
     * break and continue statements.  See the comments for
     * "mergeIncomingBranches" for more information.
     */
    void popIncomingVarTypes() {
        vector<LinkedList<pair<int, CFGPartialType*> >*>* incomingVarTypes =
            incomingVarTypesStack.back();
        incomingVarTypesStack.pop_back();
        mergeIncomingBranches(*incomingVarTypes);
        delete incomingVarTypes;
    }
    
    /**
     * Pops entries from "breakLevels" and "incomingVarTypesStack", updating
     * "allVarTypes" and the last entry of "reverseVarTypesStack" to reflect the
     * types of the variables at the break statements that target the current
     * point of computation.  See the comments for "mergeIncomingBranches" for
     * an indication of what updates we perform.  We call this method when we
     * finish a loop or switch statement, out of which we may break.
     */
    void popBreakLevel() {
        popIncomingVarTypes();
        breakLevels.pop_back();
    }
    
    /**
     * Pops entries from "continueLevels" and "incomingVarTypesStack", updating
     * "allVarTypes" and the last entry of "reverseVarTypesStack" to reflect the
     * types of the variables at the break statements that target the current
     * point of computation.  See the comments for "mergeIncomingBranches" for
     * an indication of what updates we perform.  We call this method when we
     * finish a loop, which we may continue.
     */
    void popContinueLevel() {
        popIncomingVarTypes();
        continueLevels.pop_back();
    }
    
    /**
     * Sets the type of the value indicated by the specified variable node of
     * type AST_IDENTIFIER.  Emits an "incompatible types in assignment" error
     * if the node indicates a field or argument that is not compatible with the
     * specified type (e.g. if setting an Int argument to have a Double value).
     */
    void setVarValueType(ASTNode* node, CFGPartialType* type) {
        assert(node->type == AST_IDENTIFIER || !"Not a variable node");
        assert(
            varIDs.count(node) > 0 ||
            !"Missing variable id.  Probably a bug in VarResolver.");
        incrementReferenceCount(type);
        if (nodeTypes.count(node) > 0)
            decrementReferenceCount(nodeTypes[node]);
        nodeTypes[node] = type;
        string identifier = node->tokenStr;
        if (varIDs[node] < 0) {
            if (fieldTypes->count(identifier) > 0) {
                if (!isSubtype(type, (*fieldTypes)[identifier]))
                    emitError(node, "Incompatible types in assignment");
                return;
            } else if (argTypes.count(identifier) > 0) {
                if (!isSubtype(type, argTypes[identifier]))
                    emitError(node, "Incompatible types in assignment");
                return;
            }
        } else if (reverseVarTypesStack.back() != NULL) {
            int varID = varIDs[node];
            if (allVarTypes.count(varID) > 0 &&
                isEqual(allVarTypes[varID], type))
                return;
            if (reverseVarTypesStack.back()->count(varID) == 0) {
                if (allVarTypes.count(varID) > 0)
                    (*(reverseVarTypesStack.back()))[varID] =
                        allVarTypes[varID];
                else
                    (*(reverseVarTypesStack.back()))[varID] = NULL;
            }
            setVarType(varID, type);
        }
    }
    
    /**
     * Returns the type of the elements of arrays of the specified type.
     * Returns a suitable substitute if "arrayType" is not an array type.
     */
    CFGPartialType* getElementType(CFGPartialType* arrayType) {
        if (arrayType->getType()->getNumDimensions() == 0)
            return arrayType;
        else
            return new CFGPartialType(
                new CFGType(
                    arrayType->getType()->getClassName(),
                    arrayType->getType()->getNumDimensions() - 1));
    }
    
    /**
     * Visits a node of type AST_ARRAY_GET.
     * @param node the node to visit.
     * @return the type of the expression.
     */
    CFGPartialType* visitArrayGet(ASTNode* node) {
        assert(node->type == AST_ARRAY_GET || !"Not an array get node");
        CFGPartialType* arrayType = visitExpression(node->child1);
        if (arrayType->getType()->getNumDimensions() == 0)
            emitError(node, "Operand must be an array");
        CFGPartialType* indexType = visitExpression(node->child2);
        CFGType* intType = new CFGType("Int");
        if (!indexType->getType()->isIntegerLike() ||
            indexType->getType()->isMorePromotedThan(intType))
            emitError(node, "Array index must be an integer");
        delete intType;
        return getElementType(arrayType);
    }
    
    /**
     * Visits a node of type AST_ASSIGNMENT_EXPRESSION.
     * @param node the node to visit.
     * @return the type of the expression.
     */
    CFGPartialType* visitAssignmentExpression(ASTNode* node) {
        int operation = 0;
        switch (node->child2->type) {
            case AST_AND_ASSIGN:
                operation = AST_BITWISE_AND;
                break;
            case AST_ASSIGN:
                break;
            case AST_DIV_ASSIGN:
                operation = AST_DIV;
                break;
            case AST_LEFT_SHIFT_ASSIGN:
                operation = AST_LEFT_SHIFT;
                break;
            case AST_MINUS_ASSIGN:
                operation = AST_MINUS;
                break;
            case AST_MOD_ASSIGN:
                operation = AST_MOD;
                break;
            case AST_MULT_ASSIGN:
                operation = AST_MULT;
                break;
            case AST_OR_ASSIGN:
                operation = AST_BITWISE_OR;
                break;
            case AST_PLUS_ASSIGN:
                operation = AST_PLUS;
                break;
            case AST_RIGHT_SHIFT_ASSIGN:
                operation = AST_RIGHT_SHIFT;
                break;
            case AST_UNSIGNED_RIGHT_SHIFT_ASSIGN:
                operation = AST_UNSIGNED_RIGHT_SHIFT;
                break;
            case AST_XOR_ASSIGN:
                operation = AST_XOR;
                break;
            default:
                assert(!"Unhandled assignment type");
        }
        CFGPartialType* arrayElementType;
        if (node->child1->type == AST_ARRAY_GET)
            arrayElementType = visitExpression(node->child1);
        else
            arrayElementType = NULL;
        CFGPartialType* type;
        if (node->child2->type == AST_ASSIGN)
            type = visitExpression(node->child3);
        else
            type = visitMathExpression(
                node,
                node->child1,
                node->child3,
                operation);
        if (node->child1->type == AST_IDENTIFIER)
            setVarValueType(node->child1, type);
        else if (node->child1->type == AST_ARRAY_GET) {
            if (!isSubtype(type, arrayElementType->getType()))
                emitError(node->child1, "Incorrect element type for array");
            return arrayElementType;
        }
        return type;
    }
    
    /**
     * Visits a node for performing a "mathematical" operation.
     * @param node the node to visit.
     * @return the type of the expression.
     */
    CFGPartialType* visitMathExpression(
        ASTNode* node,
        ASTNode* operand1,
        ASTNode* operand2,
        int operation) {
        switch (operation) {
            case AST_BITWISE_AND:
            case AST_BITWISE_OR:
            case AST_XOR:
            {
                CFGPartialType* type1 = visitExpression(operand1);
                if (!type1->getType()->isIntegerLike())
                    emitError(node, "Operand must be of an integer-like type");
                CFGPartialType* type2 = visitExpression(operand2);
                if (!type2->getType()->isIntegerLike())
                    emitError(node, "Operand must be of an integer-like type");
                return getLeastCommonType(type1, type2);
            }
            case AST_BITWISE_INVERT:
            {
                CFGPartialType* type = visitExpression(operand1);
                if (!type->getType()->isIntegerLike())
                    emitError(node, "Operand must be of an integer-like type");
                return type;
            }
            case AST_DIV:
            case AST_MINUS:
            case AST_MULT:
            case AST_PLUS:
            {
                CFGPartialType* type1 = visitExpression(operand1);
                if (!type1->getType()->isNumeric())
                    emitError(node, "Operand must be a number");
                CFGPartialType* type2 = visitExpression(operand2);
                if (!type2->getType()->isNumeric())
                    emitError(node, "Operand must be a number");
                return getLeastCommonType(type1, type2);
            }
            case AST_GREATER_THAN:
            case AST_GREATER_THAN_OR_EQUAL_TO:
            case AST_LESS_THAN:
            case AST_LESS_THAN_OR_EQUAL_TO:
            {
                CFGPartialType* type1 = visitExpression(operand1);
                if (!type1->getType()->isNumeric())
                    emitError(node, "Operand must be a number");
                CFGPartialType* type2 = visitExpression(operand2);
                if (!type2->getType()->isNumeric())
                    emitError(node, "Operand must be a number");
                return new CFGPartialType(new CFGType("Bool"));
            }
            case AST_LEFT_SHIFT:
            case AST_RIGHT_SHIFT:
            case AST_UNSIGNED_RIGHT_SHIFT:
            {
                CFGPartialType* type1 = visitExpression(operand1);
                if (!type1->getType()->isIntegerLike())
                    emitError(node, "Operand must be of an integer-like type");
                CFGPartialType* type2 = visitExpression(operand2);
                if (type2->getType()->getNumDimensions() > 0 ||
                    (type2->getType()->getClassName() != "Int" &&
                     type2->getType()->getClassName() != "Byte"))
                    emitError(
                        node,
                        "Operand to bit shift must be integer or byte");
                return type1;
            }
            case AST_MOD:
            {
                CFGPartialType* type1 = visitExpression(operand1);
                if (!type1->getType()->isIntegerLike())
                    emitError(node, "Operand must be of an integer-like type");
                CFGPartialType* type2 = visitExpression(operand2);
                if (!type2->getType()->isIntegerLike())
                    emitError(node, "Operand must be of an integer-like type");
                return getLeastCommonType(type1, type2);
            }
            case AST_NEGATE:
            {
                CFGPartialType* type = visitExpression(operand1);
                if (!type->getType()->isNumeric())
                    emitError(node, "Operand must be a number");
                return type;
            }
            default:
                assert(!"Unhandled math expression");
                return NULL;
        }
    }
    
    /**
     * Visits a node for performing a boolean-related operation.
     * @param node the node to visit.
     * @return the type of the expression.
     */
    CFGPartialType* visitBooleanExpression(ASTNode* node) {
        switch (node->type) {
            case AST_BOOLEAN_AND:
            case AST_BOOLEAN_OR:
            {
                CFGPartialType* type1 = visitExpression(node->child1);
                if (!type1->getType()->isBool())
                    emitError(node, "Operand must be a boolean");
                pushBranch();
                CFGPartialType* type2 = visitExpression(node->child2);
                if (!type2->getType()->isBool())
                    emitError(node, "Operand must be a boolean");
                popBranch();
                return new CFGPartialType(new CFGType("Bool"));
            }
            case AST_EQUALS:
            case AST_NOT_EQUALS:
                // TODO (classes) type checking
                visitExpression(node->child1);
                visitExpression(node->child2);
                return new CFGPartialType(new CFGType("Bool"));
            case AST_NOT:
            {
                CFGPartialType* type = visitExpression(node->child1);
                if (!type->getType()->isBool())
                    emitError(node, "Operand must be a boolean");
                return new CFGPartialType(new CFGType("Bool"));
            }
            case AST_TERNARY:
            {
                CFGPartialType* conditionType = visitExpression(node->child1);
                if (!conditionType->getType()->isBool())
                    emitError(node, "Operand must be a boolean");
                vector<LinkedList<pair<int, CFGPartialType*> >*>
                    incomingVarTypes;
                pushBranch();
                CFGPartialType* trueType = visitExpression(node->child2);
                incomingVarTypes.push_back(popBranch());
                pushBranch();
                CFGPartialType* falseType = visitExpression(node->child3);
                incomingVarTypes.push_back(popBranch());
                mergeIncomingBranches(incomingVarTypes);
                return getLeastCommonType(trueType, falseType);
            }
            default:
                assert(!"Unhanded boolean expression");
        }
        return NULL;
    }
    
    /**
     * Returns the type of the specified node indicating a literal value.
     */
    CFGPartialType* visitLiteral(ASTNode* node) {
        switch (node->type) {
            case AST_FALSE:
            case AST_TRUE:
                return new CFGPartialType(new CFGType("Bool"));
            case AST_FLOAT_LITERAL:
            {
                string str = node->tokenStr;
                if (str.at(str.length() - 1) != 'f' &&
                    str.at(str.length() - 1) != 'F')
                    return new CFGPartialType(new CFGType("Double"));
                else
                    return new CFGPartialType(new CFGType("Float"));
            }
            case AST_INT_LITERAL:
            {
                string str = node->tokenStr;
                if (str.at(str.length() - 1) != 'l' &&
                    str.at(str.length() - 1) != 'L')
                    return new CFGPartialType(new CFGType("Int"));
                else
                    return new CFGPartialType(new CFGType("Long"));
            }
            default:
                assert(!"Unhandled literal type");
        }
        return NULL;
    }
    
    /**
     * Visits a node of type AST_IDENTIFIER for using a variable's value.
     * @param node the node to visit.
     * @return the type of the expression.
     */
    CFGPartialType* visitVarUse(ASTNode* node) {
        // TODO use non-specific partial types rather than Object
        assert(node->type == AST_IDENTIFIER || !"Not a variable node");
        assert(
            varIDs.count(node) > 0 ||
            !"Missing variable id.  Probably a bug in VarResolver.");
        int varID = varIDs[node];
        if (varID < 0) {
            string identifier = node->tokenStr;
            if (argTypes.count(identifier) > 0)
                return new CFGPartialType(new CFGType(argTypes[identifier]));
            else if (fieldTypes->count(identifier) > 0)
                return new CFGPartialType(
                    new CFGType((*fieldTypes)[identifier]));
            else
                return new CFGPartialType(new CFGType("Object"));
        } else if (reverseVarTypesStack.back() == NULL)
            return new CFGPartialType(new CFGType("Object"));
        else if (allVarTypes.count(varID) > 0) {
            if (nodeTypes.count(node) > 0)
                // This branch is essential.  Without it, TypeEvaluator would
                // take exponential time
                return getLeastCommonType(allVarTypes[varID], nodeTypes[node]);
            else
                return allVarTypes[varID];
        } else {
            emitError(
                node,
                "Variable may be used before it is initialized");
            return new CFGPartialType(new CFGType("Object"));
        }
    }
    
    /**
     * Visits an expression list node (the "expressionList" rule in grammar.y).
     * @param node the node to visit.
     * @param types a vector to which to append the types of the expressions.
     */
    void visitExpressionList(ASTNode* node, vector<CFGPartialType*>& types) {
        if (node->type != AST_EXPRESSION_LIST)
            types.push_back(visitExpression(node));
        else {
            visitExpressionList(node->child1, types);
            types.push_back(visitExpression(node->child2));
        }
    }
    
    /**
     * Visits a node of type AST_METHOD_CALL.
     * @param node the node to visit.
     * @return the type of the expression, or NULL if the method does not have
     *     a return type.
     */
    CFGPartialType* visitMethodCall(ASTNode* node) {
        string identifier = node->child1->tokenStr;
        vector<CFGPartialType*> types;
        if (node->child2 != NULL)
            visitExpressionList(node->child2, types);
        if (methodInterfaces->count(identifier) == 0)
            return new CFGPartialType(new CFGType("Object"));
        else {
            MethodInterface* interface = (*methodInterfaces)[identifier];
            vector<CFGType*> argTypes = interface->getArgTypes();
            for (int i = 0; i < (int)min(types.size(), argTypes.size()); i++) {
                if (!isSubtype(types[i], argTypes[i]))
                    emitError(node, "Method argument is of incorrect type");
            }
            if (interface->getReturnType() != NULL)
                return new CFGPartialType(
                    new CFGType(interface->getReturnType()));
            else
                return NULL;
        }
    }
    
    /**
     * Visits a node of type AST_[POST|PRE]_[INCREMENT_DECREMENT].
     * @param node the node to visit.
     * @return the type of the expression.
     */
    CFGPartialType* visitIncrementExpression(ASTNode* node) {
        CFGPartialType* type = visitExpression(node->child1);
        if (!type->getType()->isNumeric())
            emitError(
                node,
                "Increment / decrement operator may only be used on numbers");
        return type;
    }
    
    /**
     * Visits an expression node.
     * @param node the node to visit.
     * @return the type of the expression.
     */
    CFGPartialType* visitExpression(ASTNode* node) {
        CFGPartialType* type = NULL;
        switch (node->type) {
            case AST_ARRAY:
                assert(!"TODO array literals");
                break;
            case AST_ARRAY_GET:
                type = visitArrayGet(node);
                break;
            case AST_ASSIGNMENT_EXPRESSION:
                type = visitAssignmentExpression(node);
                break;
            case AST_BITWISE_AND:
            case AST_BITWISE_INVERT:
            case AST_BITWISE_OR:
            case AST_DIV:
            case AST_GREATER_THAN:
            case AST_GREATER_THAN_OR_EQUAL_TO:
            case AST_LEFT_SHIFT:
            case AST_LESS_THAN:
            case AST_LESS_THAN_OR_EQUAL_TO:
            case AST_MINUS:
            case AST_MOD:
            case AST_MULT:
            case AST_NEGATE:
            case AST_PLUS:
            case AST_RIGHT_SHIFT:
            case AST_UNSIGNED_RIGHT_SHIFT:
            case AST_XOR:
                type = visitMathExpression(
                    node,
                    node->child1,
                    node->child2,
                    node->type);
                break;
            case AST_BOOLEAN_AND:
            case AST_BOOLEAN_OR:
            case AST_EQUALS:
            case AST_NOT:
            case AST_NOT_EQUALS:
            case AST_TERNARY:
                type = visitBooleanExpression(node);
                break;
            case AST_CAST:
            case AST_NEW:
            case AST_TARGETED_METHOD_CALL:
                assert(!"TODO classes");
                break;
            case AST_FALSE:
            case AST_FLOAT_LITERAL:
            case AST_INT_LITERAL:
            case AST_TRUE:
                type = visitLiteral(node);
                break;
            case AST_IDENTIFIER:
                type = visitVarUse(node);
                break;
            case AST_METHOD_CALL:
                type = visitMethodCall(node);
                if (type == NULL)
                    type = new CFGPartialType(new CFGType("Object"));
                break;
            case AST_POST_DECREMENT:
            case AST_POST_INCREMENT:
            case AST_PRE_DECREMENT:
            case AST_PRE_INCREMENT:
                type = visitIncrementExpression(node);
                break;
            case AST_QUALIFIED_IDENTIFIER:
                assert(!"TODO classes");
                break;
            default:
                assert(!"Unhandled expression type");
                break;
        }
        if (nodeTypes.count(node) == 0 || !isEqual(nodeTypes[node], type)) {
            incrementReferenceCount(type);
            nodeTypes[node] = type;
            hasChangedStack.pop_back();
            hasChangedStack.push_back(true);
        } else {
            CFGPartialType* storedType = nodeTypes[node];
            if (typeReferenceCounts.count(type) == 0)
                delete type;
            type = storedType;
        }
        return type;
    }
    
    /**
     * Visits the specified control flow statement node (a break, continue, or
     * return statement).
     */
    void visitControlFlowStatement(ASTNode* node) {
        switch (node->type) {
            case AST_BREAK:
            case AST_CONTINUE:
            {
                // Add an element to "incomingVarTypesStack"
                long long numLoops;
                if (node->child1 == NULL ||
                    !ASTUtil::getIntLiteralValue(
                        node->child1->tokenStr,
                        numLoops))
                    numLoops = 1;
                else if (numLoops <= 0)
                    numLoops = 1;
                int breakLevel;
                if (node->type == AST_BREAK)
                    breakLevel = breakLevels.at(
                        (int)breakLevels.size() -
                            (int)min(
                                numLoops,
                                (long long)breakLevels.size()));
                else
                    breakLevel = continueLevels.at(
                        (int)continueLevels.size() -
                            (int)min(
                                numLoops,
                                (long long)continueLevels.size()));
                if (reverseVarTypesStack.back() != NULL) {
                    if (prevIncomingSizesStack.back()->count(breakLevel) == 0)
                        (*(prevIncomingSizesStack.back()))[breakLevel] =
                            (int)incomingVarTypesStack.at(breakLevel)->size();
                    incomingVarTypesStack.at(breakLevel)->push_back(
                        varTypesLinkedListStack.back());
                }
                break;
            }
            case AST_RETURN:
                if (node->child1 != NULL) {
                    CFGPartialType* type = visitExpression(node->child1);
                    if (returnType != NULL && !isSubtype(type, returnType))
                        emitError(node, "Return value is of incorrect type");
                }
                break;
            default:
                assert(!"Unhanded control flow statement type");
        }
        
        map<int, CFGPartialType*>* reverseVarTypes =
            reverseVarTypesStack.back();
        if (reverseVarTypes != NULL) {
            // Undo "reverseVarTypes" and replace it with NULL
            for (map<int, CFGPartialType*>::const_iterator iterator =
                     reverseVarTypes->begin();
                 iterator != reverseVarTypes->end();
                 iterator++)
                setVarType(iterator->first, iterator->second);
            delete reverseVarTypes;
            reverseVarTypesStack.pop_back();
            reverseVarTypesStack.push_back(NULL);
            if (varTypesLinkedListStack.back() != NULL) {
                if (node->type == AST_RETURN)
                    returnNodesToDelete.push_back(
                        varTypesLinkedListStack.back());
                varTypesLinkedListStack.pop_back();
                varTypesLinkedListStack.push_back(NULL);
            }
        }
    }
    
    /**
     * Visits the portion of the specified loop node (a while or for loop) that
     * we execute unconditionally, excluding any portion that we also execute in
     * each iteration of the loop.
     */
    void visitLoopInitialization(ASTNode* node) {
        switch (node->type) {
            case AST_DO_WHILE:
            case AST_WHILE:
                break;
            case AST_FOR_IN:
            case AST_FOR_IN_DECLARED:
            {
                CFGPartialType* containerType = visitExpression(node->child2);
                assert(
                    containerType->getType()->getNumDimensions() > 0 ||
                    !"TODO classes");
                CFGPartialType* elementType = getElementType(containerType);
                setVarValueType(node->child1, elementType);
                break;
            }
            case AST_FOR:
                visitStatementList(node->child1);
                break;
            default:
                assert(!"Unhandled loop type");
        }
    }
    
    /**
     * Visits the portion of the specified loop node (a while or for loop) that
     * we execute unconditionally and we also execute in the iterations of the
     * loop.
     */
    void visitLoopInitializationAndIteration(ASTNode* node) {
        switch (node->type) {
            case AST_DO_WHILE:
                visitStatement(node->child1);
                visitExpression(node->child2);
                break;
            case AST_FOR:
                visitExpression(node->child2);
                break;
            case AST_FOR_IN:
            case AST_FOR_IN_DECLARED:
                break;
            case AST_WHILE:
                visitExpression(node->child1);
                break;
            default:
                assert(!"Unhandled loop type");
        }
    }
    
    /**
     * Visits the iteration portion of the specified loop node (a while or
     * for loop), including any portion that we also execute unconditionally.
     */
    void visitLoopIteration(ASTNode* node) {
        pushBranch();
        switch (node->type) {
            case AST_DO_WHILE:
            {
                visitStatement(node->child1);
                CFGPartialType* type = visitExpression(node->child2);
                if (!type->getType()->isBool())
                    emitError(
                        node->child2,
                        "Condition must be a boolean value");
                break;
            }
            case AST_FOR:
            {
                visitStatement(node->child4);
                visitStatementList(node->child3);
                CFGPartialType* type = visitExpression(node->child2);
                if (!type->getType()->isBool())
                    emitError(
                        node->child2,
                        "Condition must be a boolean value");
                break;
            }
            case AST_FOR_IN:
            case AST_FOR_IN_DECLARED:
                visitStatement(node->child3);
                break;
            case AST_WHILE:
            {
                CFGPartialType* type = visitExpression(node->child1);
                if (!type->getType()->isBool())
                    emitError(
                        node->child1,
                        "Condition must be a boolean value");
                visitStatement(node->child2);
                break;
            }
            default:
                assert(!"Unhandled loop type");
        }
        popBranch();
    }
    
    /**
     * Deallocates the unneeded linked list nodes we just produced in a "trial
     * run" iteration of a loop.  See the comments for "isCheckingLoopStack" for
     * more information regarding trial runs.  This has the effect of deleting
     * the newly created linked list nodes that only appear in
     * "incomingVarTypesStack" or "returnNodesToDelete".  It also condenses the
     * list varTypesLinkedListStack.back() by removing and deallocating nodes
     * containing mappings that are superceded by later mappings.
     * 
     * The purpose of deallocating such nodes immediately rather than at the end
     * of the call to "evaluateTypes" is to achieve memory usage of O(n).
     * 
     * @param varTypesLinkedListEnd the earliest node in
     *     varTypesLinkedListStack.back() that we should avoid deallocating,
     *     because we need it elsewhere.
     */
    void condenseLoopIterationLinkedLists(
        LinkedList<pair<int, CFGPartialType*> >* varTypesLinkedListEnd) {
        LinkedList<pair<int, CFGPartialType*> >* varTypesLinkedListStart =
            varTypesLinkedListStack.back();
        map<int, LinkedList<pair<int, CFGPartialType*> >*> varIDToNode;
        set<LinkedList<pair<int, CFGPartialType*> >*> nodesToRemove;
        for (LinkedList<pair<int, CFGPartialType*> >* node =
                 varTypesLinkedListStart;
             node != varTypesLinkedListEnd;
             node = node->getNext()) {
            if (varIDToNode.count(node->getValue().first) > 0)
                nodesToRemove.insert(varIDToNode[node->getValue().first]);
            varIDToNode[node->getValue().first] = node;
        }
        set<LinkedList<pair<int, CFGPartialType*> >*> nodesToKeep;
        set<LinkedList<pair<int, CFGPartialType*> >*> nodesToDelete;
        LinkedList<pair<int, CFGPartialType*> >* node = varTypesLinkedListStart;
        while (nodesToRemove.count(node) > 0)
            node = node->getNext();
        while (node != varTypesLinkedListEnd) {
            LinkedList<pair<int, CFGPartialType*> >* next = node->getNext();
            while (nodesToRemove.count(next) > 0) {
                nodesToDelete.insert(next);
                next = next->getNext();
            }
            node->setNext(next);
            nodesToKeep.insert(node);
            node = next;
        }
        
        vector<LinkedList<pair<int, CFGPartialType*> >*> listsToDelete;
        for (map<int, int>::const_iterator iterator =
                 prevIncomingSizesStack.back()->begin();
             iterator != prevIncomingSizesStack.back()->end();
             iterator++) {
            while ((int)incomingVarTypesStack[iterator->first]->size() >
                   iterator->second) {
                listsToDelete.push_back(
                    incomingVarTypesStack[iterator->first]->back());
                incomingVarTypesStack[iterator->first]->pop_back();
            }
        }
        prevIncomingSizesStack.back()->clear();
        for (vector<LinkedList<pair<int, CFGPartialType*> >*>::const_iterator
                 iterator = returnNodesToDelete.begin();
             iterator != returnNodesToDelete.end();
             iterator++)
            listsToDelete.push_back(*iterator);
        returnNodesToDelete.clear();
        
        for (vector<LinkedList<pair<int, CFGPartialType*> >*>::const_iterator
                 iterator = listsToDelete.begin();
             iterator != listsToDelete.end();
             iterator++) {
            for (LinkedList<pair<int, CFGPartialType*> >* node = *iterator;
                 node != varTypesLinkedListEnd;
                 node = node->getNext()) {
                if (nodesToKeep.count(node) == 0)
                    nodesToDelete.insert(node);
            }
        }
        for (set<LinkedList<pair<int, CFGPartialType*> >*>::const_iterator
                 iterator = nodesToDelete.begin();
             iterator != nodesToDelete.end();
             iterator++) {
            linkedListNodes.erase(*iterator);
            if ((*iterator)->getValue().second != NULL)
                decrementReferenceCount((*iterator)->getValue().second);
            delete *iterator;
        }
    }
    
    /**
     * Pops an element from "prevIncomingSizesStack", updating the new last
     * element of "prevIncomingSizesStack" to include the popped mappings.
     */
    void popIncomingSizes() {
        map<int, int>* oldSizes = prevIncomingSizesStack.back();
        prevIncomingSizesStack.pop_back();
        map<int, int>* sizes = prevIncomingSizesStack.back();
        prevIncomingSizesStack.pop_back();
        map<int, int>* largeMap;
        map<int, int>* smallMap;
        if (sizes->size() > oldSizes->size()) {
            largeMap = sizes;
            smallMap = oldSizes;
        } else {
            largeMap = oldSizes;
            smallMap = sizes;
        }
        for (map<int, int>::const_iterator iterator = smallMap->begin();
             iterator != smallMap->end();
             iterator++) {
            if (sizes->count(iterator->first) > 0)
                // The mappings from "sizes" take precedence over those from
                // "oldSizes"
                (*largeMap)[iterator->first] = (*sizes)[iterator->first];
            else
                (*largeMap)[iterator->first] = iterator->second;
        }
        delete smallMap;
        prevIncomingSizesStack.push_back(largeMap);
    }
    
    /**
     * Visits the specified loop node node (a while or for loop).  The method's
     * implementation may require us to visit the loop's body multiple times;
     * see the comments at the top of the file.
     */
    void visitLoop(ASTNode* node) {
        // Visit the loop's initialization
        visitLoopInitialization(node);
        pushBreakLevel();
        pushContinueLevel();
        LinkedList<pair<int, CFGPartialType*> >* varTypesLinkedList =
            varTypesLinkedListStack.back();
        prevIncomingSizesStack.push_back(new map<int, int>());
        isCheckingLoopStack.push_back(true);
        if (node->type != AST_DO_WHILE) {
            visitLoopInitializationAndIteration(node);
            hasChangedStack.push_back(true);
        } else {
            hasChangedStack.push_back(false);
            visitLoopInitializationAndIteration(node);
            if (hasChangedStack.back()) {
                // Propagate the "hasChanged" flag down the stack
                hasChangedStack.pop_back();
                hasChangedStack.pop_back();
                hasChangedStack.push_back(true);
                hasChangedStack.push_back(true);
            }
        }
        
        // Repeatedly visit the loop's body until this did not cause us to
        // change any variable types
        bool isFirst = true;
        while (hasChangedStack.back()) {
            if (isFirst) {
                condenseLoopIterationLinkedLists(varTypesLinkedList);
                isFirst = false;
            }
            hasChangedStack.pop_back();
            hasChangedStack.push_back(false);
            visitLoopIteration(node);
            if (hasChangedStack.back()) {
                // Propagate the "hasChanged" flag down the stack
                hasChangedStack.pop_back();
                hasChangedStack.pop_back();
                hasChangedStack.push_back(true);
                hasChangedStack.push_back(true);
            }
        }
        
        popIncomingSizes();
        isCheckingLoopStack.pop_back();
        hasChangedStack.pop_back();
        if (!isCheckingLoopStack.back())
            // Visit the loop's body again, but emit errors this time.  (The
            // condition is essential.  Without it, TypeEvaluator would take
            // exponential time.)
            visitLoopIteration(node);
        incomingVarTypesStack.back()->push_back(varTypesLinkedListStack.back());
        popBreakLevel();
        incomingVarTypesStack.back()->push_back(varTypesLinkedListStack.back());
        popContinueLevel();
    }
    
    /**
     * Visits an expression list node (the "caseList" rule in grammar.y).
     * @param node the node to visit.
     * @param hasDefaultLabel a reference to set to true if we encounter a
     *     default label.
     */
    void visitCaseList(ASTNode* node, bool& hasDefaultLabel) {
        if (node->type == AST_EMPTY_CASE_LIST)
            return;
        assert(node->type == AST_CASE_LIST || !"Not a case list node");
        visitCaseList(node->child1, hasDefaultLabel);
        if (node->child2->type == AST_CASE_LABEL_DEFAULT)
            hasDefaultLabel = true;
        else {
            visitExpression(node->child2->child1);
            string str = node->child2->child1->tokenStr;
            if (str.at(str.length() - 1) == 'l' ||
                str.at(str.length() - 1) == 'L')
                emitError(node->child2, "Type of case value must be Int");
        }
        if (node->child3->type != AST_EMPTY_STATEMENT_LIST) {
            pushBranch();
            visitStatementList(node->child3);
            popBranch();
        }
    }
    
    /**
     * Visits the specified selection statement node (an if or switch
     * statement).
     */
    void visitSelectionStatement(ASTNode* node) {
        switch (node->type) {
            case AST_IF:
            {
                CFGPartialType* type = visitExpression(node->child1);
                if (!type->getType()->isBool())
                    emitError(
                        node->child1,
                        "Condition must be a boolean value");
                pushBranch();
                visitStatement(node->child2);
                popBranch();
                break;
            }
            case AST_IF_ELSE:
            {
                CFGPartialType* type = visitExpression(node->child1);
                if (!type->getType()->isBool())
                    emitError(
                        node->child1,
                        "Condition must be a boolean value");
                pushBranch();
                visitStatement(node->child2);
                bool isTrueBranchReachable = reverseVarTypesStack.back() !=
                    NULL;
                LinkedList<pair<int, CFGPartialType*> >* trueVarTypes =
                    popBranch();
                pushBranch();
                visitStatement(node->child3);
                bool isFalseBranchReachable = reverseVarTypesStack.back() !=
                    NULL;
                LinkedList<pair<int, CFGPartialType*> >* falseVarTypes =
                    popBranch();
                vector<LinkedList<pair<int, CFGPartialType*> >*>
                    incomingVarTypes;
                if (isTrueBranchReachable)
                    incomingVarTypes.push_back(trueVarTypes);
                if (isFalseBranchReachable)
                    incomingVarTypes.push_back(falseVarTypes);
                mergeIncomingBranches(incomingVarTypes);
                break;
            }
            case AST_SWITCH:
            {
                // Unlike with AST_IF_ELSE, we don't need to explicitly call
                // "mergeIncomingBranches" or get variable type maps.  The call
                // to popBreakLevel() magically makes the relevant type updates.
                pushBreakLevel();
                CFGPartialType* type = visitExpression(node->child1);
                if (!type->getType()->isIntegerLike() ||
                    type->getType()->getClassName() == "Long")
                    emitError(node->child1, "Operand must be an Int or Byte");
                bool hasDefaultLabel = false;
                visitCaseList(node->child2, hasDefaultLabel);
                if (!hasDefaultLabel) {
                    incomingVarTypesStack.back()->push_back(
                        varTypesLinkedListStack.back());
                }
                popBreakLevel();
                break;
            }
            default:
                assert(!"Unhanded selection statement type");
        }
    }
    
    /**
     * Visits the specified variable declaration item node (the
     * "varDeclarationItem" rule in grammar.y).
     */
    void visitVarDeclarationItem(ASTNode* node) {
        if (node->type == AST_ASSIGNMENT_EXPRESSION)
            visitExpression(node);
        else
            assert(
                node->type == AST_IDENTIFIER ||
                !"Not a variable declaration node");
    }
    
    /**
     * Visits the specified variable declaration list node (the
     * "varDeclarationList" rule in grammar.y).
     */
    void visitVarDeclarationList(ASTNode* node) {
        if (node->type != AST_VAR_DECLARATION_LIST)
            visitVarDeclarationItem(node);
        else {
            visitVarDeclarationList(node->child1);
            visitVarDeclarationItem(node->child2);
        }
    }
    
    /**
     * Visits the specified statement node (the "statement" rule in grammar.y).
     */
    void visitStatement(ASTNode* node) {
        switch (node->type) {
            case AST_ASSIGNMENT_EXPRESSION:
            case AST_POST_DECREMENT:
            case AST_POST_INCREMENT:
            case AST_PRE_DECREMENT:
            case AST_PRE_INCREMENT:
                visitExpression(node);
                break;
            case AST_BLOCK:
                visitStatementList(node->child1);
                break;
            case AST_BREAK:
            case AST_CONTINUE:
            case AST_RETURN:
                visitControlFlowStatement(node);
                break;
            case AST_DO_WHILE:
            case AST_FOR:
            case AST_FOR_IN:
            case AST_WHILE:
                visitLoop(node);
                break;
            case AST_EMPTY_STATEMENT:
                break;
            case AST_IF:
            case AST_IF_ELSE:
            case AST_SWITCH:
                visitSelectionStatement(node);
                break;
            case AST_METHOD_CALL:
                visitMethodCall(node);
                break;
            case AST_TARGETED_METHOD_CALL:
                assert(!"TODO classes");
                break;
            case AST_VAR_DECLARATION:
                visitVarDeclarationList(node->child1);
                break;
            default:
                assert(!"Unhandled statement type");
                break;
        }
    }
    
    /**
     * Visits the specified statement list node (the "statementList" rule in
     * grammar.y).
     */
    void visitStatementList(ASTNode* node) {
        if (node->type == AST_EMPTY_STATEMENT_LIST)
            return;
        assert(
            node->type == AST_STATEMENT_LIST || !"Not a statement list node");
        visitStatementList(node->child1);
        visitStatement(node->child2);
    }
    
    /**
     * Visits the specified node of type AST_ARG.
     */
    void visitArg(ASTNode* node) {
        assert(node->type == AST_ARG || !"Not an arg node");
        string identifier = node->child2->tokenStr;
        if (argTypes.count(identifier) == 0)
            argTypes[identifier] = ASTUtil::getCFGType(node->child1);
        if (node->child3 != NULL)
            visitExpression(node->child3);
    }
    
    /**
     * Visits the specified argument list node (the "argList" rule in
     * grammar.y).
     */
    void visitArgList(ASTNode* node) {
        if (node->type == AST_ARG)
            visitArg(node);
        else {
            assert(node->type == AST_ARG_LIST || !"Not an arg list node");
            visitArgList(node->child1);
            visitArg(node->child2);
        }
    }
public:
    ~TypeEvaluatorImpl() {
        set<CFGPartialType*> types;
        for (map<ASTNode*, CFGPartialType*>::const_iterator iterator =
                 nodeTypes.begin();
             iterator != nodeTypes.end();
             iterator++)
            types.insert(iterator->second);
        for (map<int, CFGPartialType*>::const_iterator iterator =
                 allVarTypes.begin();
             iterator != allVarTypes.end();
             iterator++)
            types.insert(iterator->second);
        for (set<CFGPartialType*>::const_iterator iterator = types.begin();
             iterator != types.end();
             iterator++)
            delete *iterator;
        for (map<string, CFGType*>::const_iterator iterator = argTypes.begin();
             iterator != argTypes.end();
             iterator++)
            delete iterator->second;
    }
    
    void evaluateTypes(
        ASTNode* node,
        map<string, CFGType*>& fieldTypes2,
        map<ASTNode*, int>& varIDs2,
        map<string, MethodInterface*>& methodInterfaces2,
        CompilerErrors* errors2) {
        assert(node->type == AST_METHOD_DEFINITION || !"Not a method node");
        fieldTypes = &fieldTypes2;
        varIDs = varIDs2;
        methodInterfaces = &methodInterfaces2;
        errors = errors2;
        if (node->child1->type != AST_VOID)
            returnType = ASTUtil::getCFGType(node->child1);
        else
            returnType = NULL;
        varTypesLinkedListStack.push_back(NULL);
        prevIncomingSizesStack.push_back(new map<int, int>());
        isCheckingLoopStack.push_back(false);
        hasChangedStack.push_back(false);
        pushBranch();
        if (node->child4 == NULL)
            visitStatementList(node->child3);
        else {
            visitArgList(node->child3);
            visitStatementList(node->child4);
        }
        
        for (map<string, CFGType*>::const_iterator iterator = argTypes.begin();
             iterator != argTypes.end();
             iterator++)
            delete iterator->second;
        argTypes.clear();
        delete returnType;
        isCheckingLoopStack.pop_back();
        hasChangedStack.pop_back();
        popBranch();
        delete prevIncomingSizesStack.at(0);
        prevIncomingSizesStack.clear();
        returnNodesToDelete.clear();
        for (set<LinkedList<pair<int, CFGPartialType*> >*>::const_iterator
                 iterator = linkedListNodes.begin();
             iterator != linkedListNodes.end();
             iterator++) {
            if ((*iterator)->getValue().second != NULL)
                decrementReferenceCount((*iterator)->getValue().second);
            delete *iterator;
        }
        linkedListNodes.clear();
    }
    
    CFGReducedType getExpressionType(ASTNode* node) {
        assert(
            nodeTypes.count(node) > 0 ||
            !"Missing expression type.  Either the node is not an expression "
            "or there is a bug in TypeEvaluator.");
        return nodeTypes[node]->getType()->getReducedType();
    }
};

TypeEvaluator::TypeEvaluator() {
    impl = new TypeEvaluatorImpl();
}

TypeEvaluator::~TypeEvaluator() {
    delete impl;
}

void TypeEvaluator::evaluateTypes(
    ASTNode* node,
    map<string, CFGType*>& fieldTypes,
    map<ASTNode*, int>& varIDs,
    map<string, MethodInterface*>& methodInterfaces,
    CompilerErrors* errors) {
    impl->evaluateTypes(node, fieldTypes, varIDs, methodInterfaces, errors);
}

CFGReducedType TypeEvaluator::getExpressionType(ASTNode* node) {
    return impl->getExpressionType(node);
}
