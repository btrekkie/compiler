/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.3"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Using locations.  */
#define YYLSP_NEEDED 0



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




/* Copy the first part of user declarations.  */
#line 1 "grammar.y"

extern "C" {
    int yyparse(void);
    int yylex(void);
    
    #include "ast.h"
}

#include <stdio.h>
#include "compiler.hpp"

#define YYSTYPE AST*

extern char yytext[];
extern int column;

void yyerror(const char* s) {
    fflush(stdout);
    printf("\n%*s\n%*s\n", column, "^", column, s);
}

int main() {
    yyparse();
    return 0;
}



/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif

#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 224 "grammar.cpp"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int i)
#else
static int
YYID (i)
    int i;
#endif
{
  return i;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  5
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   546

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  69
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  51
/* YYNRULES -- Number of rules.  */
#define YYNRULES  139
/* YYNRULES -- Number of states.  */
#define YYNSTATES  262

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   300

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    67,     2,     2,     2,    65,    58,     2,
      51,    52,    63,    61,    49,    62,    68,    64,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    53,    48,
      59,    50,    60,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    54,     2,    55,    57,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    46,    56,    47,    66,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     5,    11,    14,    15,    17,    19,    21,
      25,    29,    31,    33,    37,    39,    42,    49,    56,    62,
      68,    72,    74,    77,    82,    86,    89,    90,    92,    94,
      97,   100,   103,   105,   107,   109,   111,   113,   115,   117,
     121,   123,   125,   127,   129,   131,   133,   135,   137,   139,
     141,   143,   145,   148,   151,   154,   157,   163,   171,   179,
     184,   185,   188,   190,   192,   194,   196,   202,   210,   220,
     222,   223,   227,   229,   231,   233,   235,   240,   249,   252,
     256,   259,   263,   266,   270,   275,   279,   283,   285,   290,
     294,   296,   300,   302,   304,   308,   310,   314,   316,   320,
     324,   326,   330,   332,   336,   338,   342,   344,   348,   352,
     356,   360,   362,   366,   370,   374,   376,   380,   384,   386,
     390,   394,   398,   400,   405,   407,   409,   412,   416,   418,
     420,   423,   426,   430,   432,   436,   441,   445,   447,   449
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      70,     0,    -1,    71,    -1,     5,    43,    46,    72,    47,
      -1,    72,    73,    -1,    -1,    74,    -1,    79,    -1,    75,
      -1,    78,    76,    48,    -1,    76,    49,    77,    -1,    77,
      -1,    43,    -1,    43,    50,   103,    -1,    43,    -1,    78,
      31,    -1,    78,    43,    51,    80,    52,    82,    -1,    18,
      43,    51,    80,    52,    82,    -1,    78,    43,    51,    52,
      82,    -1,    18,    43,    51,    52,    82,    -1,    80,    49,
      81,    -1,    81,    -1,    78,    43,    -1,    78,    43,    50,
     103,    -1,    46,    83,    47,    -1,    83,    84,    -1,    -1,
      48,    -1,    75,    -1,   104,    48,    -1,    86,    48,    -1,
     100,    48,    -1,    87,    -1,    88,    -1,    92,    -1,    93,
      -1,    94,    -1,    98,    -1,    99,    -1,    46,    83,    47,
      -1,    50,    -1,    39,    -1,    35,    -1,    37,    -1,    33,
      -1,    36,    -1,    34,    -1,    40,    -1,    41,    -1,    32,
      -1,    38,    -1,    42,    -1,   119,    30,    -1,   119,    29,
      -1,    30,   119,    -1,    29,   119,    -1,    12,    51,   103,
      52,    84,    -1,    12,    51,   103,    52,    84,     9,    84,
      -1,    16,    51,   103,    52,    46,    89,    47,    -1,    89,
      90,    53,    83,    -1,    -1,     4,    91,    -1,     7,    -1,
      44,    -1,    17,    -1,    10,    -1,    19,    51,   103,    52,
      84,    -1,     8,    84,    19,    51,   103,    52,    48,    -1,
      11,    51,    95,    48,   103,    48,    95,    52,    84,    -1,
      96,    -1,    -1,    97,    49,    96,    -1,    97,    -1,   104,
      -1,    86,    -1,   100,    -1,    78,    43,    50,   103,    -1,
      11,    51,    78,    43,    53,   103,    52,    84,    -1,     3,
      48,    -1,     3,    44,    48,    -1,     6,    48,    -1,     6,
      44,    48,    -1,    15,    48,    -1,    15,   103,    48,    -1,
      43,    51,   101,    52,    -1,    43,    51,    52,    -1,   101,
      49,   103,    -1,   103,    -1,   102,    54,   103,    55,    -1,
      54,   103,    55,    -1,   105,    -1,   119,    85,   105,    -1,
     104,    -1,   106,    -1,   106,    21,   107,    -1,   107,    -1,
     107,    20,   109,    -1,   108,    -1,   108,    22,   109,    -1,
     108,    23,   109,    -1,   109,    -1,   109,    56,   110,    -1,
     110,    -1,   110,    57,   111,    -1,   111,    -1,   111,    58,
     112,    -1,   112,    -1,   112,    59,   113,    -1,   112,    60,
     113,    -1,   112,    24,   113,    -1,   112,    25,   113,    -1,
     113,    -1,   113,    26,   114,    -1,   113,    27,   114,    -1,
     113,    28,   114,    -1,   114,    -1,   114,    61,   115,    -1,
     114,    62,   115,    -1,   115,    -1,   115,    63,   116,    -1,
     115,    64,   116,    -1,   115,    65,   116,    -1,   116,    -1,
      51,   103,    52,   118,    -1,   117,    -1,    86,    -1,    62,
     117,    -1,    54,   101,    55,    -1,    31,    -1,   118,    -1,
      66,   117,    -1,    67,   117,    -1,    14,    43,   102,    -1,
     119,    -1,    51,   103,    52,    -1,   119,    54,   103,    55,
      -1,   119,    68,    13,    -1,   100,    -1,    91,    -1,    43,
      -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,    44,    44,    46,    48,    51,    52,    54,    56,    58,
      60,    62,    64,    66,    72,    74,    76,    79,    86,    88,
      94,    96,    98,   100,   102,   104,   107,   108,   110,   112,
     114,   116,   118,   120,   122,   124,   126,   128,   130,   132,
     134,   136,   138,   140,   142,   144,   146,   148,   150,   152,
     154,   156,   158,   160,   162,   164,   166,   168,   170,   172,
     175,   176,   178,   180,   182,   184,   186,   188,   190,   193,
     196,   197,   199,   204,   206,   208,   210,   219,   221,   223,
     225,   227,   229,   231,   233,   235,   237,   239,   241,   243,
     248,   250,   254,   256,   258,   260,   262,   264,   266,   268,
     270,   272,   274,   276,   278,   280,   282,   284,   286,   288,
     291,   294,   296,   298,   300,   303,   305,   307,   309,   311,
     313,   315,   317,   319,   321,   323,   325,   327,   329,   331,
     333,   335,   337,   339,   341,   343,   345,   347,   349,   351
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "BREAK", "CASE", "CLASS", "CONTINUE",
  "DEFAULT", "DO", "ELSE", "FALSE", "FOR", "IF", "LENGTH", "NEW", "RETURN",
  "SWITCH", "TRUE", "VOID", "WHILE", "BOOLEAN_AND", "BOOLEAN_OR", "EQUALS",
  "NOT_EQUALS", "LESS_THAN_OR_EQUAL_TO", "GREATER_THAN_OR_EQUAL_TO",
  "LEFT_SHIFT", "RIGHT_SHIFT", "UNSIGNED_RIGHT_SHIFT", "DECREMENT",
  "INCREMENT", "OPEN_AND_CLOSE_BRACKET", "AND_ASSIGN", "DIV_ASSIGN",
  "LEFT_SHIFT_ASSIGN", "MINUS_ASSIGN", "MOD_ASSIGN", "MULT_ASSIGN",
  "OR_ASSIGN", "PLUS_ASSIGN", "RIGHT_SHIFT_ASSIGN",
  "UNSIGNED_RIGHT_SHIFT_ASSIGN", "XOR_ASSIGN", "IDENTIFIER", "INT_LITERAL",
  "IFX", "'{'", "'}'", "';'", "','", "'='", "'('", "')'", "':'", "'['",
  "']'", "'|'", "'^'", "'&'", "'<'", "'>'", "'+'", "'-'", "'*'", "'/'",
  "'%'", "'~'", "'!'", "'.'", "$accept", "file", "classDefinition",
  "classBodyItemList", "classBodyItem", "fieldDeclaration",
  "varDeclaration", "varDeclarationList", "varDeclarationItem", "type",
  "methodDefinition", "argList", "argItem", "methodBody", "statementList",
  "statement", "assignmentOperator", "directIncrementExpression",
  "ifStatement", "switchStatement", "caseList", "caseClause", "literal",
  "whileStatement", "doWhileStatement", "forStatement", "forStatementList",
  "forNonEmptyStatementList", "forComponentStatement", "forInStatement",
  "controlFlowStatement", "methodCall", "expressionList",
  "bracketExpressionList", "expression", "directAssignmentExpression",
  "assignmentExpression", "booleanOrExpression", "booleanAndExpression",
  "equalityExpression", "bitwiseOrExpression", "xorExpression",
  "bitwiseAndExpression", "comparisonExpression", "bitShiftExpression",
  "additionExpression", "multiplicationExpression", "castExpression",
  "castUnfriendlyUnaryExpression", "castFriendlyUnaryExpression",
  "parenthesesExpression", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   123,   125,    59,    44,
      61,    40,    41,    58,    91,    93,   124,    94,    38,    60,
      62,    43,    45,    42,    47,    37,   126,    33,    46
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    69,    70,    71,    72,    72,    73,    73,    74,    75,
      76,    76,    77,    77,    78,    78,    79,    79,    79,    79,
      80,    80,    81,    81,    82,    83,    83,    84,    84,    84,
      84,    84,    84,    84,    84,    84,    84,    84,    84,    84,
      85,    85,    85,    85,    85,    85,    85,    85,    85,    85,
      85,    85,    86,    86,    86,    86,    87,    87,    88,    89,
      89,    90,    90,    91,    91,    91,    92,    93,    94,    95,
      95,    96,    96,    97,    97,    97,    97,    98,    99,    99,
      99,    99,    99,    99,   100,   100,   101,   101,   102,   102,
     103,   104,   105,   105,   106,   106,   107,   107,   108,   108,
     108,   109,   109,   110,   110,   111,   111,   112,   112,   112,
     112,   112,   113,   113,   113,   113,   114,   114,   114,   115,
     115,   115,   115,   116,   116,   117,   117,   117,   117,   117,
     118,   118,   118,   118,   119,   119,   119,   119,   119,   119
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     5,     2,     0,     1,     1,     1,     3,
       3,     1,     1,     3,     1,     2,     6,     6,     5,     5,
       3,     1,     2,     4,     3,     2,     0,     1,     1,     2,
       2,     2,     1,     1,     1,     1,     1,     1,     1,     3,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     2,     2,     2,     2,     5,     7,     7,     4,
       0,     2,     1,     1,     1,     1,     5,     7,     9,     1,
       0,     3,     1,     1,     1,     1,     4,     8,     2,     3,
       2,     3,     2,     3,     4,     3,     3,     1,     4,     3,
       1,     3,     1,     1,     3,     1,     3,     1,     3,     3,
       1,     3,     1,     3,     1,     3,     1,     3,     3,     3,
       3,     1,     3,     3,     3,     1,     3,     3,     1,     3,
       3,     3,     1,     4,     1,     1,     2,     3,     1,     1,
       2,     2,     3,     1,     3,     4,     3,     1,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     0,     0,     2,     0,     1,     5,     0,     0,    14,
       3,     4,     6,     8,     0,     7,     0,    15,    12,     0,
      11,     0,     0,     0,     9,     0,     0,     0,     0,    21,
      65,     0,    64,     0,     0,   128,   139,    63,     0,     0,
       0,     0,     0,   125,   138,   137,    13,    92,    90,    93,
      95,    97,   100,   102,   104,   106,   111,   115,   118,   122,
     124,   129,   133,     0,     0,    12,    10,    26,    19,    22,
       0,     0,     0,     0,    55,    54,     0,     0,     0,    87,
     126,   133,   130,   131,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    53,    52,    49,    44,    46,    42,    45,
      43,    50,    41,    47,    48,    51,    40,     0,     0,     0,
      18,     0,     0,     0,    20,    17,     0,   132,     0,    85,
       0,   134,     0,   127,    94,    96,    98,    99,   101,   103,
     105,   109,   110,   107,   108,   112,   113,   114,   116,   117,
     119,   120,   121,     0,   136,    91,    16,     0,     0,     0,
       0,     0,     0,     0,     0,   139,    26,    24,    27,    28,
       0,    25,     0,    32,    33,    34,    35,    36,    37,    38,
     137,     0,     0,    23,     0,     0,   134,    84,   123,   133,
      86,   135,     0,    78,     0,    80,     0,    70,     0,    82,
       0,     0,     0,     0,    30,    31,    29,    89,     0,    79,
      81,     0,     0,    74,     0,    69,    72,   137,    73,     0,
      83,     0,     0,    39,    88,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    71,    56,    60,
      66,     0,    76,     0,    70,     0,     0,     0,    67,     0,
       0,    57,     0,    62,    58,     0,    77,     0,    61,    26,
      68,    59
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     2,     3,     7,    11,    12,   169,    19,    20,   170,
      15,    28,    29,    68,   122,   171,   119,    43,   173,   174,
     247,   255,    44,   175,   176,   177,   214,   215,   216,   178,
     179,    45,    78,   127,    79,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      81
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -166
static const yytype_int16 yypact[] =
{
      -2,    16,    99,  -166,    71,  -166,  -166,    65,    63,  -166,
    -166,  -166,  -166,  -166,    18,  -166,    68,  -166,    -6,    86,
    -166,   -20,   285,   -18,  -166,    82,    97,    19,   -44,  -166,
    -166,   103,  -166,    -4,    -4,  -166,    98,  -166,   285,   285,
     313,   313,   313,  -166,  -166,  -166,  -166,  -166,  -166,   153,
     158,   136,   137,   135,   138,    11,   125,   100,    91,  -166,
    -166,  -166,   478,    97,    62,   145,  -166,  -166,  -166,   147,
     156,    97,   154,   285,   -16,   -16,    12,   149,    27,  -166,
    -166,    39,  -166,  -166,   285,   285,   285,   285,   285,   285,
     285,   285,   285,   285,   285,   285,   285,   285,   285,   285,
     285,   285,   285,  -166,  -166,  -166,  -166,  -166,  -166,  -166,
    -166,  -166,  -166,  -166,  -166,  -166,  -166,   285,   189,   285,
    -166,    97,   366,   285,  -166,  -166,   285,   160,   163,  -166,
      90,    14,   285,  -166,   158,   137,   137,   137,   135,   138,
      11,   125,   125,   125,   125,   100,   100,   100,    91,    91,
    -166,  -166,  -166,   155,  -166,  -166,  -166,    48,    78,   458,
     167,   168,   246,   170,   172,   -24,  -166,  -166,  -166,  -166,
      46,  -166,   180,  -166,  -166,  -166,  -166,  -166,  -166,  -166,
     183,   185,   478,  -166,   179,   285,  -166,  -166,  -166,   -16,
    -166,  -166,   188,  -166,   190,  -166,   218,   140,   285,  -166,
     195,   285,   285,   412,  -166,  -166,  -166,  -166,   191,  -166,
    -166,   193,    55,  -166,   205,  -166,   206,    72,  -166,   207,
    -166,   209,   212,  -166,  -166,   285,    22,   285,   140,   458,
     211,   458,   213,   285,   285,   220,    60,  -166,   257,  -166,
    -166,   222,  -166,   219,   140,   223,   458,    26,  -166,   458,
     226,  -166,     4,  -166,  -166,   221,  -166,   458,  -166,  -166,
    -166,   458
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -166,  -166,  -166,  -166,  -166,  -166,   272,  -166,   256,    -3,
    -166,   259,   215,   -61,  -165,  -144,  -166,  -113,  -166,  -166,
    -166,  -166,    31,  -166,  -166,  -166,    42,    59,  -166,  -166,
    -166,     1,   216,  -166,    15,    23,   169,  -166,   214,  -166,
      79,   203,   204,   217,    36,    76,    69,    85,   148,   165,
     -22
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -76
static const yytype_int16 yytable[] =
{
      62,   203,   120,     1,    14,    70,    30,   -14,    71,   172,
     125,    74,    75,    32,    30,   196,    62,    62,    27,   -14,
      27,    32,    30,     9,    30,     9,    31,    76,    31,    32,
     252,    32,    26,   253,    63,    91,    92,    46,   117,    36,
      37,    33,    34,    35,    22,    23,   172,    73,    37,    17,
      17,    62,   118,    77,    62,    36,    37,    36,    37,     4,
     156,    18,    69,    38,   129,    73,    39,    27,   103,   104,
      93,    94,   233,   254,    40,   234,   132,    17,    41,    42,
      41,    42,   133,     8,   213,   238,    17,   240,   128,    65,
     172,    17,   192,   117,   261,    62,   193,    62,   226,     5,
     182,    62,   251,   245,    62,   256,    16,   118,     9,   189,
      62,    70,    10,   260,   121,   213,   172,     6,   172,    21,
     -75,   -75,   194,   180,   -75,    65,   195,   141,   142,   143,
     144,   213,   153,   172,    24,    25,   172,   182,   183,   132,
      62,   184,   187,    67,   172,   181,    72,   190,   172,    76,
      30,    95,    96,    97,   100,   101,   102,    32,    86,    87,
     180,    98,    99,    62,   135,   136,   137,   148,   149,    33,
      34,   145,   146,   147,    84,   182,    62,   200,    85,    62,
      62,   182,   181,   165,    37,   150,   151,   152,    80,    82,
      83,    73,    89,    88,   212,    22,    90,   123,   217,     9,
     208,   131,   154,    62,   180,    62,   182,   182,   126,   182,
     191,    62,    62,   219,   185,   186,   221,   222,   197,   198,
     218,   201,   182,   202,   182,   236,   181,   182,   204,   217,
     180,   205,   180,   206,   207,   182,   209,   211,   210,   182,
     232,   236,   235,   220,   225,   217,   224,   180,   242,   243,
     180,   218,   181,   227,   181,   228,    30,   239,   180,   229,
      31,   230,   180,    32,   231,   241,   -76,   218,   244,   181,
     248,   249,   181,   233,   259,    33,    34,    35,   257,    13,
     181,    66,    64,   258,   181,   124,   250,   237,   155,    36,
      37,   138,   130,   139,   199,    30,   188,    38,   134,    31,
      39,     0,    32,     0,     0,     0,     0,   140,    40,     0,
       0,     0,    41,    42,    33,    34,    35,     0,     0,     0,
       0,     0,     0,    30,     0,     0,     0,    31,    36,    37,
      32,     0,     0,     0,     0,     0,    38,     0,     0,    39,
       0,     0,    33,    34,    35,     0,     0,    40,     0,     0,
       0,    41,    42,     0,     0,     0,    36,    37,     0,     0,
       0,     0,     0,     0,    73,     0,     0,    39,     0,   157,
       0,     0,   158,     0,   159,    40,    30,   160,   161,    41,
      42,   162,   163,    32,     0,   164,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    33,    34,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   165,
      37,     0,   166,   167,   168,   157,     0,    73,   158,     0,
     159,     0,    30,   160,   161,     0,     0,   162,   163,    32,
       0,   164,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    33,    34,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   165,    37,     0,   166,   223,
     168,   157,     0,    73,   158,     0,   159,     0,    30,   160,
     161,     0,     0,   162,   163,    32,     0,   164,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    33,    34,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   165,    37,     0,   166,     0,   168,   103,   104,    73,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,     0,     0,     0,     0,     0,     0,     0,   116,     0,
       0,     0,   117,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   118
};

static const yytype_int16 yycheck[] =
{
      22,   166,    63,     5,     7,    49,    10,    31,    52,   122,
      71,    33,    34,    17,    10,   159,    38,    39,    21,    43,
      23,    17,    10,    43,    10,    43,    14,    51,    14,    17,
       4,    17,    52,     7,    52,    24,    25,    22,    54,    43,
      44,    29,    30,    31,    50,    51,   159,    51,    44,    31,
      31,    73,    68,    38,    76,    43,    44,    43,    44,    43,
     121,    43,    43,    51,    52,    51,    54,    70,    29,    30,
      59,    60,    50,    47,    62,    53,    49,    31,    66,    67,
      66,    67,    55,    18,   197,   229,    31,   231,    73,    43,
     203,    31,    44,    54,   259,   117,    48,   119,    43,     0,
     122,   123,   246,    43,   126,   249,    43,    68,    43,   131,
     132,    49,    47,   257,    52,   228,   229,    46,   231,    51,
      48,    49,    44,   122,    52,    43,    48,    91,    92,    93,
      94,   244,   117,   246,    48,    49,   249,   159,   123,    49,
     162,   126,    52,    46,   257,   122,    43,   132,   261,    51,
      10,    26,    27,    28,    63,    64,    65,    17,    22,    23,
     159,    61,    62,   185,    85,    86,    87,    98,    99,    29,
      30,    95,    96,    97,    21,   197,   198,   162,    20,   201,
     202,   203,   159,    43,    44,   100,   101,   102,    40,    41,
      42,    51,    57,    56,   197,    50,    58,    50,   197,    43,
     185,    52,    13,   225,   203,   227,   228,   229,    54,   231,
      55,   233,   234,   198,    54,    52,   201,   202,    51,    51,
     197,    51,   244,    51,   246,   228,   203,   249,    48,   228,
     229,    48,   231,    48,    55,   257,    48,    19,    48,   261,
     225,   244,   227,    48,    51,   244,    55,   246,   233,   234,
     249,   228,   229,    48,   231,    49,    10,    46,   257,    52,
      14,    52,   261,    17,    52,    52,     9,   244,    48,   246,
      48,    52,   249,    50,    53,    29,    30,    31,    52,     7,
     257,    25,    23,   252,   261,    70,   244,   228,   119,    43,
      44,    88,    76,    89,    48,    10,   131,    51,    84,    14,
      54,    -1,    17,    -1,    -1,    -1,    -1,    90,    62,    -1,
      -1,    -1,    66,    67,    29,    30,    31,    -1,    -1,    -1,
      -1,    -1,    -1,    10,    -1,    -1,    -1,    14,    43,    44,
      17,    -1,    -1,    -1,    -1,    -1,    51,    -1,    -1,    54,
      -1,    -1,    29,    30,    31,    -1,    -1,    62,    -1,    -1,
      -1,    66,    67,    -1,    -1,    -1,    43,    44,    -1,    -1,
      -1,    -1,    -1,    -1,    51,    -1,    -1,    54,    -1,     3,
      -1,    -1,     6,    -1,     8,    62,    10,    11,    12,    66,
      67,    15,    16,    17,    -1,    19,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    29,    30,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    43,
      44,    -1,    46,    47,    48,     3,    -1,    51,     6,    -1,
       8,    -1,    10,    11,    12,    -1,    -1,    15,    16,    17,
      -1,    19,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    29,    30,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    43,    44,    -1,    46,    47,
      48,     3,    -1,    51,     6,    -1,     8,    -1,    10,    11,
      12,    -1,    -1,    15,    16,    17,    -1,    19,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    29,    30,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    43,    44,    -1,    46,    -1,    48,    29,    30,    51,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    50,    -1,
      -1,    -1,    54,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    68
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     5,    70,    71,    43,     0,    46,    72,    18,    43,
      47,    73,    74,    75,    78,    79,    43,    31,    43,    76,
      77,    51,    50,    51,    48,    49,    52,    78,    80,    81,
      10,    14,    17,    29,    30,    31,    43,    44,    51,    54,
      62,    66,    67,    86,    91,   100,   103,   104,   105,   106,
     107,   108,   109,   110,   111,   112,   113,   114,   115,   116,
     117,   118,   119,    52,    80,    43,    77,    46,    82,    43,
      49,    52,    43,    51,   119,   119,    51,   103,   101,   103,
     117,   119,   117,   117,    21,    20,    22,    23,    56,    57,
      58,    24,    25,    59,    60,    26,    27,    28,    61,    62,
      63,    64,    65,    29,    30,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    50,    54,    68,    85,
      82,    52,    83,    50,    81,    82,    54,   102,   103,    52,
     101,    52,    49,    55,   107,   109,   109,   109,   110,   111,
     112,   113,   113,   113,   113,   114,   114,   114,   115,   115,
     116,   116,   116,   103,    13,   105,    82,     3,     6,     8,
      11,    12,    15,    16,    19,    43,    46,    47,    48,    75,
      78,    84,    86,    87,    88,    92,    93,    94,    98,    99,
     100,   104,   119,   103,   103,    54,    52,    52,   118,   119,
     103,    55,    44,    48,    44,    48,    84,    51,    51,    48,
     103,    51,    51,    83,    48,    48,    48,    55,   103,    48,
      48,    19,    78,    86,    95,    96,    97,   100,   104,   103,
      48,   103,   103,    47,    55,    51,    43,    48,    49,    52,
      52,    52,   103,    50,    53,   103,    78,    96,    84,    46,
      84,    52,   103,   103,    48,    43,     9,    89,    48,    52,
      95,    84,     4,     7,    47,    90,    84,    52,    91,    53,
      84,    83
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *bottom, yytype_int16 *top)
#else
static void
yy_stack_print (bottom, top)
    yytype_int16 *bottom;
    yytype_int16 *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      fprintf (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      fprintf (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */



/* The look-ahead symbol.  */
int yychar;

/* The semantic value of the look-ahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
  
  int yystate;
  int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Look-ahead token as an internal (translated) token number.  */
  int yytoken = 0;
#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  yytype_int16 yyssa[YYINITDEPTH];
  yytype_int16 *yyss = yyssa;
  yytype_int16 *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  YYSTYPE *yyvsp;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;


      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     look-ahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to look-ahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a look-ahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid look-ahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the look-ahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
#line 45 "grammar.y"
    { (yyval) = astNew1(AST_FILE, (yyvsp[(1) - (1)])); processFile((yyval)); astFree((yyval)); }
    break;

  case 3:
#line 47 "grammar.y"
    { (yyval) = astNew2(AST_CLASS_DEFINITION, (yyvsp[(2) - (5)]), (yyvsp[(4) - (5)])); }
    break;

  case 4:
#line 49 "grammar.y"
    { (yyval) = astNew2(AST_CLASS_BODY_ITEM_LIST, (yyvsp[(1) - (2)]), (yyvsp[(2) - (2)])); }
    break;

  case 5:
#line 51 "grammar.y"
    { (yyval) = astNew0(AST_EMPTY_CLASS_BODY_ITEM_LIST); }
    break;

  case 6:
#line 53 "grammar.y"
    { (yyval) = (yyvsp[(1) - (1)]); }
    break;

  case 7:
#line 55 "grammar.y"
    { (yyval) = (yyvsp[(1) - (1)]); }
    break;

  case 8:
#line 57 "grammar.y"
    { (yyval) = (yyvsp[(1) - (1)]); }
    break;

  case 9:
#line 59 "grammar.y"
    { (yyval) = astNew2(AST_VAR_DECLARATION, (yyvsp[(1) - (3)]), (yyvsp[(2) - (3)])); }
    break;

  case 10:
#line 61 "grammar.y"
    { (yyval) = astNew2(AST_VAR_DECLARATION_LIST, (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)])); }
    break;

  case 11:
#line 63 "grammar.y"
    { (yyval) = (yyvsp[(1) - (1)]); }
    break;

  case 12:
#line 65 "grammar.y"
    { (yyval) = (yyvsp[(1) - (1)]); }
    break;

  case 13:
#line 67 "grammar.y"
    { (yyval) = astNew3(
                           AST_ASSIGNMENT_EXPRESSION,
                           (yyvsp[(1) - (3)]),
                           astNew0(AST_ASSIGN),
                           (yyvsp[(3) - (3)])); }
    break;

  case 14:
#line 73 "grammar.y"
    { (yyval) = astNew1(AST_TYPE, (yyvsp[(1) - (1)])); }
    break;

  case 15:
#line 75 "grammar.y"
    { (yyval) = astNew1(AST_TYPE_ARRAY, (yyvsp[(1) - (2)])); }
    break;

  case 16:
#line 77 "grammar.y"
    { (yyval) = astNew4(
                         AST_METHOD_DEFINITION, (yyvsp[(1) - (6)]), (yyvsp[(2) - (6)]), (yyvsp[(4) - (6)]), (yyvsp[(6) - (6)])); }
    break;

  case 17:
#line 80 "grammar.y"
    { (yyval) = astNew4(
                         AST_METHOD_DEFINITION,
                         astNew0(AST_VOID),
                         (yyvsp[(2) - (6)]),
                         (yyvsp[(4) - (6)]),
                         (yyvsp[(6) - (6)])); }
    break;

  case 18:
#line 87 "grammar.y"
    { (yyval) = astNew3(AST_METHOD_DEFINITION, (yyvsp[(1) - (5)]), (yyvsp[(2) - (5)]), (yyvsp[(5) - (5)])); }
    break;

  case 19:
#line 89 "grammar.y"
    { (yyval) = astNew3(
                         AST_METHOD_DEFINITION,
                         astNew0(AST_VOID),
                         (yyvsp[(2) - (5)]),
                         (yyvsp[(5) - (5)])); }
    break;

  case 20:
#line 95 "grammar.y"
    { (yyval) = astNew2(AST_ARG_LIST, (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)])); }
    break;

  case 21:
