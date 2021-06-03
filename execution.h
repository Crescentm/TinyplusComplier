#pragma once
#ifndef _EXECUTION_H_
#define _EXECUTION_H_
#define _CRT_SECURE_NO_WARNINGS
#include "globals.h"
#include "util.h"
/*函数名长度*/
#define FUNNAMELEN 100
/*参数个数*/
#define PARANAMELEN 10
/*定义变量个数*/
#define VARINAMELEN 10
/*定义的数组的长度*/
#define ARRAYLEN 100
#define STACKLEN 100

typedef struct {
  char name[100];
  int val[ARRAYLEN];
} ValName;
typedef struct {
  char name[FUNNAMELEN];     //函数名
  ValName para[PARANAMELEN]; //参数列表
  ValName vari[VARINAMELEN]; //定义变量列表
  int reval;                 //返回值
  int varinum;               //参数个数
  int paranum;               //变量个数
} FunctStack;
/*定义函数调用栈*/
extern FunctStack stack[STACKLEN];

/*保存语法树的头节点*/
extern TreeNode *firstnode;
/*定义栈顶指针*/
extern int stackpoint;
/*入口函数*/
void execution(TreeNode *tree);
/*递归遍历解释执行*/
int Traverse(TreeNode *tree);
/*检测变量是否是数组，返回偏移量*/
int Is_Array(TreeNode *tree);
#endif