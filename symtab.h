#ifndef _SYMTAB_H_
#define _SYMTAB_H_

// 第一次遇到变量时，将变量的lineno和loc插入符号表
void st_insert(char *name, int lineno, int loc, DeclarKind kind);

// 返回变量的内存位置
int st_lookup(char *name);

// 将符号表打印到文件
void printSymTab(FILE *listing);

#endif