#line 97 "grammar.y"
    { (yyval) = (yyvsp[(1) - (1)]); }
    break;

  case 22:
#line 99 "grammar.y"
    { (yyval) = astNew2(AST_ARG, (yyvsp[(1) - (2)]), (yyvsp[(2) - (2)])); }
    break;

  case 23:
#line 101 "grammar.y"
    { (yyval) = astNew3(AST_ARG, (yyvsp[(1) - (4)]), (yyvsp[(2) - (4)]), (yyvsp[(4) - (4)])); }
    break;

  case 24:
#line 103 "grammar.y"
    { (yyval) = (yyvsp[(2) - (3)]); }
    break;

  case 25:
#line 105 "grammar.y"
    { (yyval) = astNew2(AST_STATEMENT_LIST, (yyvsp[(1) - (2)]), (yyvsp[(2) - (2)])); }
    break;

  case 26:
#line 107 "grammar.y"
    { (yyval) = astNew0(AST_EMPTY_STATEMENT_LIST); }
    break;

  case 27:
#line 109 "grammar.y"
    { (yyval) = astNew0(AST_EMPTY_STATEMENT); }
    break;

  case 28:
#line 111 "grammar.y"
    { (yyval) = (yyvsp[(1) - (1)]); }
    break;

  case 29:
