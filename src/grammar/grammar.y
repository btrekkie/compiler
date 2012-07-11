%{
extern "C" {
    #include "ASTNode.h"
    #include "grammar.h"
    
    int yyparse();
    int yylex();
    void yyerror(const char* str);
}

#include <stdio.h>
#include "Compiler.hpp"

#define YYSTYPE ASTNode*

extern char yytext[];
extern int yylineno;
%}

%start file
%token AUTO BREAK CASE CLASS CONTINUE DEFAULT DO FALSE FOR IF LENGTH NEW RETURN
%token SWITCH TRUE VOID WHILE
%token BOOLEAN_AND BOOLEAN_OR
%token EQUALS NOT_EQUALS
%token LESS_THAN_OR_EQUAL_TO GREATER_THAN_OR_EQUAL_TO
%token LEFT_SHIFT RIGHT_SHIFT UNSIGNED_RIGHT_SHIFT
%token DECREMENT INCREMENT OPEN_AND_CLOSE_BRACKET
%token AND_ASSIGN DIV_ASSIGN LEFT_SHIFT_ASSIGN MINUS_ASSIGN MOD_ASSIGN
%token MULT_ASSIGN OR_ASSIGN PLUS_ASSIGN RIGHT_SHIFT_ASSIGN
%token UNSIGNED_RIGHT_SHIFT_ASSIGN XOR_ASSIGN
%token FLOAT_LITERAL IDENTIFIER INT_LITERAL
%nonassoc IFX
%nonassoc ELSE
%%

file : classDefinition
       { $$ = astNew1(AST_FILE, $1); processFile($$); };
classDefinition : CLASS IDENTIFIER '{' classBodyItemList '}'
                  { $$ = astNew2(AST_CLASS_DEFINITION, $2, $4); };
classBodyItemList : classBodyItemList classBodyItem
                    { $$ = astNew2(AST_CLASS_BODY_ITEM_LIST, $1, $2); }
                  |
                    { $$ = astNew0(AST_EMPTY_CLASS_BODY_ITEM_LIST); };
classBodyItem : fieldDeclaration
                { $$ = $1; }
              | methodDefinition
                { $$ = $1; };
fieldDeclaration : varDeclaration
                   { $$ = $1; };
varDeclaration : type varDeclarationList ';'
                 { $$ = astNew2(AST_VAR_DECLARATION, $1, $2); };
varDeclarationList : varDeclarationList ',' varDeclarationItem
                     { $$ = astNew2(AST_VAR_DECLARATION_LIST, $1, $3); }
                   | varDeclarationItem
                     { $$ = $1; };
varDeclarationItem : IDENTIFIER
                     { $$ = $1; }
                   | IDENTIFIER '=' expression
                     { $$ = astNew3(
                           AST_ASSIGNMENT_EXPRESSION,
                           $1,
                           astNew0(AST_ASSIGN),
                           $3); };
type : AUTO
       { $$ = astNew0(AST_AUTO); }
     | nonAutoType
       { $$ = $1; };
nonAutoType : IDENTIFIER
              { $$ = astNew1(AST_TYPE, $1); }
            | type OPEN_AND_CLOSE_BRACKET
              { $$ = astNew1(AST_TYPE_ARRAY, $1); };
methodDefinition : type IDENTIFIER '(' argList ')' methodBody
                   { $$ = astNew4(
                         AST_METHOD_DEFINITION, $1, $2, $4, $6); }
                 | VOID IDENTIFIER '(' argList ')' methodBody
                   { $$ = astNew4(
                         AST_METHOD_DEFINITION,
                         astNew0(AST_VOID),
                         $2,
                         $4,
                         $6); }
                 | type IDENTIFIER '(' ')' methodBody
                   { $$ = astNew3(AST_METHOD_DEFINITION, $1, $2, $5); }
                 | VOID IDENTIFIER '(' ')' methodBody
                   { $$ = astNew3(
                         AST_METHOD_DEFINITION,
                         astNew0(AST_VOID),
                         $2,
                         $5); };
argList : argList ',' argItem
          { $$ = astNew2(AST_ARG_LIST, $1, $3); }
        | argItem
          { $$ = $1; };
argItem : type IDENTIFIER
          { $$ = astNew2(AST_ARG, $1, $2); }
        | type IDENTIFIER '=' expression
          { $$ = astNew3(AST_ARG, $1, $2, $4); };
