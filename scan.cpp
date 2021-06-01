#include "globals.h"
#include "scan.h"
#include "util.h"

/* states in scanner DFA */
typedef enum {
  S_START,
  S_INASSIGN,
  S_INCOMMENT,
  S_INNUM,
  S_INID,
  S_INLT,
  S_INHT,
  S_IEXF,
  S_OEXF,
  S_NEQ,
  S_DONE,
  S_EQ,
  S_UNCHAR,
} StateType;

/* lexeme of identifier or reserved word 标识符或保留字的词素 */
char tokenString[MAXTOKENLEN + 1];

/* BUFLEN = length of the input buffer for
   source code lines */
#define BUFLEN 256

static char lineBuf[BUFLEN]; /* holds the current line */
static int linepos = 0;      /* current position in LineBuf */
static int bufsize = 0;      /* current size of buffer string */
static int EOF_flag = true;  /* corrects ungetNextChar behavior on EOF */

//该函数将一个256字符缓冲区内部的lineBuf中的字符取到扫描程序中
//如果耗尽该缓冲区，且假设每一次都获取了一个新的源代码行（以及增加的lineno），那么就可以利用fgets从sourse更新缓冲区
static int getNextChar(void) {
  if (!(linepos < bufsize)) {
    lineno++;
    if (fgets(lineBuf, BUFLEN - 1, source)) {
      // fets从指定流source读取一行并存入lineBuf指向的字符串内
      if (EchoSource)
        fprintf(listing, "%4d: %s", lineno, lineBuf);
      bufsize = strlen(lineBuf);
      linepos = 0;
      return lineBuf[linepos++];
    } else //不能读取，说明第一遍扫描结束
    {
      EOF_flag = true;
      return EOF;
    }
  } else
    return lineBuf[linepos++];
}

// ungetNextChar backtracks one character 回溯一个字符
static void ungetNextChar(void) {
  if (!EOF_flag)
    linepos--;
}

/* lookup table of reserved words */
static struct {
  char const *str;
  TokenType tok;
} reservedWords[MAXRESERVED] = {{"if", C_IF},       {"then", C_THEN},
                                {"int", C_INT},     {"end", C_END},
                                {"until", C_UNTIL}, {"char", C_CHAR},
                                {"else", C_ELSE},   {"repeat", C_REPEAT},
                                {"read", C_READ},   {"write", C_WRITE}};

/* lookup an identifier to see if it is a reserved word
 * 检查标识符是否为保留字 uses linear search
 * 表reservedWords和该函数完成由getToken的主要循环识别的标识符知乎的保留字的查找
 */
static TokenType reservedLookup(char *s) {
  int i;
  for (i = 0; i < MAXRESERVED; i++)
    if (!strcmp(s, reservedWords[i].str))
      return reservedWords[i].tok;
  return C_ID;
}

/****************************************/
/* the primary function of the scanner  */
/****************************************/
/* function getToken returns the
 * next token in source file
 * 消耗输入字符并根据DFA返回下一个被识别的记号
 */