#line 113 "grammar.y"
    { (yyval) = (yyvsp[(1) - (2)]); }
    break;

  case 30:
#line 115 "grammar.y"
    { (yyval) = (yyvsp[(1) - (2)]); }
    break;

  case 31:
#line 117 "grammar.y"
    { (yyval) = (yyvsp[(1) - (2)]); }
    break;

  case 32:
#line 119 "grammar.y"
    { (yyval) = (yyvsp[(1) - (1)]); }
    break;

  case 33:
#line 121 "grammar.y"
    { (yyval) = (yyvsp[(1) - (1)]); }
    break;

  case 34:
#line 123 "grammar.y"
    { (yyval) = (yyvsp[(1) - (1)]); }
    break;

  case 35:
#line 125 "grammar.y"
    { (yyval) = (yyvsp[(1) - (1)]); }
    break;

  case 36:
#line 127 "grammar.y"
    { (yyval) = (yyvsp[(1) - (1)]); }
    break;

  case 37:
#line 129 "grammar.y"
    { (yyval) = (yyvsp[(1) - (1)]); }
    break;

  case 38:
#line 131 "grammar.y"
    { (yyval) = (yyvsp[(1) - (1)]); }
    break;

  case 39:
#line 133 "grammar.y"
    { (yyval) = astNew1(AST_BLOCK, (yyvsp[(2) - (3)])); }
    break;

  case 40:
