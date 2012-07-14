#include <assert.h>
#include <vector>
#include "CompilerErrors.hpp"
#include "VarResolver.hpp"

using namespace std;

/**
 * A class used to implement the VarResolver method.
 */
class VarResolverImpl {
private:
    set<string>* fieldIdentifiers;
    /**
     * A set of the method's arguments' identifiers.
     */
    set<string> argIdentifiers;
    /**
     * The CompilerErrors object we are using to emit compiler errors.
     */
    CompilerErrors* errors;
    /**
     * A map from the identifiers of all of the local (non-argument) variables
     * we have encountered thus far to their corresponding ids.
     */
    map<string, int> allVars;
    /**
     * A stack of sets indicating the variables in the current frame / scope and
     * its ancestors.
     */
    vector<set<string>*> frameVars;
    /**
     * A map from the variable nodes we have visited to the ids of the
     * variables.
     */
    map<ASTNode*, int> nodeToVar;
    /**
     * The next variable id we will use.
     */
    int nextVarID;
    
    /**
     * Pushes a frame / scope for variables to the stack of frames.
     */
    void pushFrame() {
        frameVars.push_back(new set<string>());
    }
    
    /**
     * Pops a frame / scope for variables from the stack of frames.
     */
    void popFrame() {
        set<string>* frame = frameVars.back();
        for (set<string>::const_iterator iterator = frame->begin();
             iterator != frame->end();
             iterator++)
            allVars.erase(*iterator);
        delete frame;
        frameVars.pop_back();
    }
    
    /**
     * Creates the local (non-argument) variable indicated by the specified node
     * of type AST_IDENTIFIER.  Emits a "multiple variables with the same
     * identifier" error if appropriate.
     */
    void createVar(ASTNode* node) {
        assert(node->type == AST_IDENTIFIER || !"Not a variable");
        string identifier = node->tokenStr;
        int id = nextVarID;
        nextVarID++;
        nodeToVar[node] = id;
        if (argIdentifiers.count(identifier) > 0 ||
            allVars.count(identifier) > 0)
            errors->emitError(
                node,
                "Multiple variables with the same identifier");
        else {
            allVars[identifier] = id;
            frameVars.back()->insert(identifier);
        }
    }
    
    /**
     * Returns the id of the variable indicated by the specified node of type
     * AST_IDENTIFIER, or -1 if it does not indicate a local (non-argument)
     * variable.  Emits an "undeclared variable" error if appropriate.
     */
    int getVarID(ASTNode* node) {
        assert(node->type == AST_IDENTIFIER || !"Not a variable");
        string identifier = node->tokenStr;
        if (fieldIdentifiers->count(identifier) > 0 ||
            argIdentifiers.count(identifier) > 0)
            return -1;
        else if (allVars.count(identifier) > 0)
            return allVars[identifier];
        else {
            errors->emitError(node, "Variable not declared in this scope");
            return -1;
        }
    }
    
    /**
     * Resolves variables in the specified variable declaration item node (the
     * "varDeclarationItem" rule in grammar.y) and its descendants.
     */
    void visitVarDeclarationItem(ASTNode* node) {
        if (node->type == AST_IDENTIFIER)
            createVar(node);
        else {
            assert(
                node->type == AST_ASSIGNMENT_EXPRESSION ||
                !"Not a variable declaration item node");
            createVar(node->child1);
            visitNode(node->child3);
        }
    }
    
    /**
     * Resolves variables in the specified node of type AST_VAR_DECLARATION_LIST
     * and its descendants.
     */
    void visitVarDeclarationList(ASTNode* node) {
        if (node->type == AST_VAR_DECLARATION_LIST) {
            visitVarDeclarationList(node->child1);
            visitVarDeclarationItem(node->child2);
        } else
            visitVarDeclarationItem(node);
    }
    
    /**
     * Resolves variables in the specified node and its descendants.
     */
    void visitNode(ASTNode* node) {
        bool shouldPushFrame;
        switch (node->type) {
            case AST_ARG:
            {
                string identifier = node->child2->tokenStr;
                if (argIdentifiers.count(identifier) == 0)
                    argIdentifiers.insert(identifier);
                else
                    errors->emitError(
                        node,
                        "Multiple method arguments with the same identifier");
                if (node->child3 != NULL)
                    visitNode(node->child3);
                return;
            }
            case AST_BLOCK:
            case AST_DO_WHILE:
            case AST_FOR:
            case AST_FOR_IN:
            case AST_SWITCH:
            case AST_WHILE:
                shouldPushFrame = true;
                break;
            case AST_IDENTIFIER:
                nodeToVar[node] = getVarID(node);
                return;
            case AST_METHOD_CALL:
                if (node->child2 != NULL)
                    visitNode(node->child2);
                return;
            case AST_TARGETED_METHOD_CALL:
                visitNode(node->child1);
                if (node->child3 != NULL)
                    visitNode(node->child3);
                return;
            case AST_TYPE:
            case AST_TYPE_ARRAY:
                return;
            case AST_VAR_DECLARATION:
                visitVarDeclarationList(node->child2);
                return;
            default:
                shouldPushFrame = false;
        }
        if (shouldPushFrame)
            pushFrame();
        if (node->type == AST_FOR_IN) {
            createVar(node->child2);
            visitNode(node->child3);
            visitNode(node->child4);
        } else if (node->child1 != NULL) {
            visitNode(node->child1);
            if (node->child2 != NULL) {
                visitNode(node->child2);
                if (node->child3 != NULL) {
                    visitNode(node->child3);
                    if (node->child4 != NULL)
                        visitNode(node->child4);
                }
            }
        }
        if (shouldPushFrame)
            popFrame();
    }
public:
    map<ASTNode*, int> resolveVars(
        ASTNode* node,
        set<string>& fieldIdentifiers2,
        CompilerErrors* errors2) {
        assert(node->type == AST_METHOD_DEFINITION || !"Not a method node");
        fieldIdentifiers = &fieldIdentifiers2;
        errors = errors2;
        nextVarID = 0;
        pushFrame();
        visitNode(node->child3);
        if (node->child4 != NULL)
            visitNode(node->child4);
        popFrame();
        return nodeToVar;
    }
};

map<ASTNode*, int> VarResolver::resolveVars(
    ASTNode* node,
    set<string>& fieldIdentifiers,
    CompilerErrors* errors) {
    VarResolverImpl impl;
    return impl.resolveVars(node, fieldIdentifiers, errors);
}