methodBody : '{' statementList '}'
             { $$ = $2; };
statementList : statementList statement
                { $$ = astNew2(AST_STATEMENT_LIST, $1, $2); }
              |
                { $$ = astNew0(AST_EMPTY_STATEMENT_LIST); };
statement : ';'
            { $$ = astNew0(AST_EMPTY_STATEMENT); }
          | varDeclaration
            { $$ = $1; }
          | directAssignmentExpression ';'
            { $$ = $1; }
          | directIncrementExpression ';'
            { $$ = $1; }
          | methodCall ';'
            { $$ = $1; }
          | ifStatement
            { $$ = $1; }
          | switchStatement
            { $$ = $1; }
          | whileStatement
            { $$ = $1; }
          | doWhileStatement
            { $$ = $1; }
          | forStatement
            { $$ = $1; }
          | forInStatement
            { $$ = $1; }
          | controlFlowStatement
            { $$ = $1; }
          | '{' statementList '}'
            { $$ = astNew1(AST_BLOCK, $2); };
assignmentOperator : '='
                     { $$ = astNew0(AST_ASSIGN); }
                   | PLUS_ASSIGN
                     { $$ = astNew0(AST_PLUS_ASSIGN); }
                   | MINUS_ASSIGN
                     { $$ = astNew0(AST_MINUS_ASSIGN); }
                   | MULT_ASSIGN
                     { $$ = astNew0(AST_MULT_ASSIGN); }
                   | DIV_ASSIGN
                     { $$ = astNew0(AST_DIV_ASSIGN); }
                   | MOD_ASSIGN
                     { $$ = astNew0(AST_MOD_ASSIGN); }
                   | LEFT_SHIFT_ASSIGN
                     { $$ = astNew0(AST_LEFT_SHIFT_ASSIGN); }
                   | RIGHT_SHIFT_ASSIGN
                     { $$ = astNew0(AST_RIGHT_SHIFT_ASSIGN); }
                   | UNSIGNED_RIGHT_SHIFT_ASSIGN
                     { $$ = astNew0(AST_UNSIGNED_RIGHT_SHIFT_ASSIGN); }
                   | AND_ASSIGN
                     { $$ = astNew0(AST_AND_ASSIGN); }
                   | OR_ASSIGN
                     { $$ = astNew0(AST_OR_ASSIGN); }
                   | XOR_ASSIGN
                     { $$ = astNew0(AST_XOR_ASSIGN); };
directIncrementExpression : parenthesesExpression INCREMENT
                            { $$ = astNew1(AST_POST_INCREMENT, $1); }
                          | parenthesesExpression DECREMENT
                            { $$ = astNew1(AST_POST_DECREMENT, $1); }
                          | INCREMENT parenthesesExpression
                            { $$ = astNew1(AST_PRE_INCREMENT, $2); }
                          | DECREMENT parenthesesExpression
                            { $$ = astNew1(AST_PRE_DECREMENT, $2); };
ifStatement : IF '(' expression ')' statement %prec IFX
              { $$ = astNew2(AST_IF, $3, $5); }
            | IF '(' expression ')' statement ELSE statement
              { $$ = astNew3(AST_IF_ELSE, $3, $5, $7); };
switchStatement : SWITCH '(' expression ')' '{' caseList '}'
                  { $$ = astNew2(AST_SWITCH, $3, $6); }
caseList : caseList caseLabel ':' statementList
           { $$ = astNew3(AST_CASE_LIST, $1, $2, $4); }
         |
           { $$ = astNew0(AST_EMPTY_CASE_LIST); };
caseLabel : CASE INT_LITERAL
            { $$ = astNew1(AST_CASE_LABEL, $2); }
          | DEFAULT
            { $$ = astNew0(AST_CASE_LABEL_DEFAULT); };
whileStatement : WHILE '(' expression ')' statement
                 { $$ = astNew2(AST_WHILE, $3, $5); };
doWhileStatement : DO statement WHILE '(' expression ')' ';'
                   {$$ = astNew2(AST_DO_WHILE, $2, $5); };
forStatement : FOR '(' forStatementList ';' expression ';' forStatementList ')'
                 statement
               { $$ = astNew4(AST_FOR, $3, $5, $7, $9); };
