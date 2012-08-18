INT ([1-9][0-9]*|0)
EXP [eE][+-]?{INT}

%option yylineno

%{
#include <stdio.h>
#include "ASTNode.h"
#include "grammar.h"
#include "grammar.hpp"

/**
 * Inputs and ignores characters until we reach the current block comment.
 */
void blockComment();
%}

%%

"/*" { blockComment(); }
\/\/[^\n]*\n { }
"break" { return BREAK; }
"case" { return CASE; }
"class" { return CLASS; }
"continue" { return CONTINUE; }
"default" { return DEFAULT; }
"do" { return DO; }
"else" { return ELSE; }
"false" { return FALSE; }
"for" { return FOR; }
"if" { return IF; }
"new" { return NEW; }
"return" { return RETURN; }
"switch" { return SWITCH; }
"true" { return TRUE; }
"var" { return VAR; }
"void" { return VOID; }
"while" { return WHILE; }

"&=" { return AND_ASSIGN; }
"&&" { return BOOLEAN_AND; }
"||" { return BOOLEAN_OR; }
"--" { return DECREMENT; }
"/=" { return DIV_ASSIGN; }
"==" { return EQUALS; }
">=" { return GREATER_THAN_OR_EQUAL_TO; }
"++" { return INCREMENT; }
"<<" { return LEFT_SHIFT; }
"<<=" { return LEFT_SHIFT_ASSIGN; }
"<=" { return LESS_THAN_OR_EQUAL_TO; }
"-=" { return MINUS_ASSIGN; }
"%=" { return MOD_ASSIGN; }
"*=" { return MULT_ASSIGN; }
"!=" { return NOT_EQUALS; }
"|=" { return OR_ASSIGN; }
"+=" { return PLUS_ASSIGN; }
">>" { return RIGHT_SHIFT; }
">>=" { return RIGHT_SHIFT_ASSIGN; }
">>>" { return UNSIGNED_RIGHT_SHIFT; }
">>>=" { return UNSIGNED_RIGHT_SHIFT_ASSIGN; }
"^=" { return XOR_ASSIGN; }

"(" { return '('; }
")" { return ')'; }
"[" { return '['; }
"]" { return ']'; }
"{" { return '{'; }
"}" { return '}'; }
"<" { return '<'; }
">" { return '>'; }
"=" { return '='; }
"," { return ','; }
";" { return ';'; }
"&" { return '&'; }
"|" { return '|'; }
"^" { return '^'; }
":" { return ':'; }
"+" { return '+'; }
"-" { return '-'; }
"*" { return '*'; }
"/" { return '/'; }
"%" { return '%'; }
"~" { return '~'; }
"!" { return '!'; }
"." { return '.'; }
"?" { return '?'; }

[a-zA-Z_][a-zA-Z_0-9]* {
    yylval = astNewStr(AST_IDENTIFIER, yytext);
    return IDENTIFIER;
}
-?0[xX][a-fA-F0-9]+[l|L]? {
    yylval = astNewStr(AST_INT_LITERAL, yytext);
    return INT_LITERAL;
}
-?{INT}[l|L]? {
    yylval = astNewStr(AST_INT_LITERAL, yytext);
    return INT_LITERAL;
}

{INT}{EXP}[f|F]? {
    yylval = astNewStr(AST_FLOAT_LITERAL, yytext);
    return FLOAT_LITERAL;
}
{INT}?"."[0-9]+({EXP})?[f|F]? {
    yylval = astNewStr(AST_FLOAT_LITERAL, yytext);
    return FLOAT_LITERAL;
}

\[\s*\] { return OPEN_AND_CLOSE_BRACKET; }
[ \t\n\f\v] { }
. { yyerror("Invalid character"); }

%%
int yywrap() {
    return 1;
}

int column = 0;

void count() {
    int i;
    for (i = 0; yytext[i] != '\0'; i++) {
        if (yytext[i] == '\n')
            column = 0;
        else if (yytext[i] == '\t')
            column += 4 - (column % 4);
        else
            column++;
    }
}

void blockComment() {
    int depth = 1;
    wchar_t c = input();
    while (depth > 0) {
        while (c != '*' && c != '/' && c != '\0')
          c = input();
        if (c == '\0')
            break;
        else if (c == '*') {
            c = input();
            if (c == '/') {
                depth--;
                c = input();
            }
        } else {
            c = input();
            if (c == '*') {
                depth++;
                c = input();
            }
        }
    }
}

void* setFileToParse(FILE* file) {
    YY_BUFFER_STATE buffer = yy_create_buffer(file, YY_BUF_SIZE);
    yy_switch_to_buffer(buffer);
    yylineno = 1;
    return buffer;
}

void freeParseData(void* data) {
    yy_delete_buffer((YY_BUFFER_STATE)data);
}
