/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     BREAK = 258,
     CASE = 259,
     CLASS = 260,
     CONTINUE = 261,
     DEFAULT = 262,
     DO = 263,
     ELSE = 264,
     FALSE = 265,
     FOR = 266,
     IF = 267,
     LENGTH = 268,
     NEW = 269,
     RETURN = 270,
     SWITCH = 271,
     TRUE = 272,
     VOID = 273,
     WHILE = 274,
     BOOLEAN_AND = 275,
     BOOLEAN_OR = 276,
     EQUALS = 277,
     NOT_EQUALS = 278,
     LESS_THAN_OR_EQUAL_TO = 279,
     GREATER_THAN_OR_EQUAL_TO = 280,
     LEFT_SHIFT = 281,
     RIGHT_SHIFT = 282,
     UNSIGNED_RIGHT_SHIFT = 283,
     DECREMENT = 284,
     INCREMENT = 285,
     OPEN_AND_CLOSE_BRACKET = 286,
     AND_ASSIGN = 287,
     DIV_ASSIGN = 288,
     LEFT_SHIFT_ASSIGN = 289,
     MINUS_ASSIGN = 290,
     MOD_ASSIGN = 291,
     MULT_ASSIGN = 292,
     OR_ASSIGN = 293,
     PLUS_ASSIGN = 294,
     RIGHT_SHIFT_ASSIGN = 295,
     UNSIGNED_RIGHT_SHIFT_ASSIGN = 296,
     XOR_ASSIGN = 297,
     IDENTIFIER = 298,
     INT_LITERAL = 299,
     IFX = 300
   };
#endif
/* Tokens.  */
#define BREAK 258
#define CASE 259
#define CLASS 260
#define CONTINUE 261
#define DEFAULT 262
#define DO 263
#define ELSE 264
#define FALSE 265
#define FOR 266
#define IF 267
#define LENGTH 268
#define NEW 269
#define RETURN 270
#define SWITCH 271
#define TRUE 272
#define VOID 273
#define WHILE 274
#define BOOLEAN_AND 275
#define BOOLEAN_OR 276
#define EQUALS 277
#define NOT_EQUALS 278
#define LESS_THAN_OR_EQUAL_TO 279
#define GREATER_THAN_OR_EQUAL_TO 280
#define LEFT_SHIFT 281
#define RIGHT_SHIFT 282
#define UNSIGNED_RIGHT_SHIFT 283
#define DECREMENT 284
#define INCREMENT 285
#define OPEN_AND_CLOSE_BRACKET 286
#define AND_ASSIGN 287
#define DIV_ASSIGN 288
#define LEFT_SHIFT_ASSIGN 289
#define MINUS_ASSIGN 290
#define MOD_ASSIGN 291
#define MULT_ASSIGN 292
#define OR_ASSIGN 293
#define PLUS_ASSIGN 294
#define RIGHT_SHIFT_ASSIGN 295
#define UNSIGNED_RIGHT_SHIFT_ASSIGN 296
#define XOR_ASSIGN 297
#define IDENTIFIER 298
#define INT_LITERAL 299
#define IFX 300




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

