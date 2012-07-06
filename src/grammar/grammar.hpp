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
     FALSE = 264,
     FOR = 265,
     IF = 266,
     LENGTH = 267,
     NEW = 268,
     RETURN = 269,
     SWITCH = 270,
     TRUE = 271,
     VOID = 272,
     WHILE = 273,
     BOOLEAN_AND = 274,
     BOOLEAN_OR = 275,
     EQUALS = 276,
     NOT_EQUALS = 277,
     LESS_THAN_OR_EQUAL_TO = 278,
     GREATER_THAN_OR_EQUAL_TO = 279,
     LEFT_SHIFT = 280,
     RIGHT_SHIFT = 281,
     UNSIGNED_RIGHT_SHIFT = 282,
     DECREMENT = 283,
     INCREMENT = 284,
     OPEN_AND_CLOSE_BRACKET = 285,
     AND_ASSIGN = 286,
     DIV_ASSIGN = 287,
     LEFT_SHIFT_ASSIGN = 288,
     MINUS_ASSIGN = 289,
     MOD_ASSIGN = 290,
     MULT_ASSIGN = 291,
     OR_ASSIGN = 292,
     PLUS_ASSIGN = 293,
     RIGHT_SHIFT_ASSIGN = 294,
     UNSIGNED_RIGHT_SHIFT_ASSIGN = 295,
     XOR_ASSIGN = 296,
     IDENTIFIER = 297,
     INT_LITERAL = 298,
     IFX = 299,
     ELSE = 300
   };
#endif
/* Tokens.  */
#define BREAK 258
#define CASE 259
#define CLASS 260
#define CONTINUE 261
#define DEFAULT 262
#define DO 263
#define FALSE 264
#define FOR 265
#define IF 266
#define LENGTH 267
#define NEW 268
#define RETURN 269
#define SWITCH 270
#define TRUE 271
#define VOID 272
#define WHILE 273
#define BOOLEAN_AND 274
#define BOOLEAN_OR 275
#define EQUALS 276
#define NOT_EQUALS 277
#define LESS_THAN_OR_EQUAL_TO 278
#define GREATER_THAN_OR_EQUAL_TO 279
#define LEFT_SHIFT 280
#define RIGHT_SHIFT 281
#define UNSIGNED_RIGHT_SHIFT 282
#define DECREMENT 283
#define INCREMENT 284
#define OPEN_AND_CLOSE_BRACKET 285
#define AND_ASSIGN 286
#define DIV_ASSIGN 287
#define LEFT_SHIFT_ASSIGN 288
#define MINUS_ASSIGN 289
#define MOD_ASSIGN 290
#define MULT_ASSIGN 291
#define OR_ASSIGN 292
#define PLUS_ASSIGN 293
#define RIGHT_SHIFT_ASSIGN 294
#define UNSIGNED_RIGHT_SHIFT_ASSIGN 295
#define XOR_ASSIGN 296
#define IDENTIFIER 297
#define INT_LITERAL 298
#define IFX 299
#define ELSE 300




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