#line 135 "grammar.y"
    { (yyval) = astNew0(AST_ASSIGN); }
    break;

  case 41:
#line 137 "grammar.y"
    { (yyval) = astNew0(AST_PLUS_ASSIGN); }
    break;

  case 42:
#line 139 "grammar.y"
    { (yyval) = astNew0(AST_MINUS_ASSIGN); }
    break;

  case 43:
#line 141 "grammar.y"
    { (yyval) = astNew0(AST_MULT_ASSIGN); }
    break;

  case 44:
#line 143 "grammar.y"
    { (yyval) = astNew0(AST_DIV_ASSIGN); }
    break;

  case 45:
#line 145 "grammar.y"
    { (yyval) = astNew0(AST_MOD_ASSIGN); }
    break;

  case 46:
#line 147 "grammar.y"
    { (yyval) = astNew0(AST_LEFT_SHIFT_ASSIGN); }
    break;

  case 47:
#line 149 "grammar.y"
    { (yyval) = astNew0(AST_RIGHT_SHIFT_ASSIGN); }
    break;

  case 48:
#line 151 "grammar.y"
    { (yyval) = astNew0(AST_UNSIGNED_RIGHT_SHIFT_ASSIGN); }
    break;

  case 49:
#line 153 "grammar.y"
    { (yyval) = astNew0(AST_AND_ASSIGN); }
    break;

  case 50:
#line 155 "grammar.y"
    { (yyval) = astNew0(AST_OR_ASSIGN); }
    break;

  case 51:
#line 157 "grammar.y"
    { (yyval) = astNew0(AST_XOR_ASSIGN); }
    break;

  case 52:
#line 159 "grammar.y"
    { (yyval) = astNew1(AST_POST_INCREMENT, (yyvsp[(1) - (2)])); }
    break;

  case 53:
#line 161 "grammar.y"
    { (yyval) = astNew1(AST_POST_DECREMENT, (yyvsp[(1) - (2)])); }
    break;

  case 54:
#line 163 "grammar.y"
    { (yyval) = astNew1(AST_PRE_INCREMENT, (yyvsp[(1) - (2)])); }
    break;

  case 55:
#line 165 "grammar.y"
    { (yyval) = astNew1(AST_PRE_DECREMENT, (yyvsp[(1) - (2)])); }
    break;

  case 56:
#line 167 "grammar.y"
    { (yyval) = astNew2(AST_IF, (yyvsp[(3) - (5)]), (yyvsp[(5) - (5)])); }
    break;

  case 57:
#line 169 "grammar.y"
    { (yyval) = astNew3(AST_IF_ELSE, (yyvsp[(3) - (7)]), (yyvsp[(5) - (7)]), (yyvsp[(7) - (7)])); }
    break;

  case 58:
