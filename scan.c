/****************************************************/
/* File: scan.c                                     */
/* The scanner implementation for the TINY compiler */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include "scan.h"
#include "globals.h"
#include "util.h"


/* states in scanner DFA */
// TODO: 要添加一些状态 !done
typedef enum {
  START,
  INEQUAL,
  INGT,
  INLT,
  INEXCLA,
  INCOMMENTORDIV,
  OUTCOMMENT,
  INCOMMENT,
  INNUM,
  INID,
  DONE
} StateType;

TokenNode *TokenTable;

/* lexeme of identifier or reserved word */
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
   exhausted */
static int getNextChar(void) {
  if (!(linepos < bufsize)) {
    lineno++;
    if (fgets(lineBuf, BUFLEN - 1, source)) {
      if (EchoSource)
        fprintf(listing, "%4d: %s", lineno, lineBuf);
      bufsize = strlen(lineBuf);
      linepos = 0;
      return lineBuf[linepos++];
    } else {
      EOF_flag = TRUE;
      return EOF;
    }
  } else
    return lineBuf[linepos++];
}

/* ungetNextChar backtracks one character
   in lineBuf */
static void ungetNextChar(void) {
  if (!EOF_flag)
    linepos--;
}

/* lookup table of reserved words */
// TODO: need to remove and add some reserved words !done
static struct {
  char *str;
  TokenType tok;
} reservedWords[MAXRESERVED] = {{"if", IF},         {"int", INT},
                                {"else", ELSE},     {"void", VOID},
                                {"return", RETURN}, {"while", WHILE}};

/* lookup an identifier to see if it is a reserved word */
/* uses linear search */

// 查找是哪一种保留字, 或者返回为标识符.
static TokenType reservedLookup(char *s) {
  int i;
  for (i = 0; i < MAXRESERVED; i++)
    if (!strcmp(s, reservedWords[i].str))
      return reservedWords[i].tok;
  return ID;
}

/****************************************/
/* the primary function of the scanner  */
/****************************************/
/* function getToken returns the
 * next token in source file
 */