TokenType getToken(void) { /* index for storing into tokenString
                              用于存储到tokenstring的索引*/
  int tokenStringIndex = 0;
  /* holds current token to be returned */
  TokenType currentToken;
  /* current state - always begins at START */
  StateType state = S_START;
  /* flag to indicate save to tokenString 指示是否保存到tokenString的标志*/
  int save;
  while (state != S_DONE) //根据DFA扫描
  {
    int c = getNextChar(); //扫描程序的字符输入
    save = true;
    switch (state) {
    case S_START:
      if (isdigit(c)) //如果为阿拉伯数字，返回非0
        state = S_INNUM;
      else if (isalpha(c)) //如果为字母，返回非0
        state = S_INID;
      else if (c == '=')
        state = S_EQ;
      else if ((c == ' ') || (c == '\t') || (c == '\n')) {
        save = false;
        state = S_START;
      } else if (c == '<')
        state = S_INLT;
      else if (c == '>')
        state = S_INHT;
      else if (c == '!')
        state = S_NEQ;
      else if (c == '/')
        state = S_IEXF;
      else if (c == ':')
        state = S_INASSIGN;
      else if (c == '"') {
        state = S_UNCHAR;
        save = false;
      } else //单目标识符或首字符唯一的标识符
      {
        state = S_DONE;
        switch (c) {
        case EOF:
          save = false;
          currentToken = C_ENDFILE;
          break;
        case '+':
          currentToken = C_PLUS;
          break;
        case '-':
          currentToken = C_MINUS;
          break;
        case '*':
          currentToken = C_TIMES;
          break;
        case '%':
          currentToken = C_MOD;
          break;
        case '(':
          currentToken = C_LPAREN;
          break;
        case ')':
          currentToken = C_RPAREN;
          break;
        case ';':
          currentToken = C_SEMI;
          break;
        case ',': ///
          currentToken = C_COM;
          break;
        default:
          currentToken = C_ERROR;
          break;
        }
      }
      break;

    case S_INLT: /* < or <= */
      state = S_DONE;
      currentToken = C_LT;
      if (c == '=') {
        currentToken = C_NGT;
      } else {
        ungetNextChar();
        save = false;
      }
      break;

    case S_INHT: /* > or >= */
      state = S_DONE;
      currentToken = C_GT;
      if (c == '=') {
        currentToken = C_NLT;
      } else {
        ungetNextChar();
        save = false;
      }
      break;

    case S_NEQ:
      if (c == '=')
        currentToken = C_NEQ;
      else {
        currentToken = C_ERROR;
        ungetNextChar();
        save = false;
      }
      break;

    case S_EQ:
      if (c == '=')
        currentToken = C_EQ;
      else {
        currentToken = C_ERROR;
        ungetNextChar();
        save = false;
      };
      break;

    case S_INASSIGN:
      state = S_DONE;
      if (c == '=')
        currentToken = C_ASSIGN;
      else { /* backup in the input 在输入中备份*/
        currentToken = C_ERROR;
        ungetNextChar(); // tiny中数和标识符的识别要求从INNUM和INID到最终状态的转换都应该是非消耗的
        save =
            false; //可提供ungetNextChar过程，在输入缓冲区中回溯一个字符来完成任务
      }

    case S_INNUM:
      if (!isdigit(c)) { /* backup in the input */
        ungetNextChar();
        save = false;
        state = S_DONE;
        currentToken = C_NUM;
      }
      break;

    case S_INID:
      if (!isalpha(c)) { /* backup in the input */
        ungetNextChar();
        save = false;
        state = S_DONE;
        currentToken = C_ID;
      }
      break;

    case S_IEXF:
      state = S_DONE;
      currentToken = C_DIV;
      if (c == '*') {
        state = S_INCOMMENT;
        save = false;
      } else {
        ungetNextChar();
        save = false;
      }
      break;

    case S_INCOMMENT:
      save = false;
      if (c == '*') {
        state = S_OEXF;
      } else
        state = S_INCOMMENT;
      break;

    case S_OEXF:
      save = false;
      if (c == '/') {
        state = S_START;
      } else
        state = S_INCOMMENT;
      break;

    case S_UNCHAR:
      if (c == '"') {
        save = false;
        state = S_DONE;
        currentToken = C_CHARS;
      } else if (isalpha(c) || isdigit(c)) {
        state = S_UNCHAR;
      } else {
        save = false;
        currentToken = C_ERROR;
      }
    case S_DONE:
    default: /* should never happen */
      fprintf(listing, "Scanner Bug: state= %d\n", state);
      state = S_DONE;
      currentToken = C_ERROR;
      break;
    }
    if ((save) && (tokenStringIndex <=
                   MAXTOKENLEN)) //如果可填入且长度符号要求，则填入tokenStirng中
      tokenString[tokenStringIndex++] = (char)c;
    if (state == S_DONE) //如果到达DONE操作，则存储
    {
      tokenString[tokenStringIndex] = '\0';
      if (currentToken == C_ID) //检查保留字
        currentToken = reservedLookup(tokenString);
    }
  }
  if (TraceScan) //当token均被识别，可以将信息打印
  {
    fprintf(listing, "\t%d: ", lineno);
    printToken(currentToken, tokenString);
  }
  return currentToken;
} /* end getToken */