/****************************************************/
/* File: scan.c                                     */
/* The scanner implementation for the TINY compiler */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include "globals.h"
#include "util.h"
#include "scan.h"

/* states in scanner DFA */
typedef enum
{
  S_START,
  S_INASSIGN,
  S_INCOMMENT,
  S_INNUM,
  S_INID,
  S_INLT,
  S_INHT,
  S_IEXF,
  S_IEX,
  S_OEXF,
  S_NEQ,
  S_DONE
} StateType;

/* lexeme of identifier or reserved word ��ʶ�������ֵĴ��� */
char tokenString[MAXTOKENLEN + 1];

/* BUFLEN = length of the input buffer for
   source code lines */
#define BUFLEN 256

static char lineBuf[BUFLEN]; /* holds the current line */
static int linepos = 0;      /* current position in LineBuf */
static int bufsize = 0;      /* current size of buffer string */
static int EOF_flag = FALSE; /* corrects ungetNextChar behavior on EOF */

/* getNextChar fetches the next non-blank character
  from lineBuf, reading in a new line if lineBuf is
  exhausted 
  �ú�����һ��256-�ַ��������ڲ���lineBuf�е��ַ�ȡ��ɨ�������
  ����ľ��û��������Ҽ���ÿһ�ζ���ȡ��һ���µ�Դ�����У��Լ����ӵ�lineno������ô�Ϳ�������fgets��sourse���»����� */
static int getNextChar(void)
{
  if (!(linepos < bufsize))
  {
    lineno++;
    if (fgets(lineBuf, BUFLEN - 1, source))//fets��ָ����source��ȡһ�в�����lineBufָ����ַ�����
    {
      if (EchoSource)
        fprintf(listing, "%4d: %s", lineno, lineBuf);
      bufsize = strlen(lineBuf);
      linepos = 0;
      return lineBuf[linepos++];
    }
    else//���ܶ�ȡ��˵����һ��ɨ�����
    {
      EOF_flag = TRUE;
      return EOF;
    }
  }
  else
    return lineBuf[linepos++];
}

/* ungetNextChar backtracks one character ����һ���ַ�
   in lineBuf */
static void ungetNextChar(void)
{
  if (!EOF_flag)
    linepos--;
}

/* lookup table of reserved words */
static struct
{
  char *str;
  TokenType tok;
} reservedWords[MAXRESERVED] = {{"if", C_IF},{"int", C_INT},{"void", C_VOID},
{"else", C_ELSE},{"return", C_RETURN},{"while", C_WHILE},
{"read", C_READ},{"write", C_WRITE},/*{"random", C_RANDOM},*/ };

