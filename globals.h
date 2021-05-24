/****************************************************/
/* File: globals.h                                  */
/* Global types and vars for TINY compiler          */
/* must come before other include files             */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

/* MAXRESERVED = the number of reserved words ���������ֵ*/
#define MAXRESERVED 8

typedef enum
/* book-keeping tokens */
{
    C_ENDFILE,
    C_ERROR,
   /* reserved Keyword ������ */
    C_ELSE,C_IF,C_INT,C_RETURN,C_VOID,C_WHILE,
   C_WRITE,C_READ,  //C_RANDOM/* random */,
   /* multicharacter tokens */
    C_ID,C_NUM,
   /* special symbols */
    C_PLUS,C_MINUS,C_TIMES,C_DIV,//+ - * /
    C_ASSIGN,C_MOD,C_LT,C_GT,//  = % <  >
    C_LPAREN,C_RPAREN,C_LBRACE,C_RBRACE,C_LSQUARE, C_RSQUARE,// ( ) { } [ ]
    C_COM,C_SEMI,// ,  ;
    C_EQ,C_NEQ,C_NGT,C_NLT,// == !=  <=  >=
   //k_EOF, k_ID, k_NUM, k_ERROR, k_NONE

} TokenType;

extern FILE *source;  /* source code text file */
extern FILE *listing; /* listing output text file */
extern FILE *code;    /* code text file for TM simulator */

extern int lineno; /* source line number for listing */

/**************************************************/
/***********   Syntax tree for parsing ************/
/**************************************************/

typedef enum
{
    StmtK,
    ExpK
} NodeKind;
typedef enum
{
    IfK,
    RepeatK,
    AssignK,
    ReadK,
    WriteK
} StmtKind;
typedef enum
{
    OpK,
    ConstK,
    IdK
} ExpKind;

/* ExpType is used for type checking */
typedef enum
{
    Void,
    Integer,
    Boolean
} ExpType;

#define MAXCHILDREN 3

typedef struct treeNode
{
   struct treeNode *child[MAXCHILDREN];
   struct treeNode *sibling;
    int lineno;
    NodeKind nodekind;
    union
    {
        StmtKind stmt;
        ExpKind exp;
    } kind;
    union
    {
        TokenType op;
        int val;
      char *name;
    } attr;
   ExpType type; /* for type checking of exps */
} TreeNode;

/**************************************************/
/***********   Flags for tracing       ************/
/**************************************************/

/* EchoSource = TRUE causes the source program to
 * be echoed to the listing file with line numbers
 * during parsing
 */
extern int EchoSource;

/* TraceScan = TRUE causes token information to be
 * printed to the listing file as each token is
 * recognized by the scanner
 */
extern int TraceScan;

/* TraceParse = TRUE causes the syntax tree to be
 * printed to the listing file in linearized form
 * (using indents for children)
 */
extern int TraceParse;

/* TraceAnalyze = TRUE causes symbol table inserts
 * and lookups to be reported to the listing file
 */
extern int TraceAnalyze;

/* TraceCode = TRUE causes comments to be written
 * to the TM code file as code is generated
 */
extern int TraceCode;

/* Error = TRUE prevents further passes if an error occurs */
extern int Error;
#endif