#line 171 "grammar.y"
    { (yyval) = astNew2(AST_SWITCH, (yyvsp[(3) - (7)]), (yyvsp[(6) - (7)])); }
    break;

  case 59:
#line 173 "grammar.y"
    { (yyval) = astNew3(AST_CASE_LIST, (yyvsp[(1) - (4)]), (yyvsp[(2) - (4)]), (yyvsp[(4) - (4)])); }
    break;

  case 60:
#line 175 "grammar.y"
    { (yyval) = astNew0(AST_EMPTY_CASE_LIST); }
    break;

  case 61:
#line 177 "grammar.y"
    { (yyval) = astNew1(AST_CASE_LABEL, (yyvsp[(2) - (2)])); }
    break;

  case 62:
#line 179 "grammar.y"
    { (yyval) = astNew0(AST_CASE_LABEL_DEFAULT); }
    break;

  case 63:
#line 181 "grammar.y"
    { (yyval) = (yyvsp[(1) - (1)]); }
    break;

  case 64:
#line 183 "grammar.y"
    { (yyval) = astNew0(AST_TRUE); }
    break;

  case 65:
#line 185 "grammar.y"
    { (yyval) = astNew0(AST_FALSE); }
    break;

  case 66:
#line 187 "grammar.y"
    { (yyval) = astNew2(AST_WHILE, (yyvsp[(3) - (5)]), (yyvsp[(5) - (5)])); }
    break;

  case 67:
