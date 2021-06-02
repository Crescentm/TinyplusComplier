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
static char* genExpc(TreeNode *tree);
/* tmpOffset is the memory offset for temps
It is decremented each time a temp is
stored, and incremeted when loaded again
tmpOffset 是 temp 的内存偏移量，每次存储一个 temp 时递减，再次加载时递增 
*/
static int tmpOffset = 0;

static int genExpi(TreeNode *tree)
{
	
}

static void genFtmt(TreeNode *tree)
{
	TreeNode *p1, *p2, *p3;
	int savedLoc1, savedLoc2, currentLoc;
	int loc;
	char *cloc;

	switch(tree->kind.stmt)
	{
	case IfK:
		p1 = tree->child[0];
		p2 = tree->child[1];
		p3 = tree->child[2];

		if(p1->kind.exp == Charstringk)//字符型均返回1执行
		{
			genFtmt(p2);
		}else{
			if(genExpi(p1))//返回非0执行p2，为0执行p3
				genFtmt(p2);
			else
				genFtmt(p3);
		}
		break;

	case RepeatK:
		p1 = tree->child[0];
		p2 = tree->child[1];

		do
		{
			genFtmt(p1);

			if(p2->kind.exp == Charstringk)//字符型均返回1执行
				loc=1;
			else
				loc=genExpi(p2);
		} while (loc);
		break;


	case AssignK:
		p1 = tree->child[0];
		if(p1->kind.exp == Charstringk)
			st_assival(tree->attr.name,1,0,p1->attr.charstring);
		else
			st_assival(tree->attr.name,0,genExpi(p1),NULL);
		break;
		case ReadK:
		loc=st_lookup(tree->attr.name);
		printf("%s:\n",loc);//提醒输入
		if(loc == Int)
		{
			scanf("%d",&loc);
			st_assival(tree->attr.name,0,loc,NULL);
		}
		else
		{
			scanf("%s",cloc);
			st_assival(tree->attr.name,1,0,NULL);
		}
		break;
		
		case WriteK:
			p1 = tree->child[0];
			cloc=genExpc(p1);
			if(cloc!=NULL)
				printf("%s\n",cloc);
			else
				printf("%d\n",genExpi(p1));
			break;
		default:
			break;
	}
}
static void execution(TreeNode * tree)
{
	if(tree != NULL)
	{
		if(tree->nodekind==StmtK)
			genFtmt(tree);
		
		execution(tree->sibling);
	}
}



