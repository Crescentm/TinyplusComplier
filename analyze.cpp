#include "globals.h"
#include "symtab.h"
#include "analyze.h"
//#include <bits/types/locale_t.h>
#include <cstdio>
#include <cstring>

static int location = 0;
static TreeNode *declared_var = NULL;

// // 判断是否已经定义
// static bool is_declare(TreeNode *t) {
//   TreeNode *tmp = declared_var;
//   while (tmp != NULL) {
//     if (strcmp(t->attr.name, tmp->attr.name) == 0) {
//       return true;
//     }
//     tmp = tmp->sibling;
//   }
//   return false;
// }

// 返回已定义变量的exp类型方便类型检查
static ExpType typeofvar(TreeNode *t) {
  TreeNode *tmp = declared_var;
  while (tmp != NULL) {
    if (strcmp(t->attr.name, tmp->attr.name) == 0) {
      if (tmp->kind.declar == Int)
        return Integer;
      else if (tmp->kind.declar == Char)
        return Character;
      else
        continue;
    }
    tmp = tmp->sibling;
  }
  return Void;
}

// 遍历语法树
static void traverse(TreeNode *t, void (*preProc)(TreeNode *),
                     void (*postProc)(TreeNode *)) {
  if (t != NULL) {
    preProc(t);
    {
      int i;
      for (i = 0; i < MAXCHILDREN; i++) {
        traverse(t->sibling, preProc, postProc);
      }
      postProc(t);
      traverse(t->sibling, preProc, postProc);
    }
  }
}

static void nullProc(TreeNode *t) {
  if (t == NULL)
    return;
  else
    return;
}

static void insertNode(TreeNode *t) {
  TreeNode *var = declared_var;
  while (var != NULL) {
    if (st_lookup(var->attr.name) != -1) {
      fprintf(listing, "multi declare of variable at line %d: %s\n", t->lineno,
              t->attr.name);
      Error = true;
    } else {
      st_insert(t->attr.name, t->lineno, location++, t->kind.declar);
    }
  }

  switch (t->nodekind) {
  case StmtK:
    switch (t->kind.stmt) {
    case AssignK:
    case ReadK:
      if (st_lookup(t->attr.name) == -1) {
        fprintf(listing, "undeclare of variable at line %d: %s\n", t->lineno,
                t->attr.name);
        Error = true;
      } else {
        st_insert(t->attr.name, t->lineno, -1, NONE);
      }
      break;
    default:
      break;
    }
  case ExpK:
    switch (t->kind.exp) {
    case IdK:
      if (st_lookup(t->attr.name) == -1) {
        fprintf(listing, "undeclare of variable at line %d: %s\n", t->lineno,
                t->attr.name);
        Error = true;
      } else {
        st_insert(t->attr.name, t->lineno, -1, NONE);
      }
      break;
    default:
      break;
    }
  default:
    break;
  }
}

void buildSymtab(TreeNode *syntaxTree) {
  declared_var = syntaxTree->child[0];
  traverse(syntaxTree->sibling, insertNode, nullProc);
  if (TraceAnalyze) {
    fprintf(listing, "\nSymbol table:\n\n");
    printSymTab(listing);
  }
  return;
}

static void typeError(TreeNode *t, const char *message) {
  fprintf(listing, "Type error at line %d: %s\n", t->lineno, message);
  Error = true;
}

static void checkNode(TreeNode *t) {
  switch (t->nodekind) {
  case ExpK:
    switch (t->kind.exp) {
    case OpK:
      if ((t->child[0]->type != Integer) || (t->child[1]->type != Integer)) {
        typeError(t, "Op applied to non-integer");
      }
      if ((t->attr.op == C_EQ) || (t->attr.op == C_NEQ) ||
          (t->attr.op == C_LT) || (t->attr.op == C_GT) ||
          (t->attr.op == C_NGT) || (t->attr.op == C_NLT)) {
        t->type = Boolean;
      }
      break;
    case ConstK:
      t->type = Integer;
    case IdK:
      t->type = typeofvar(t);
    case Charstringk:
      t->type = Character;
    default:
      break;
    }
  case StmtK:
    switch (t->kind.stmt) {
    case IfK:
      if (t->child[0]->type != Boolean) {
        typeError(t->child[0], "if test is not Boolean");
      }
      break;
    case AssignK:
      if ((t->child[0]->type != Integer) || (t->child[0]->type != Character))
        typeError(t->child[0],
                  "assignment of non-integer or non-character value");
      break;
    case WriteK:
      if ((t->child[0]->type != Integer) || (t->child[0]->type != Character))
        typeError(t->child[0], "write of non-integer or non-character value");
      break;
    case RepeatK:
      if (t->child[1]->type == Integer)
        typeError(t->child[1], "repeat test is not Boolean");
      break;
    default:
      break;
    }
  default:
    break;
  }
}

void typeCheck(TreeNode *syntaxTree) {
  traverse(syntaxTree->sibling, nullProc,
           checkNode); //第三次遍历语法树，完成类型检查
}