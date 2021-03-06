#ifndef _SYMTAB_H_
#define _SYMTAB_H_

// 第一次遇到变量时，将变量的lineno和loc插入符号表
void st_insert(char *name, int lineno, int loc, DeclarKind kind);

// 返回变量的kind类型
int st_lookup(char *name);

// st_assival: 变量赋值
int st_assival(char *name, int ischar, int valnum, char *valchar);

//返回变量信息，int型返回数值，char型返回头指针
int st_val(char *name);
char *st_char(char *name);
// 将符号表打印到文件
void printSymTab(FILE *listing);
DeclarKind st_type(char *name);

#endif