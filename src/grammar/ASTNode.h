#ifndef AST_NODE_H_INCLUDED
#define AST_NODE_H_INCLUDED

/**
 * The type of an AST node.  See grammar.y regarding the precise structure of an
 * AST.
 */
enum ASTType {
    AST_AND_ASSIGN,
    AST_ARG,
    AST_ARG_LIST,
    AST_ARRAY,
    AST_ARRAY_GET,
    AST_ASSIGN,
    AST_ASSIGNMENT_EXPRESSION,
    AST_AUTO,
    AST_BITWISE_AND,
    AST_BITWISE_INVERT,
    AST_BITWISE_OR,
    AST_BLOCK,
    AST_BOOLEAN_AND,
    AST_BOOLEAN_OR,
    AST_BRACKET_EXPRESSION,
    AST_BRACKET_EXPRESSION_LIST,
    AST_BREAK,
    AST_CASE_LABEL,
    AST_CASE_LABEL_DEFAULT,
    AST_CASE_LIST,
    AST_CAST,
    AST_CLASS_DEFINITION,
    AST_CLASS_BODY_ITEM_LIST,
    AST_CONTINUE,
    AST_DIV,
    AST_DIV_ASSIGN,
    AST_DO_WHILE,
    AST_EMPTY_CASE_LIST,
    AST_EMPTY_CLASS_BODY_ITEM_LIST,
    AST_EMPTY_STATEMENT,
    AST_EMPTY_STATEMENT_LIST,
    AST_EQUALS,
    AST_EXPRESSION_LIST,
    AST_FALSE,
    AST_FIELD,
    AST_FILE,
    AST_FLOAT_LITERAL,
    AST_FOR,
    AST_FOR_IN,
    AST_GREATER_THAN,
    AST_GREATER_THAN_OR_EQUAL_TO,
    AST_IDENTIFIER,
    AST_IF,
    AST_IF_ELSE,
    AST_INT_LITERAL,
    AST_LEFT_SHIFT,
    AST_LEFT_SHIFT_ASSIGN,
    AST_LESS_THAN,
    AST_LESS_THAN_OR_EQUAL_TO,
    AST_METHOD_CALL,
    AST_METHOD_DEFINITION,
    AST_MINUS,
    AST_MINUS_ASSIGN,
    AST_MOD,
    AST_MOD_ASSIGN,
    AST_MULT,
    AST_MULT_ASSIGN,
    AST_NEGATE,
    AST_NEW,
    AST_NOT,
    AST_NOT_EQUALS,
    AST_OR_ASSIGN,
    AST_PARENTHESES,
    AST_PLUS,
    AST_PLUS_ASSIGN,
    AST_POST_DECREMENT,
    AST_POST_INCREMENT,
    AST_PRE_DECREMENT,
    AST_PRE_INCREMENT,
    AST_QUALIFIED_IDENTIFIER,
    AST_RETURN,
    AST_RIGHT_SHIFT,
    AST_RIGHT_SHIFT_ASSIGN,
    AST_STATEMENT_LIST,
    AST_SWITCH,
    AST_TARGETED_METHOD_CALL,
    AST_TERNARY,
    AST_TRUE,
    AST_TYPE,
    AST_TYPE_ARRAY,
    AST_UNSIGNED_RIGHT_SHIFT,
    AST_UNSIGNED_RIGHT_SHIFT_ASSIGN,
    AST_VAR_DECLARATION,
    AST_VAR_DECLARATION_LIST,
    AST_VOID,
    AST_WHILE,
    AST_XOR,
    AST_XOR_ASSIGN
};

/**
 * A node in the AST (abstract syntax tree) for a code file.
 */
typedef struct ASTNodeStruct {
    /**
     * The ASTType of the node.
     */
    int type;
    /**
     * If this node indicates a token, the text of the token.
     */
    char* tokenStr;
    /**
     * The first child of this node, as ordered in the source code, if any.
     */
    struct ASTNodeStruct* child1;
    /**
     * The second child of this node, as ordered in the source code, if any.
     */
    struct ASTNodeStruct* child2;
    /**
     * The third child of this node, as ordered in the source code, if any.
     */
    struct ASTNodeStruct* child3;
    /**
     * The fourth child of this node, as ordered in the source code, if any.
     */
    struct ASTNodeStruct* child4;
    /**
     * The line number of the last character in the source file that produced
     * this node.
     */
    int lineNumber;
} ASTNode;

#define YYSTYPE ASTNode*

/**
 * Returns a new AST for a token.
 */
ASTNode* astNewStr(int type, const char* tokenStr);
/**
 * Returns a new AST with no children.
 */
ASTNode* astNew0(int type);
/**
 * Returns a new AST with one child.
 */
ASTNode* astNew1(int type, ASTNode* child1);
/**
 * Returns a new AST with two children.
 */
ASTNode* astNew2(int type, ASTNode* child1, ASTNode* child2);
/**
 * Returns a new AST with three children.
 */
ASTNode* astNew3(int type, ASTNode* child1, ASTNode* child2, ASTNode* child3);
/**
 * Returns a new AST with four children.
 */
ASTNode* astNew4(
    int type,
    ASTNode* child1,
    ASTNode* child2,
    ASTNode* child3,
    ASTNode* child4);
/**
 * Deallocates the specified AST.
 */
void astFree(ASTNode* node);

/**
 * Responds to the specified ASTNode's being allocated.  Note that it is
 * possible that not all of its fields have been initialized by the time we call
 * this method.
 */
void handleASTNodeCreated(ASTNode* node);

#endif