/* lookup an identifier to see if it is a reserved word ����ʶ���Ƿ�Ϊ������*/
/* uses linear search ��reservedWords�͸ú��������getToken����Ҫѭ��ʶ��ı�ʶ��֪���ı����ֵĲ���*/
static TokenType reservedLookup(char *s)
{
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
 * ���������ַ�������DFA������һ����ʶ��ļǺ�
 */
TokenType getToken(void)
{ /* index for storing into tokenString ���ڴ洢��tokenstring������*/
  int tokenStringIndex = 0;
  /* holds current token to be returned */
  TokenType currentToken;
  /* current state - always begins at START */
  StateType state = S_START;
  /* flag to indicate save to tokenString ָʾ�Ƿ񱣴浽tokenString�ı�־*/
  int save;
  while (state != S_DONE)//����DFAɨ��
  {
    int c = getNextChar();//ɨ�������ַ�����
    save = TRUE;
    switch (state)
    {
    case S_START:
      if (isdigit(c))//���Ϊ���������֣����ط�0
        state = S_INNUM;
      else if (isalpha(c))//���Ϊ��ĸ�����ط�0
        state = S_INID;
      else if (c == '=')
        state = S_INASSIGN;
      else if ((c == ' ') || (c == '\t') || (c == '\n'))
      {
        save = FALSE;
        state = S_START;
      }        
      else if (c == '<')
        state = S_INLT;
      else if (c == '>')
        state = S_INHT;
      else if (c == '!')
        state = C_NEQ;
      else if (c == '/')
        state = S_IEXF;  
      else//��Ŀ��ʶ�������ַ�Ψһ�ı�ʶ��
      {
        state = S_DONE;
        switch (c)
        {
        case EOF:
          save = FALSE;
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
        case '{':
					currentToken = C_LBRACE;
					break;
				case '}':
					currentToken = C_RBRACE;
					break;
        case '[':
					currentToken = C_LSQUARE;
					break;
				case ']':
					currentToken = C_RSQUARE;
					break;
        case ';':
          currentToken = C_SEMI;
          break;
        case ',':
          currentToken = C_COM;
          break;
        default:
          currentToken = C_ERROR;
          break;
        }
      }
      break;

    case S_INLT:/* < or <= */
      state = S_DONE;
      currentToken = C_LT;
      if(c == '=')
      {
        currentToken = C_NGT;
      }
      else
      {
        ungetNextChar();
        save=FALSE;
      }
      break;

    case S_INHT:/* > or >= */
      state = S_DONE;
      currentToken = C_GT;
      if(c == '=')
      {
        currentToken = C_NLT;
      }
      else
      {
        ungetNextChar();
        save=FALSE;
      }
      break;

    case S_NEQ:
      if(c == '=')
        currentToken = C_NEQ;
      else
      {
        currentToken = C_ERROR;
        ungetNextChar();
      }
      break;

    case S_INASSIGN:
      state = S_DONE;
      if (c == '=')
        currentToken = C_EQ;
      else
      { /* backup in the input �������б���*/
        ungetNextChar();//tiny�����ͱ�ʶ����ʶ��Ҫ���INNUM��INID������״̬��ת����Ӧ���Ƿ����ĵ�
        save = FALSE;//���ṩungetNextChar���̣������뻺�����л���һ���ַ����������
      }
      break;


    case S_INNUM:
      if (!isdigit(c))
      { /* backup in the input */
        ungetNextChar();
        save = FALSE;
        state = S_DONE;
        currentToken = C_NUM;
      }
      break;

    case S_INID:
      if (!isalpha(c))
      { /* backup in the input */
        ungetNextChar();
        save = FALSE;
        state = S_DONE;
        currentToken = C_ID;
      }
      break;

    case S_IEXF:
      state = S_DONE;
      currentToken = C_DIV;
      if(c == '*')
      {
        state = S_IEX;
        save = FALSE;////
      }
      else
      {
        ungetNextChar();
        save=FALSE;
      }
      break;

      case S_IEX:
        save = FALSE;
        if(c == '*')
        {
          state = S_OEXF;
        }
        else
          state = S_IEX;
        break;

      case S_OEXF:
        save = FALSE;
        if(c== '/')
        {
          state = S_START;
        }
        else
          state = S_IEX;
        break;



    case S_DONE:
    default: /* should never happen */
      fprintf(listing, "Scanner Bug: state= %d\n", state);
      state = S_DONE;
      currentToken = C_ERROR;
      break;
    }

    
    if ((save) && (tokenStringIndex <= MAXTOKENLEN))//����������ҳ��ȷ���Ҫ��������tokenStirng��
      tokenString[tokenStringIndex++] = (char)c;
    if (state == S_DONE)//�������DONE��������洢
    {
      tokenString[tokenStringIndex] = '\0';
      if (currentToken == C_ID)//��鱣����
        currentToken = reservedLookup(tokenString);
    }
  }
  if (TraceScan)//��token����ʶ�𣬿��Խ���Ϣ��ӡ
  {
    fprintf(listing, "\t%d: ", lineno);
    printToken(currentToken, tokenString);
  }
  return currentToken;
} /* end getToken */