#line 189 "grammar.y"
    {(yyval) = astNew2(AST_DO_WHILE, (yyvsp[(2) - (7)]), (yyvsp[(5) - (7)])); }
    break;

  case 68:
#line 192 "grammar.y"
    { (yyval) = astNew4(AST_FOR, (yyvsp[(3) - (9)]), (yyvsp[(5) - (9)]), (yyvsp[(7) - (9)]), (yyvsp[(9) - (9)])); }
    break;

  case 69:
#line 194 "grammar.y"
    { (yyval) = (yyvsp[(1) - (1)]); }
    break;

  case 70:
#line 196 "grammar.y"
    { (yyval) = astNew0(AST_EMPTY_STATEMENT_LIST); }
    break;

  case 71:
#line 198 "grammar.y"
    { (yyval) = astNew2(AST_STATEMENT_LIST, (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)])); }
    break;

  case 72:
#line 200 "grammar.y"
    { (yyval) = astNew2(
                                 AST_STATEMENT_LIST,
                                 astNew0(AST_EMPTY_STATEMENT_LIST),
                                 (yyvsp[(1) - (1)])); }
    break;

  case 73:
#line 205 "grammar.y"
    { (yyval) = (yyvsp[(1) - (1)]); }
    break;

  case 74:
#line 207 "grammar.y"
    { (yyval) = (yyvsp[(1) - (1)]); }
    break;

  case 75:
#line 209 "grammar.y"
    { (yyval) = (yyvsp[(1) - (1)]); }
    break;

  case 76:
#line 211 "grammar.y"
    { (yyval) = astNew2(
                              AST_VAR_DECLARATION,
                              (yyvsp[(1) - (4)]),
                              astNew3(
                                  AST_ASSIGNMENT_EXPRESSION,
                                  (yyvsp[(2) - (4)]),
                                  astNew0(AST_ASSIGN),
                                  (yyvsp[(4) - (4)]))); }
    break;

  case 77:
#line 220 "grammar.y"
    { (yyval) = astNew4(AST_FOR_IN, (yyvsp[(3) - (8)]), (yyvsp[(4) - (8)]), (yyvsp[(6) - (8)]), (yyvsp[(8) - (8)])); }
    break;

  case 78:
#line 222 "grammar.y"
    { (yyval) = astNew0(AST_BREAK); }
    break;

  case 79:
#line 224 "grammar.y"
    { (yyval) = astNew1(AST_BREAK, (yyvsp[(2) - (3)])); }
    break;

  case 80:
#line 226 "grammar.y"
    { (yyval) = astNew0(AST_CONTINUE); }
    break;

  case 81:
#line 228 "grammar.y"
    { (yyval) = astNew1(AST_CONTINUE, (yyvsp[(2) - (3)])); }
    break;

  case 82:
#line 230 "grammar.y"
    { (yyval) = astNew0(AST_RETURN); }
    break;

  case 83:
#line 232 "grammar.y"
    { (yyval) = astNew1(AST_RETURN, (yyvsp[(2) - (3)])); }
    break;

  case 84:
#line 234 "grammar.y"
    { (yyval) = astNew2(AST_METHOD_CALL, (yyvsp[(1) - (4)]), (yyvsp[(3) - (4)])); }
    break;

  case 85:
#line 236 "grammar.y"
    { (yyval) = astNew1(AST_METHOD_CALL, (yyvsp[(1) - (3)])); }
    break;

  case 86:
#line 238 "grammar.y"
    { (yyval) = astNew2(AST_EXPRESSION_LIST, (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)])); }
    break;

  case 87:
#line 240 "grammar.y"
    { (yyval) = (yyvsp[(1) - (1)]); }
    break;

  case 88:
#line 242 "grammar.y"
    { (yyval) = astNew2(AST_BRACKET_EXPRESSION_LIST, (yyvsp[(1) - (4)]), (yyvsp[(3) - (4)])); }
    break;

  case 89:
#line 244 "grammar.y"
    { (yyval) = astNew1(AST_BRACKET_EXPRESSION, (yyvsp[(2) - (3)])); }
    break;

  case 90:
#line 249 "grammar.y"
    { (yyval) = (yyvsp[(1) - (1)]); }
    break;

  case 91:
#line 252 "grammar.y"
    { (yyval) = astNew3(
                                   AST_ASSIGNMENT_EXPRESSION, (yyvsp[(1) - (3)]), (yyvsp[(2) - (3)]), (yyvsp[(3) - (3)])); }
    break;

  case 92:
#line 255 "grammar.y"
    { (yyval) = (yyvsp[(1) - (1)]); }
    break;

  case 93:
#line 257 "grammar.y"
    { (yyval) = (yyvsp[(1) - (1)]); }
    break;

  case 94:
#line 259 "grammar.y"
    { (yyval) = astNew2(AST_BOOLEAN_OR, (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)])); }
    break;

  case 95:
#line 261 "grammar.y"
    { (yyval) = (yyvsp[(1) - (1)]); }
    break;

  case 96:
#line 263 "grammar.y"
    { (yyval) = astNew2(AST_BOOLEAN_AND, (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)])); }
    break;

  case 97:
