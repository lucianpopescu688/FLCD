/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

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

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_PARSER_TAB_H_INCLUDED
# define YY_YY_PARSER_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    INT_T = 258,                   /* INT_T  */
    STRING_T = 259,                /* STRING_T  */
    BOOLEAN_T = 260,               /* BOOLEAN_T  */
    PROCESS_T = 261,               /* PROCESS_T  */
    STATE_T = 262,                 /* STATE_T  */
    GROUP_T = 263,                 /* GROUP_T  */
    IP_ADDRESS_T = 264,            /* IP_ADDRESS_T  */
    DEPENDS_ON = 265,              /* DEPENDS_ON  */
    IF = 266,                      /* IF  */
    ELSE = 267,                    /* ELSE  */
    SWITCH = 268,                  /* SWITCH  */
    CASE = 269,                    /* CASE  */
    DEFAULT = 270,                 /* DEFAULT  */
    WHILE = 271,                   /* WHILE  */
    FOR = 272,                     /* FOR  */
    BREAK = 273,                   /* BREAK  */
    CONTINUE = 274,                /* CONTINUE  */
    TRY = 275,                     /* TRY  */
    CATCH = 276,                   /* CATCH  */
    WAIT_SECONDS = 277,            /* WAIT_SECONDS  */
    PING_SERVER = 278,             /* PING_SERVER  */
    SPIN_UP_INSTANCE = 279,        /* SPIN_UP_INSTANCE  */
    KILL_INSTANCE = 280,           /* KILL_INSTANCE  */
    LOG_ALERT = 281,               /* LOG_ALERT  */
    START = 282,                   /* START  */
    STOP = 283,                    /* STOP  */
    RESTART = 284,                 /* RESTART  */
    ENABLE = 285,                  /* ENABLE  */
    DISABLE = 286,                 /* DISABLE  */
    STATUS = 287,                  /* STATUS  */
    AUTOSTART = 288,               /* AUTOSTART  */
    AND = 289,                     /* AND  */
    OR = 290,                      /* OR  */
    NOT = 291,                     /* NOT  */
    TRUE_T = 292,                  /* TRUE_T  */
    FALSE_T = 293,                 /* FALSE_T  */
    RUNNING_T = 294,               /* RUNNING_T  */
    STOPPED_T = 295,               /* STOPPED_T  */
    ERROR_T = 296,                 /* ERROR_T  */
    EQ = 297,                      /* EQ  */
    NE = 298,                      /* NE  */
    LE = 299,                      /* LE  */
    GE = 300,                      /* GE  */
    ID = 301,                      /* ID  */
    NUMBER = 302,                  /* NUMBER  */
    STRING_CONST = 303             /* STRING_CONST  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_PARSER_TAB_H_INCLUDED  */
