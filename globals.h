#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* MAXRESERVED = the number of reserved words 保留字最大值*/
#define MAXRESERVED 8

typedef enum {
  C_ENDFILE,
  C_ERROR,
  /* reserved Keyword 保留字 */
  C_ELSE,
  C_IF,
  C_INT,
  C_RETURN,
  C_VOID,
  C_WHILE,
  C_WRITE,
  C_READ,
  /* multicharacter tokens */
  C_ID,
  C_NUM,
  /* special symbols */
  C_PLUS,    // +
  C_MINUS,   // -
  C_TIMES,   // *
  C_DIV,     // /
  C_ASSIGN,  // =
  C_MOD,     // %
  C_LT,      // <
  C_GT,      // >
  C_LPAREN,  // (
  C_RPAREN,  // )
  C_LBRACE,  // {
  C_RBRACE,  // }
  C_LSQUARE, // [
  C_RSQUARE, // ]
  C_COM,     // ,
  C_SEMI,    // ;
  C_EQ,      // ==
  C_NEQ,     // !=
  C_NGT,     // <=
  C_NLT,     // >=
  // k_EOF, k_ID, k_NUM, k_ERROR, k_NONE

} TokenType;

extern FILE *source;  /* 源代码 */
extern FILE *listing; /* 输出文件 */
// extern FILE *code;    /* code text file for TM simulator */

extern int lineno; /* 行号 */

/**************************************************/
/******************* 抽象语法树 *********************/
/**************************************************/

typedef enum { StmtK, ExpK } NodeKind;
typedef enum { IfK, RepeatK, AssignK, ReadK, WriteK } StmtKind;
typedef enum { OpK, ConstK, IdK } ExpKind;

/* ExpType 用于类型检测 */
typedef enum { Void, Integer, Boolean } ExpType;

#define MAXCHILDREN 3

typedef struct treeNode {
  struct treeNode *child[MAXCHILDREN];
  struct treeNode *sibling;
  int lineno;
  NodeKind nodekind;
  union {
    StmtKind stmt;
    ExpKind exp;
  } kind;
  union {
    TokenType op;
    int val;
    char *name;
  } attr;
  ExpType type; /* 用于类型检测 */
} TreeNode;

/**************************************************/
/*****************   调试标志    *******************/
/**************************************************/

// EchoSource = true 打印源代码
extern int EchoSource;

// TraceScan = true 打印token信息
extern int TraceScan;

// TraceParse = true 打印语法树
extern int TraceParse;

// TraceAnalyze = true 打印对符号表的操作
extern int TraceAnalyze;

// TraceCode = true 将注释打印到输出文件
extern int TraceCode;

// Error = true 在发生错误时，防止通过编译
extern int Error;
#endif