#line 265 "grammar.y"
    { (yyval) = (yyvsp[(1) - (1)]); }
    break;

  case 98:
#line 267 "grammar.y"
    { (yyval) = astNew2(AST_EQUALS, (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)])); }
    break;

  case 99:
#line 269 "grammar.y"
    { (yyval) = astNew2(AST_NOT_EQUALS, (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)])); }
    break;

  case 100:
#line 271 "grammar.y"
    { (yyval) = (yyvsp[(1) - (1)]); }
    break;

  case 101:
#line 273 "grammar.y"
    { (yyval) = astNew2(AST_BITWISE_OR, (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)])); }
    break;

  case 102:
#line 275 "grammar.y"
    { (yyval) = (yyvsp[(1) - (1)]); }
    break;

  case 103:
#line 277 "grammar.y"
    { (yyval) = astNew2(AST_XOR, (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)])); }
    break;

  case 104:
#line 279 "grammar.y"
    { (yyval) = (yyvsp[(1) - (1)]); }
    break;

  case 105:
#line 281 "grammar.y"
    { (yyval) = astNew2(AST_BITWISE_AND, (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)])); }
    break;

  case 106:
#line 283 "grammar.y"
    { (yyval) = (yyvsp[(1) - (1)]); }
    break;

  case 107:
#line 285 "grammar.y"
    { (yyval) = astNew2(AST_LESS_THAN, (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)])); }
    break;

  case 108:
#line 287 "grammar.y"
    { (yyval) = astNew2(AST_GREATER_THAN, (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)])); }
    break;

  case 109:
#line 290 "grammar.y"
    { (yyval) = astNew2(AST_LESS_THAN_OR_EQUAL_TO, (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)])); }
    break;

  case 110:
#line 293 "grammar.y"
    { (yyval) = astNew2(AST_GREATER_THAN_OR_EQUAL_TO, (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)])); }
    break;

  case 111:
#line 295 "grammar.y"
    { (yyval) = (yyvsp[(1) - (1)]); }
    break;

  case 112:
#line 297 "grammar.y"
    { (yyval) = astNew2(AST_LEFT_SHIFT, (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)])); }
    break;

  case 113:
#line 299 "grammar.y"
    { (yyval) = astNew2(AST_RIGHT_SHIFT, (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)])); }
    break;

  case 114:
#line 302 "grammar.y"
    { (yyval) = astNew2(AST_UNSIGNED_RIGHT_SHIFT, (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)])); }
    break;

  case 115:
#line 304 "grammar.y"
    { (yyval) = (yyvsp[(1) - (1)]); }
    break;

  case 116:
#line 306 "grammar.y"
    { (yyval) = astNew2(AST_PLUS, (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)])); }
    break;

  case 117:
#line 308 "grammar.y"
    { (yyval) = astNew2(AST_MINUS, (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)])); }
    break;

  case 118:
#line 310 "grammar.y"
    { (yyval) = (yyvsp[(1) - (1)]); }
    break;

  case 119:
#line 312 "grammar.y"
    { (yyval) = astNew2(AST_MULT, (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)])); }
    break;

  case 120:
#line 314 "grammar.y"
    { (yyval) = astNew2(AST_DIV, (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)])); }
    break;

  case 121:
#line 316 "grammar.y"
    { (yyval) = astNew2(AST_MOD, (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)])); }
    break;

  case 122:
#line 318 "grammar.y"
    { (yyval) = (yyvsp[(1) - (1)]); }
    break;

  case 123:
#line 320 "grammar.y"
    { (yyval) = astNew2(AST_CAST, (yyvsp[(2) - (4)]), (yyvsp[(4) - (4)])); }
    break;

  case 124:
#line 322 "grammar.y"
    { (yyval) = (yyvsp[(1) - (1)]); }
    break;

  case 125:
#line 324 "grammar.y"
    { (yyval) = (yyvsp[(1) - (1)]); }
    break;

  case 126:
#line 326 "grammar.y"
    { (yyval) = astNew1(AST_NEGATE, (yyvsp[(1) - (2)])); }
    break;

  case 127:
#line 328 "grammar.y"
    { (yyval) = astNew1(AST_ARRAY, (yyvsp[(2) - (3)])); }
    break;

  case 128:
#line 330 "grammar.y"
    { (yyval) = astNew0(AST_ARRAY); }
    break;

  case 129:
#line 332 "grammar.y"
    { (yyval) = (yyvsp[(1) - (1)]); }
    break;

  case 130:
#line 334 "grammar.y"
    { (yyval) = astNew1(AST_BITWISE_INVERT, (yyvsp[(2) - (2)])); }
    break;

  case 131:
#line 336 "grammar.y"
    { (yyval) = astNew1(AST_NOT, (yyvsp[(2) - (2)])); }
    break;

  case 132:
#line 338 "grammar.y"
    { (yyval) = astNew2(AST_NEW, (yyvsp[(2) - (3)]), (yyvsp[(3) - (3)])); }
    break;

  case 133:
#line 340 "grammar.y"
    { (yyval) = (yyvsp[(1) - (1)]); }
    break;

  case 134:
#line 342 "grammar.y"
    { (yyval) = (yyvsp[(2) - (3)]); }
    break;

  case 135:
#line 344 "grammar.y"
    { (yyval) = astNew2(AST_ARRAY_GET, (yyvsp[(1) - (4)]), (yyvsp[(3) - (4)])); }
    break;

  case 136:
#line 346 "grammar.y"
    { (yyval) = astNew1(AST_ARRAY_LENGTH, (yyvsp[(1) - (3)])); }
    break;

  case 137:
#line 348 "grammar.y"
    { (yyval) = (yyvsp[(1) - (1)]); }
    break;

  case 138:
#line 350 "grammar.y"
    { (yyval) = (yyvsp[(1) - (1)]); }
    break;

  case 139:
#line 352 "grammar.y"
    { (yyval) = (yyvsp[(1) - (1)]); }
    break;


/* Line 1267 of yacc.c.  */
#line 2447 "grammar.cpp"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;


  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse look-ahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse look-ahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#ifndef yyoverflow
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEOF && yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}