forStatementList : forNonEmptyStatementList
                   { $$ = $1; }
                 |
                   { $$ = astNew0(AST_EMPTY_STATEMENT_LIST); };
forNonEmptyStatementList : forComponentStatement ',' forNonEmptyStatementList
                           { $$ = astNew2(AST_STATEMENT_LIST, $1, $3); }
                         | forComponentStatement
                           { $$ = astNew2(
                                 AST_STATEMENT_LIST,
                                 astNew0(AST_EMPTY_STATEMENT_LIST),
                                 $1); };
forComponentStatement : directAssignmentExpression
                        { $$ = $1; }
                      | directIncrementExpression
                        { $$ = $1; }
                      | methodCall
                        { $$ = $1; }
                      | type IDENTIFIER '=' expression
                        { $$ = astNew2(
                              AST_VAR_DECLARATION,
                              $1,
                              astNew3(
                                  AST_ASSIGNMENT_EXPRESSION,
                                  $2,
                                  astNew0(AST_ASSIGN),
                                  $4)); };
forInStatement : FOR '(' type IDENTIFIER ':' expression ')' statement
                 { $$ = astNew4(AST_FOR_IN, $3, $4, $6, $8); };
controlFlowStatement : BREAK ';'
                       { $$ = astNew0(AST_BREAK); }
                     | BREAK INT_LITERAL ';'
                       { $$ = astNew1(AST_BREAK, $2); }
                     | CONTINUE ';'
                       { $$ = astNew0(AST_CONTINUE); }
                     | CONTINUE INT_LITERAL ';'
                       { $$ = astNew1(AST_CONTINUE, $2); }
                     | RETURN ';'
                       { $$ = astNew0(AST_RETURN); }
                     | RETURN expression ';'
                       { $$ = astNew1(AST_RETURN, $2); };
methodCall : IDENTIFIER '(' expressionList ')'
             { $$ = astNew2(AST_METHOD_CALL, $1, $3); }
           | IDENTIFIER '(' ')'
             { $$ = astNew1(AST_METHOD_CALL, $1); };
expressionList : expressionList ',' expression
                 { $$ = astNew2(AST_EXPRESSION_LIST, $1, $3); }
               | expression
                 { $$ = $1; };
bracketExpressionList : bracketExpressionList '[' expression ']'
                        { $$ = astNew2(AST_BRACKET_EXPRESSION_LIST, $1, $3); }
                      | '[' expression ']'
                        { $$ = astNew1(AST_BRACKET_EXPRESSION, $2); };



expression : assignmentExpression
             { $$ = $1; };
directAssignmentExpression : parenthesesExpression assignmentOperator
                               assignmentExpression
                             { $$ = astNew3(
                                   AST_ASSIGNMENT_EXPRESSION, $1, $2, $3); };
assignmentExpression : directAssignmentExpression
                       { $$ = $1; }
                     | ternaryExpression
                       { $$ = $1; };
ternaryExpression : booleanOrExpression '?' ternaryExpression ':'
                      ternaryExpression
                    { $$ = astNew3(AST_TERNARY, $1, $3, $5); }
                  | booleanOrExpression
                    { $$ = $1; }
booleanOrExpression : booleanOrExpression BOOLEAN_OR booleanAndExpression
                       { $$ = astNew2(AST_BOOLEAN_OR, $1, $3); }
                    | booleanAndExpression
                       { $$ = $1; };
booleanAndExpression : booleanAndExpression BOOLEAN_AND equalityExpression
                       { $$ = astNew2(AST_BOOLEAN_AND, $1, $3); }
                     | equalityExpression
                       { $$ = $1; };
equalityExpression : equalityExpression EQUALS bitwiseOrExpression
                     { $$ = astNew2(AST_EQUALS, $1, $3); }
                   | equalityExpression NOT_EQUALS bitwiseOrExpression
                     { $$ = astNew2(AST_NOT_EQUALS, $1, $3); }
                   | bitwiseOrExpression
                     { $$ = $1; };
bitwiseOrExpression : bitwiseOrExpression '|' xorExpression
                      { $$ = astNew2(AST_BITWISE_OR, $1, $3); }
                    | xorExpression
                      { $$ = $1; };
