%{
#include <stdio.h>
#include "ASTNode.h"
#include "grammar.h"
#include "grammar.hpp"

void count();
%}

%%
"auto" { count(); return AUTO; }
"break" { count(); return BREAK; }
"case" { count(); return CASE; }
"class" { count(); return CLASS; }
"continue" { count(); return CONTINUE; }
"default" { count(); return DEFAULT; }
"do" { count(); return DO; }
"else" { count(); return ELSE; }
"false" { count(); return FALSE; }
"for" { count(); return FOR; }
"if" { count(); return IF; }
"length" { count(); return LENGTH; }
"new" { count(); return NEW; }
"return" { count(); return RETURN; }
"switch" { count(); return SWITCH; }
"true" { count(); return TRUE; }
"void" { count(); return VOID; }
"while" { count(); return WHILE; }

"&=" { count(); return AND_ASSIGN; }
"&&" { count(); return BOOLEAN_AND; }
"||" { count(); return BOOLEAN_OR; }
"--" { count(); return DECREMENT; }
"/=" { count(); return DIV_ASSIGN; }
"==" { count(); return EQUALS; }
">=" { count(); return GREATER_THAN_OR_EQUAL_TO; }
"++" { count(); return INCREMENT; }
"<<" { count(); return LEFT_SHIFT; }
"<<=" { count(); return LEFT_SHIFT_ASSIGN; }
"<=" { count(); return LESS_THAN_OR_EQUAL_TO; }
"-=" { count(); return MINUS_ASSIGN; }
"%=" { count(); return MOD_ASSIGN; }
"*=" { count(); return MULT_ASSIGN; }
"!=" { count(); return NOT_EQUALS; }
"|=" { count(); return OR_ASSIGN; }
"+=" { count(); return PLUS_ASSIGN; }
">>" { count(); return RIGHT_SHIFT; }
">>=" { count(); return RIGHT_SHIFT_ASSIGN; }
">>>" { count(); return UNSIGNED_RIGHT_SHIFT; }
">>>=" { count(); return UNSIGNED_RIGHT_SHIFT_ASSIGN; }
"^=" { count(); return XOR_ASSIGN; }

"(" { count(); return '('; }
")" { count(); return ')'; }
"[" { count(); return '['; }
"]" { count(); return ']'; }
"{" { count(); return '{'; }
"}" { count(); return '}'; }
"<" { count(); return '<'; }
">" { count(); return '>'; }
"=" { count(); return '='; }
"," { count(); return ','; }
";" { count(); return ';'; }
"&" { count(); return '&'; }
"|" { count(); return '|'; }
"^" { count(); return '^'; }
":" { count(); return ':'; }
"+" { count(); return '+'; }
"-" { count(); return '-'; }
"*" { count(); return '*'; }
"/" { count(); return '/'; }
"%" { count(); return '%'; }
"~" { count(); return '~'; }
"!" { count(); return '!'; }
"." { count(); return '.'; }
"?" { count(); return '?'; }

[a-zA-Z_][a-zA-Z_0-9]* {
    count();
    yylval = astNewStr(AST_IDENTIFIER, yytext);
    return IDENTIFIER;
}
(\-?[1-9][0-9]*|0) {
    count();
    yylval = astNewStr(AST_INT_LITERAL, yytext);
    return INT_LITERAL;
}
\[\s*\] { count(); return OPEN_AND_CLOSE_BRACKET; }
[ \t\n\f\v] { count(); }
. { /* Ignore bad characters. */ }

%%
yywrap() {
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

void* setFileToParse(FILE* file) {
    YY_BUFFER_STATE buffer = yy_create_buffer(file, YY_BUF_SIZE);
    yy_switch_to_buffer(buffer);
    return buffer;
}

void freeParseData(void* data) {
    yy_delete_buffer((YY_BUFFER_STATE)data);
}
