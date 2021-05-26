#ifndef _UTIL_H_
#define _UTIL_H_

// 打印token和token的词素
void printToken(TokenType, const char *);

// 创建语法树新语句节点
TreeNode *newStmtNode(StmtKind);

// 创建语法树的新表达式节点
TreeNode *newExpNode(ExpKind);

// 创建字符串的新副本
char *copyString(char *);

// 将语法树打印到文件
void printTree(TreeNode *);

#endif