xorExpression : xorExpression '^' bitwiseAndExpression
                { $$ = astNew2(AST_XOR, $1, $3); }
              | bitwiseAndExpression
                { $$ = $1; };
bitwiseAndExpression : bitwiseAndExpression '&' comparisonExpression
                       { $$ = astNew2(AST_BITWISE_AND, $1, $3); }
                     | comparisonExpression
                       { $$ = $1; };
comparisonExpression : comparisonExpression '<' bitShiftExpression
                       { $$ = astNew2(AST_LESS_THAN, $1, $3); }
                     | comparisonExpression '>' bitShiftExpression
                       { $$ = astNew2(AST_GREATER_THAN, $1, $3); }
                     | comparisonExpression LESS_THAN_OR_EQUAL_TO
                         bitShiftExpression
                       { $$ = astNew2(AST_LESS_THAN_OR_EQUAL_TO, $1, $3); }
                     | comparisonExpression GREATER_THAN_OR_EQUAL_TO
                         bitShiftExpression
                       { $$ = astNew2(AST_GREATER_THAN_OR_EQUAL_TO, $1, $3); }
                     | bitShiftExpression
                       { $$ = $1; };
bitShiftExpression : bitShiftExpression LEFT_SHIFT additionExpression
                     { $$ = astNew2(AST_LEFT_SHIFT, $1, $3); }
                   | bitShiftExpression RIGHT_SHIFT additionExpression
                     { $$ = astNew2(AST_RIGHT_SHIFT, $1, $3); }
                   | bitShiftExpression UNSIGNED_RIGHT_SHIFT
                       additionExpression
                     { $$ = astNew2(AST_UNSIGNED_RIGHT_SHIFT, $1, $3); }
                   | additionExpression
                     { $$ = $1; };
additionExpression : additionExpression '+' multiplicationExpression
                     { $$ = astNew2(AST_PLUS, $1, $3); }
                   | additionExpression '-' multiplicationExpression
                     { $$ = astNew2(AST_MINUS, $1, $3); }
                   | multiplicationExpression
                     { $$ = $1; };
multiplicationExpression : multiplicationExpression '*' castExpression
                           { $$ = astNew2(AST_MULT, $1, $3); }
                         | multiplicationExpression '/' castExpression
                           { $$ = astNew2(AST_DIV, $1, $3); }
                         | multiplicationExpression '%' castExpression
                           { $$ = astNew2(AST_MOD, $1, $3); }
                         | castExpression
                           { $$ = $1; };
castExpression : '(' expression ')' castFriendlyUnaryExpression
                 { $$ = astNew2(AST_CAST, $2, $4); }
               | castUnfriendlyUnaryExpression
                 { $$ = $1; };
castUnfriendlyUnaryExpression : directIncrementExpression
                                { $$ = $1; }
                              | '-' castUnfriendlyUnaryExpression
                                { $$ = astNew1(AST_NEGATE, $2); }
                              | '[' expressionList ']'
                                { $$ = astNew1(AST_ARRAY, $2); }
                              | OPEN_AND_CLOSE_BRACKET
                                { $$ = astNew0(AST_ARRAY); }
                              | castFriendlyUnaryExpression
                                { $$ = $1; };
castFriendlyUnaryExpression : '~' castUnfriendlyUnaryExpression
                              { $$ = astNew1(AST_BITWISE_INVERT, $2); }
                            | '!' castUnfriendlyUnaryExpression
                              { $$ = astNew1(AST_NOT, $2); }
                            | NEW IDENTIFIER bracketExpressionList
                              { $$ = astNew2(AST_NEW, $2, $3); }
                            | parenthesesExpression
                              { $$ = $1; };
parenthesesExpression : '(' expression ')'
                        { $$ = $2; }
                      | parenthesesExpression '[' expression ']'
                        { $$ = astNew2(AST_ARRAY_GET, $1, $3); }
                      | parenthesesExpression '.' LENGTH
                        { $$ = astNew1(AST_ARRAY_LENGTH, $1); }
                      | methodCall
                        { $$ = $1; }
                      | IDENTIFIER
                        { $$ = $1; }
                      | INT_LITERAL
                        { $$ = $1; }
                      | FLOAT_LITERAL
                        { $$ = $1; }
                      | TRUE
                        { $$ = astNew0(AST_TRUE); }
                      | FALSE
                        { $$ = astNew0(AST_FALSE); };
