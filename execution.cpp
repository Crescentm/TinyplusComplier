#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "execution.h"
#include "analyze.h"
#include "symtab.h"

FunctStack stack[STACKLEN];
TreeNode *firstnode;
int stackpoint;
int IS_OVER = 0;

static void genFtmt(TreeNode *tree);
static int genExpi(TreeNode *tree);
static char *genExpc(TreeNode *tree);
/* tmpOffset is the memory offset for temps
It is decremented each time a temp is
stored, and incremeted when loaded again
tmpOffset 是 temp 的内存偏移量，每次存储一个 temp 时递减，再次加载时递增
*/
static int tmpOffset = 0;

static char *genExpc(TreeNode *tree) {
if (tree == NULL)
      return NULL;

  int loc[3];
  char *cloc = NULL;

  loc[0] = st_type(tree->attr.name);
  if (loc[0] == Char)
    return st_char(tree->attr.name);
  else //
    return NULL;
}

static int genExpi(TreeNode *tree) {
  if (tree == NULL)
      return 0;//不可发生
  TreeNode *p1, *p2, *p3;
  p1 = tree->child[0];
  p2 = tree->child[1];
  p3 = tree->child[2];
  int loc[3];

  switch (tree->kind.exp) {
  case OpK: {
    switch (tree->attr.op) {
    case C_LT:
      if (genExpi(p1) < genExpi(p2))
        return 1;
      else
        return 0;
      break;
    case C_GT:
      if (genExpi(p1) > genExpi(p2))
        return 1;
      else
        return 0;
      break;
    case C_NEQ:
      if (genExpi(p1) != genExpi(p2))
        return 1;
      else
        return 0;
      break;
    case C_EQ:
      if (genExpi(p1) == genExpi(p2))
        return 1;
      else
        return 0;
      break;
    case C_NLT:
      if (genExpi(p1) >= genExpi(p2))
        return 1;
      else
        return 0;
      break;
    case C_NGT:
      if (genExpi(p1) <= genExpi(p2))
        return 1;
      else
        return 0;
      break;
    case C_PLUS:
      loc[0] = genExpi(p1);
      loc[1] = genExpi(p2);
      return loc[0] + loc[1];
      break;
    case C_MINUS:
      loc[0] = genExpi(p1);
      loc[1] = genExpi(p2);
      return loc[0] - loc[1];
      break;
    case C_TIMES:
      loc[0] = genExpi(p1);
      loc[1] = genExpi(p2);
      return loc[0] * loc[1];
      break;
    case C_DIV:
      loc[0] = genExpi(p1);
      loc[1] = genExpi(p2);
      return loc[0] / loc[1];
      break;
    default:
      break;
    }
    break;
  }
  case ConstK:
    return tree->attr.val;
    break;

  case IdK:
    loc[0] = st_type(tree->attr.name);
    if (loc[0] == Int)
      return st_val(tree->attr.name);
    else //不可发生
      return 0;
    break;
  default:
    break;
  }
  return 0; //不可发生
}

static void genFtmt(TreeNode *tree) {
  if (tree == NULL)
      return;
  TreeNode *p1, *p2, *p3;
  int savedLoc1, savedLoc2, currentLoc;
  int loc;
  char cloc_m[256];
  char *cloc = cloc_m;

  switch (tree->kind.stmt) {
  case IfK:
    p1 = tree->child[0];
    p2 = tree->child[1];
    p3 = tree->child[2];

    if (p1->kind.exp == Charstringk && genExpc(p1) != NULL) //字符型均返回1执行
    {
      execution(p2);
    } else {           //非字符型
      if (genExpi(p1)) //返回非0执行p2，为0执行p3
          execution(p2);
      else
          execution(p3);
    }
    break;

  case RepeatK:
    p1 = tree->child[0];
    p2 = tree->child[1];

    do {
        execution(p1);

      if (p2->kind.exp == Charstringk &&
          genExpc(p1) != NULL) //字符型均返回1执行
        loc = 1;
      else
        loc = genExpi(p2);
    } while (loc);
    break;

  case AssignK:
    p1 = tree->child[0];
    if (p1->kind.exp == Charstringk)
      st_assival(tree->attr.name, 1, 0, p1->attr.charstring);
    else
      st_assival(tree->attr.name, 0, genExpi(p1), NULL);
    break;

  case ReadK:
    loc = st_type(tree->attr.name);
    printf("请输入%s:\n", tree->attr.name); //提醒输入
    if (loc == Int) {
      scanf("%d", &loc);
      st_assival(tree->attr.name, 0, loc, NULL);
    } else {
      scanf("%s", cloc);
      st_assival(tree->attr.name, 1, 0, cloc);
    }
    break;

  case WriteK:
    p1 = tree->child[0];
    cloc = genExpc(p1);
    printf("%s:\n", p1->attr.name); //显示输出
    if (cloc != NULL)
      printf("%s\n", cloc);
    else
      printf("%d\n", genExpi(p1));
    break;
  default:
    break;
  }
}

void execution(TreeNode *tree) {
    TreeNode* temp;
    temp = tree;
  while (temp != NULL) {
    if (temp->nodekind == StmtK)
      genFtmt(temp);

    temp = temp->sibling;
  }
}
