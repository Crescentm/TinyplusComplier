/****************************************************/
/* File: parse.c                                    */
/* The parser implementation for the TINY compiler  */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include "globals.h"
#include "util.h"
#include "scan.h"
#include "parse.h"

static TokenType Token, iToken,
    tToken; /* holds current token,idnameToken,typeToken */
static char itokenString[MAXTOKENLEN + 1], ttokenString[MAXTOKENLEN + 1];
/*holds idnameTokenString typeTokenString*/

char *Scope;
/*current scope*/

/* function prototypes for recursive calls */
static TreeNode *program(void);
static TreeNode *declaration_list(void);
static TreeNode *declaration(void);
static TreeNode *stmt_sequence(void);
static TreeNode *statement(void);
static TreeNode *if_stmt(void);
static TreeNode *repeat_stmt(void);
static TreeNode *assign_stmt(void);
static TreeNode *read_stmt(void);
static TreeNode *write_stmt(void);
static TreeNode *exp(void);
static TreeNode *simple_exp(void);
static TreeNode *term(void);
static TreeNode *factor(void);

static void syntaxError(char const *message) {
  fprintf(listing, "\n>>> ");
  fprintf(listing, "Syntax error at line %d: %s", lineno, message);
  Error = true;
}

// for the error recovery ...
static void consumeUntil(const TokenType Type) {
  while (Token != Type && Token != C_ENDFILE)
    Token = getToken();
}
static void consumeUntil(const TokenType type1, const TokenType type2) {
  while (Token != type1 && Token != type2 && Token != C_ENDFILE)
    Token = getToken();
}

static void match(TokenType expected) {
  if (Token == expected)
    Token = getToken();
  else {
    syntaxError("unexpected token -> ");
    printToken(Token, tokenString);
    fprintf(listing, "      ");
  }
}
/*
program 			?	declaration-list  stmt-sequence
*/
static TreeNode *program(void) {
  TreeNode *program = NULL, *last = NULL, *temp = NULL;

  if (Token != C_INT && Token != C_CHAR) {
    syntaxError("unexpected token -> ");
    printToken(Token, tokenString);
  } else {
    if ((temp = declaration_list()) != NULL) {
      program = temp;
      temp = stmt_sequence();
      program->sibling = temp;
    }
  }
  return program;
}

/*
declaration-list 	→ 	declaration-list declaration | declaration
*/

TreeNode *declaration_list(void) {
  TreeNode *last = NULL, *temp = NULL;

  while (Token == C_INT || Token == C_CHAR) {
    if ((temp = declaration()) != NULL) {
      last->sibling = temp;
      last = temp;
    }
    Token = getToken();
  }
  return temp;
}

/*
declaration 		→ 	type-specifier identifier;
type-specifier 		→ 	int | char
*/

TreeNode *declaration(void) {
  TreeNode *t;
  if (Token == C_INT) {
    match(C_INT);
    t = newDeclarNode(Int);
  } else {
    match(C_CHAR);
    t = newDeclarNode(Char);
  }
  if ((t != NULL) && (Token == C_ID))
    t->attr.name = copyString(tokenString);
  match(C_ID);
  match(C_SEMI);
  return t;
}

/*
stmt-sequence 	?	stmt-sequence statement | statement
*/
TreeNode *stmt_sequence(void) {
  TreeNode *t = statement();
  TreeNode *p = t;
  while ((Token != C_ENDFILE) && (Token != C_END) && (Token != C_ELSE) &&
         (Token != C_UNTIL)) {
    TreeNode *q;
    q = statement();
    if (q != NULL) {
      if (t == NULL)
        t = p = q;
      else /* now p cannot be NULL either */
      {
        p->sibling = q;
        p = q;
      }
    }
  }
  return t;
}
/*
statement			?	if-stmt | repeat-stmt | assign-stmt |
read-stmt | write-stmt
*/