TokenType getToken(void) {
  /* index for storing into tokenString */
  int tokenStringIndex = 0;
  /* holds current token to be returned */
  TokenType currentToken;
  /* current state - always begins at START */
  StateType state = START;
  /* flag to indicate save to tokenString */
  int save;
  // 双层case嵌套
  while (state != DONE) {
    int c = getNextChar();
    save = TRUE;
    switch (state) {
    case START:
      if (isdigit(c))
        state = INNUM;
      else if (isalpha(c))
        state = INID;
      else if (c == '=')
        state = INEQUAL;
      else if ((c == ' ') || (c == '\t') || (c == '\n'))
        save = FALSE;
      else if (c == '/') {
        save = FALSE;
        state = INCOMMENTORDIV;
      } else if (c == '>') {
        state = INGT;
      } else if (c == '<') {
        state = INLT;
      } else if (c == '!') {
        state = INEXCLA;
      } else {
        state = DONE;
        switch (c) {
        case EOF:
          save = FALSE;
          currentToken = ENDFILE;
          break;
        case '+':
          currentToken = PLUS;
          break;
        case '-':
          currentToken = SUB;
          break;
        case '*':
          currentToken = MUL;
          break;
        case '(':
          currentToken = LPAREN;
          break;
        case ')':
          currentToken = RPAREN;
          break;
        case ';':
          currentToken = SEMI;
          break;
        case '[':
          currentToken = LBRACKET;
          break;
        case ']':
          currentToken = RBRACKET;
          break;
        case '{':
          currentToken = LBRACE;
          break;
        case '}':
          currentToken = RBRACE;
          break;
        case ',':
          currentToken = COMMA;
          break;
        default:
          currentToken = ERROR;
          break;
        }
      }
      break;
    case INCOMMENTORDIV: // 注释
      save = FALSE;
      if (c == '*') {
        state = INCOMMENT;
      } else {
        ungetNextChar();
        state = DIV;
      }
      break;
    case INCOMMENT:
      save = FALSE;
      if (c == '*') {
        state = OUTCOMMENT;
      } else if (c == EOF) {
        state = DONE;
        currentToken = ERRORENDFILE;
      }
      break;
    case OUTCOMMENT:
      save = FALSE;
      if (c == '/') {
        state = START;
      } else if (c == EOF) {
        state = DONE;
        currentToken = ERRORENDFILE;
      } else {
        state = INCOMMENT;
      }
      break;
    case INEQUAL: // 赋值或相等
      state = DONE;
      if (c == '=')
        currentToken = EQ;
      else { /* backup in the input */
        ungetNextChar();
        currentToken = ASSIGN;
      }
      break;
    case INNUM:          // 数字
      if (!isdigit(c)) { /* backup in the input */
        ungetNextChar();
        save = FALSE;
        state = DONE;
        currentToken = NUM;
      }
      break;
    case INID:           // 标识符
      if (!isalpha(c)) { /* backup in the input */
        ungetNextChar();
        save = FALSE;
        state = DONE;
        currentToken = ID;
      }
      break;
    case INGT:
      state = DONE;
      if (c == '=') {
        save = TRUE;
        currentToken = GE;
      } else {
        ungetNextChar();
        currentToken = GT;
      }
      break;
    case INLT:
      state = DONE;
      if (c == '=') {
        save = TRUE;
        currentToken = LE;
      } else {
        ungetNextChar();
        currentToken = LT;
      }
      break;
    case INEXCLA:
      state = DONE;
      if (c == '=') {
        save = TRUE;
        currentToken = NE;
      } else {
        ungetNextChar();
        save = TRUE;
        currentToken = ERROR;
      }
      break;
    case DONE:
    default: /* should never happen */
      fprintf(listing, "Scanner Bug: state= %d\n", state);
      state = DONE;
      currentToken = ERROR;
      break;
    }

    if ((save) && (tokenStringIndex <= MAXTOKENLEN))
      tokenString[tokenStringIndex++] = (char)c;
    if (state == DONE) {
      tokenString[tokenStringIndex] = '\0';
      if (currentToken == ID)
        // 查看是否为保留字
        currentToken = reservedLookup(tokenString);
    }
  }
  if (TraceScan) {
    fprintf(listing, "\t%d: ", lineno);
    printToken(currentToken, tokenString);
  }
  return currentToken;
} /* end getToken */

void scan(void) {
  TokenType tok;
  tok = getToken();
  TokenNode *t = malloc(sizeof(TokenNode));
  TokenTable = t;
  t->lineno = -1;
  t->tokenString = "";
  t->type = -1;
  t->next = NULL;
  TokenTable->pre = NULL;
  while (tok != ENDFILE) {
    char *tokstr = (char *)malloc(sizeof(char) * (MAXTOKENLEN + 1));
    TokenNode *tn = malloc(sizeof(TokenNode));
    tn->type = tok;
    strcpy(tokstr, tokenString);
    tn->tokenString = tokstr;
    tn->lineno = lineno;
    tn->next = NULL;
    t->next = tn;
    tn->pre = t;
    t = tn;
    tok = getToken();
  }
  char *tokstr = (char *)malloc(sizeof(char) * (MAXTOKENLEN + 1));
  TokenNode *tn = malloc(sizeof(TokenNode));
  tn->type = tok;
  strcpy(tokstr, tokenString);
  tn->tokenString = tokstr;
  tn->lineno = lineno;
  tn->next = NULL;
  t->next = tn;
  tn->pre = t;
  t = tn;
}

void destroyTokenTable() {
  TokenNode *t1 = TokenTable;
  TokenNode *t2 = TokenTable->next;
  while (t2 != NULL) {
    free(t1);
    t1 = t2;
    t2 = t2->next;
  }
  free(t1);
}