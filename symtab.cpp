#include <cstdlib>
#include <cstdio>
#include <cstring>
#include "globals.h"
#include "symtab.h"
#include "util.h"

// hash表的大小
#define SIZE 211

// SHIFT是哈希函数中作为乘法器使用的2的幂
#define SHIFT 4

// hash 函数
static int hash(char *key) {
  int temp = 0;
  int i = 0;
  while (key[i] != '\0') {
    temp = ((temp << SHIFT) + key[i]) % SIZE;
    i++;
  }
  return temp;
}

// 变量引用的行号列表
typedef struct LineListRec {
  int lineno;
  struct LineListRec *next;
} * LineList;

// 每个变量在bucket中的记录，包括名称、分配的内存位置、出现的行号列表
typedef struct BucketListRec {
  char *name;
  LineList lines;
  DeclarKind type;
  int memloc; // 变量内存地址
  union {
    int val;
    char *charstring;
  } val_exe;
  struct BucketListRec *next;
} * BucketList;

// hash表
static BucketList hashTable[SIZE];

// st_insert: 将变量的lineno和loc插入符号表的
void st_insert(char *name, int lineno, int loc, DeclarKind kind) {
  int h = hash(name);
  BucketList l = hashTable[h];

  while ((l != NULL) && (strcmp(name, l->name) != 0)) {
    l = l->next;
  }
  if (l == NULL) {
    l = (BucketList)malloc(sizeof(BucketList));
    l->name = name;
    l->lines = (LineList)malloc(sizeof(LineList));
    l->lines->lineno = lineno;
    l->memloc = loc;
    l->type = kind;
    l->lines->next = NULL;
    if(kind ==Int)
    {
      l->val_exe.val=0;
    }
    else
      l->val_exe.charstring=NULL;
    l->next = hashTable[h];
    hashTable[h] = l; // 头插法
  } else {
    LineList t = l->lines;
    while (t->next != NULL) {
      t = t->next;
    }
    t->next = (LineList)malloc(sizeof(LineList));
    t->next->lineno = lineno;
    t->next = NULL;
  }
}

// st_lookup: 返回变量的内存地址
int st_lookup(char *name) {
  int h = hash(name);
  BucketList l = hashTable[h];
  while ((l != NULL) && (strcmp(name, l->name) != 0)) {
    l = l->next;
  }
  if (l == NULL) {
    return -1;
  } else {
    return l->type;
  }
}

//返回变量信息，int型返回数值，char型返回头指针
int st_val(char *name)
{
  int h = hash(name);
  BucketList l = hashTable[h];
  while ((l != NULL) && (strcmp(name, l->name) != 0)) {
    l = l->next;
  }
  if (l == NULL) {
    return -1;
  } else {
    return l->val_exe.val;
  }
}
char* st_char(char *name)
{
  int h = hash(name);
  BucketList l = hashTable[h];
  while ((l != NULL) && (strcmp(name, l->name) != 0)) {
    l = l->next;
  }
  if (l == NULL) {
    return NULL;
  } else {
    return l->val_exe.charstring;
  }
}
// st_assival: 变量赋值
int st_assival(char *name, int ischar, int valnum, char *valchar) {
  int h = hash(name);
  BucketList l = hashTable[h];
  while ((l != NULL) && (strcmp(name, l->name) != 0)) {
    l = l->next;
  }
  if (l == NULL) {
    return -1;
  } else {
      if (ischar == 1)

          l->val_exe.charstring = copyString(valchar);
    else
      l->val_exe.val=valnum;

    return l->memloc;
  }
}
// printSymTab: 打印符号表
void printSymTab(FILE *listing) {
  fprintf(listing, "Variable Name  Location   Line Numbers\n");
  fprintf(listing, "-------------  --------   ------------\n");
  for (int i = 0; i < SIZE; i++) {
    if (hashTable[i] != NULL) {
      BucketList l = hashTable[i];
      while (l != NULL) {
        LineList t = l->lines;
        fprintf(listing, "%-14s", l->name);
        fprintf(listing, "%-8d  ", l->memloc);
        while (t != NULL) {
          fprintf(listing, "%4d ", t->lineno);
          t = t->next;
        }
        fprintf(listing, "\n");
        l = l->next;
      }
    }
  }
}