TreeNode *statement(void) {
  TreeNode *t = NULL;
  switch (Token) {
  case C_IF:
    t = if_stmt();
    break;
  case C_REPEAT:
    t = repeat_stmt();
    break;
  case C_ID:
    t = assign_stmt();
    break;
  case C_READ:
    t = read_stmt();
    break;
  case C_WRITE:
    t = write_stmt();
    break;
  default:
    syntaxError("unexpected token -> ");
    printToken(Token, tokenString);
    Token = getToken();
    break;
  } /* end case */
  return t;
}
/*
if-stmt 			?  	if (exp) then stmt-sequence end
                | if (exp) then stmt-sequence else stmt-sequence end
*/
TreeNode *if_stmt(void) {
  TreeNode *t = newStmtNode(IfK);
  match(C_IF);
  if (t != NULL)
    t->child[0] = exp();
  match(C_THEN);
  if (t != NULL)
    t->child[1] = stmt_sequence();
  if (Token == C_ELSE) {
    match(C_ELSE);
    if (t != NULL)
      t->child[2] = stmt_sequence();
  }
  match(C_END);
  return t;
}
/*
repeat-stmt 		?	repeat stmt-sequence until exp
*/
TreeNode *repeat_stmt(void) {
  TreeNode *t = newStmtNode(RepeatK);
  match(C_REPEAT);
  if (t != NULL)
    t->child[0] = stmt_sequence();
  match(C_UNTIL);
  if (t != NULL)
    t->child[1] = exp();
  return t;
}
/*
assign-stmt 		?	identifier := exp | achar;
*/
TreeNode *assign_stmt(void) {
  TreeNode *t = newStmtNode(AssignK);
  if ((t != NULL) && (Token == C_ID)) {
    t->attr.name = copyString(tokenString);
    match(C_ID);
  } //////////////////////////////////////////////////////////////改到这，明天中午再来

  match(C_ASSIGN);
  if (t != NULL) {
    if (Token == C_CHARS)
  }
  t->child[0] = exp();
  return t;
}

TreeNode *read_stmt(void) {
  TreeNode *t = newStmtNode(ReadK);
  match(C_READ);
  if ((t != NULL) && (Token == C_ID))
    t->attr.name = copyString(tokenString);
  match(C_ID);
  return t;
}

TreeNode *write_stmt(void) {
  TreeNode *t = newStmtNode(WriteK);
  match(C_WRITE);
  if (t != NULL)
    t->child[0] = exp();
  return t;
}

TreeNode *exp(void) {
  TreeNode *t = simple_exp();
  if ((Token == C_LT) || (Token == C_EQ)) {
    TreeNode *p = newExpNode(OpK);
    if (p != NULL) {
      p->child[0] = t;
      p->attr.op = token;
      t = p;
    }
    match(token);
    if (t != NULL)
      t->child[1] = simple_exp();
  }
  return t;
}

TreeNode *simple_exp(void) {
  TreeNode *t = term();
  while ((Token == C_PLUS) || (Token == C_MINUS)) {
    TreeNode *p = newExpNode(OpK);
    if (p != NULL) {
      p->child[0] = t;
      p->attr.op = token;
      t = p;
      match(token);
      t->child[1] = term();
    }
  }
  return t;
}

TreeNode *term(void) {
  TreeNode *t = factor();
  while ((Token == C_TIMES) || (Token == C_OVER)) {
    TreeNode *p = newExpNode(OpK);
    if (p != NULL) {
      p->child[0] = t;
      p->attr.op = token;
      t = p;
      match(token);
      p->child[1] = factor();
    }
  }
  return t;
}

TreeNode *factor(void) {
  TreeNode *t = NULL;
  switch (Token) {
  case C_NUM:
    t = newExpNode(ConstK);
    if ((t != NULL) && (Token == C_NUM))
      t->attr.val = atoi(tokenString);
    match(C_NUM);
    break;
  case C_ID:
    t = newExpNode(IdK);
    if ((t != NULL) && (Token == C_ID))
      t->attr.name = copyString(tokenString);
    match(C_ID);
    break;
  case C_LPAREN:
    match(C_LPAREN);
    t = exp();
    match(C_RPAREN);
    break;
  default:
    syntaxError("unexpected token -> ");
    printToken(Token, tokenString);
    Token = getToken();
    break;
  }
  return t;
}

/****************************************/
/* the primary function of the parser   */
/****************************************/
/* Function parse returns the newly
 * constructed syntax tree
 */
TreeNode *parse(void) {
  TreeNode *t;
  Token = getToken();
  t = program();
  if (Token != C_ENDFILE)
    syntaxError("Code ends before file\n");
  return t;
}