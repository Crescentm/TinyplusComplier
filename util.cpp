#include "globals.h"
#include "scan.h"
#include "util.h"
#include <cstdlib>
#include <cstring>
#include <cstdio>

void printToken(TokenType token, const char *tokenString) {
  switch (token) {
  case C_IF:
  case C_THEN:
  case C_ELSE:
  case C_END:
  case C_REPEAT:
  case C_UNTIL:
  case C_READ:
  case C_WRITE:
  case C_INT:
  case C_CHAR:
    fprintf(listing, "reserved word: %s\n", tokenString);
    break;

  case C_ASSIGN:
    fprintf(listing, ":=\n");
    break;
  case C_LT:
    fprintf(listing, "<\n");
    break;
  case C_GT:
    fprintf(listing, ">\n");
    break;
  case C_PLUS:
    fprintf(listing, "+\n");
    break;
  case C_MINUS:
    fprintf(listing, "-\n");
    break;
  case C_TIMES:
    fprintf(listing, "*\n");
    break;
  case C_DIV:
    fprintf(listing, "/\n");
    break;
  case C_MOD:
    fprintf(listing, "\n");
    break;
  case C_LPAREN:
    fprintf(listing, "(\n");
    break;
  case C_RPAREN:
    fprintf(listing, ")\n");
    break;
  case C_COM:
    fprintf(listing, ",\n");
    break;
  case C_SEMI:
    fprintf(listing, ";\n");
    break;
  case C_EQ:
    fprintf(listing, "==\n");
    break;
  case C_NEQ:
    fprintf(listing, "!=\n");
    break;
  case C_NGT:
    fprintf(listing, "<=\n");
    break;
  case C_NLT:
    fprintf(listing, ">=\n");
    break;

  case C_ID:
    fprintf(listing, "ID, name= %s\n", tokenString);
    break;
  case C_NUM:
    fprintf(listing, "NUM, val= %s\n", tokenString);
    break;
  case C_ENDFILE:
    fprintf(listing, "EOF\n");
    break;
  case C_ERROR:
    fprintf(listing, "ERROR: %s\n", tokenString);
    break;
  default:
    fprintf(listing, "Unknown token: %d\n", token);
  }
}

TreeNode *newStmtNode(StmtKind kind) {
  TreeNode *t = (TreeNode *)malloc(sizeof(TreeNode));
  if (t == NULL) {
    fprintf(listing, "Out of memory error at line %d\n", lineno);
  } else {
    for (int i = 0; i < MAXCHILDREN; i++) {
      t->child[i] = NULL;
    }
    t->sibling = NULL;
    t->nodekind = StmtK;
    t->kind.stmt = kind;
    t->lineno = lineno;
  }
  return t;
}

TreeNode *newExpNode(ExpKind kind) {
  TreeNode *t = (TreeNode *)malloc(sizeof(TreeNode));
  if (t == NULL) {
    fprintf(listing, "Out of memory error at line %d\n", lineno);
  } else {
    for (int i = 0; i < MAXCHILDREN; i++) {
      t->child[i] = NULL;
    }
    t->sibling = NULL;
    t->nodekind = ExpK;
    t->kind.exp = kind;
    t->lineno = lineno;
    t->type = Void;
  }
  return t;
}

TreeNode *newDeclarNode(DeclarKind kind) {
  TreeNode *t = (TreeNode *)malloc(sizeof(TreeNode));
  if (t == NULL) {
    fprintf(listing, "Out of memory error at line %d\n", lineno);
  } else {
    for (int i = 0; i < MAXCHILDREN; i++) {
      t->child[i] = NULL;
    }
    t->sibling = NULL;
    t->nodekind = DeclarK;
    t->kind.declar = kind;
    t->lineno = lineno;
    t->type = Void;
  }
  return t;
}

char *copyString(char *s) {
  int n;
  char *t;
  if (s == NULL)
    return NULL;
  n = strlen(s) + 1;
  t = (char *)malloc(sizeof(char) * n);
  if (t == NULL) {
    fprintf(listing, "Out of memory error at line %d\n", lineno);
  } else {
    strcpy(t, s);
  }
  return t;
}

static int indentno = 0;

#define INDENT indentno += 2
#define UNINDENT indentno -= 2

static void printSpaces(void) {
  int i;
  for (i = 0; i < indentno; i++) {
    fprintf(listing, " ");
  }
}

void printTree(TreeNode *tree) {
  int i;
  INDENT;
  while (tree != NULL) {
    printSpaces();
    if (tree->nodekind == StmtK) {
      switch (tree->kind.stmt) {
      case IfK:
        fprintf(listing, "If\n");
        break;
      case RepeatK:
        fprintf(listing, "Repeat\n");
        break;
      case AssignK:
        fprintf(listing, "Assign to: %s\n", tree->attr.name);
        break;
      case ReadK:
        fprintf(listing, "Read: %s\n", tree->attr.name);
        break;
      case WriteK:
        fprintf(listing, "Write\n");
        break;
      default:
        fprintf(listing, "Unknown ExpNode kind\n");
        break;
      }
    } else if (tree->nodekind == ExpK) {
      switch (tree->kind.exp) {
      case OpK:
        fprintf(listing, "Op: ");
        printToken(tree->attr.op, "\0");
        break;
      case ConstK:
        fprintf(listing, "Const: %d\n", tree->attr.val);
        break;
      case IdK:
        fprintf(listing, "Id: %s\n", tree->attr.name);
        break;
      case Charstringk:
        fprintf(listing, "Charater string: %s\n", tree->attr.charstring);
        break;
      default:
        fprintf(listing, "Unknown ExpNode kind\n");
        break;
      }
    } else if (tree->nodekind == DeclarK) {
      switch (tree->kind.declar) {
      case Int:
        fprintf(listing, "Declare an integer variable: %s\n", tree->attr.name);
        break;
      case Char:
        fprintf(listing, "Declare a character variable: %s\n", tree->attr.name);
        break;
      default:
        fprintf(listing, "Unknown DeclarNode kind\n");
        break;
      }
    } else {
      fprintf(listing, "Unknown node kind\n");
    }
    for (i = 0; i < MAXCHILDREN; i++)
      printTree(tree->child[i]);
    tree = tree->sibling;
  }
  UNINDENT;
}