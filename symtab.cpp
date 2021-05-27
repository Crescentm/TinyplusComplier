#include "symtab.h"
#include <bits/types/FILE.h>
#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
  int memloc; // 变量内存地址
  struct BucketListRec *next;
} * BucketList;

// hash表
static BucketList hashTable[SIZE];

// st_insert: 将变量的lineno和loc插入符号表的
void st_insert(char *name, int lineno, int loc) {
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
    l->lines->next = NULL;
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