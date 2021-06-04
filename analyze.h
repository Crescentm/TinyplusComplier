#ifndef _ANALYZE_H_
#define _ANALYZE_H_

// 通过先序遍历创建符号表
void buildSymtab(TreeNode *);

// 后续遍历来进行符号检测
void typeCheck(TreeNode *);

#endif