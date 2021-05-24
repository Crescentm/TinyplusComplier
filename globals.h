#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* MAXRESERVED = the number of reserved words ���������ֵ*/
#define MAXRESERVED 8

typedef enum {
  C_ENDFILE,
  C_ERROR,
  /* reserved Keyword ������ */
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

extern FILE *source;  /* Դ���� */
extern FILE *listing; /* ����ļ� */
// extern FILE *code;    /* code text file for TM simulator */

extern int lineno; /* �к� */

/**************************************************/
/******************* �����﷨�� *********************/
/**************************************************/

typedef enum { StmtK, ExpK } NodeKind;
typedef enum { IfK, RepeatK, AssignK, ReadK, WriteK } StmtKind;
typedef enum { OpK, ConstK, IdK } ExpKind;

/* ExpType �������ͼ�� */
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
  ExpType type; /* �������ͼ�� */
} TreeNode;

/**************************************************/
/*****************   ���Ա�־    *******************/
/**************************************************/

// EchoSource = true ��ӡԴ����
extern int EchoSource;

// TraceScan = true ��ӡtoken��Ϣ
extern int TraceScan;

// TraceParse = true ��ӡ�﷨��
extern int TraceParse;

// TraceAnalyze = true ��ӡ�Է��ű�Ĳ���
extern int TraceAnalyze;

// TraceCode = true ��ע�ʹ�ӡ������ļ�
extern int TraceCode;

// Error = true �ڷ�������ʱ����ֹͨ������
extern int Error;
#endif
