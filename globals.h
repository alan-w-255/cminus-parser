/****************************************************/
/* File: globals.h                                  */
/* Global types and vars for TINY compiler          */
/* must come before other include files             */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include <ctype.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

/* MAXRESERVED = the number of reserved words */
// TODO: 把 maxreserved 改为c-的保留字个数.
#define MAXRESERVED 6

#define MAXTOKENLEN 40

// TODO: 修改下面的 reserved words and special symbols
typedef enum
/* book-keeping tokens */
{
  ENDFILE,
  ERROR,
  ERRORENDFILE,
  /* reserved words */
  IF,
  ELSE,
  INT,
  RETURN,
  VOID,
  WHILE,
  /* multicharacter tokens */
  ID,
  NUM,
  /* special symbols */
  PLUS,
  SUB,
  MUL,
  DIV,
  LT,
  GT,
  NE,
  ASSIGN,
  SEMI,
  COMMA,
  LPAREN,
  RPAREN,
  LBRACKET,
  RBRACKET,
  LBRACE,
  RBRACE,
  /* two characters special symbols */
  LE,
  GE,
  EQ,
} TokenType;

typedef struct tokenNode {
  TokenType type;
  int lineno;
  char* tokenString;
  struct tokenNode* next;
  struct tokenNode* pre;
} TokenNode;

extern TokenNode *TokenTable;

extern FILE *source;  /* source code text file */
extern FILE *listing; /* listing output text file */
extern FILE *code;    /* code text file for TM simulator */


extern int lineno; /* source line number for listing */

// todo: modify the data struct to fit c-minus
/**************************************************/
/***********   Syntax tree for parsing ************/
/**************************************************/

typedef enum
{
  DclrK,// 声明类型
  StmtK,
  ExpK
} NodeKind;

typedef enum {
  VarK,
  VarArrK,
  FunK
} DclrKind;

typedef enum
{
  ASSIGNK,
  CompoundK,
  SelectionK,
  IterationK,
  ReturnK
} StmtKind;

typedef enum
{
  OpK,
  ConstK,
  IdArrK,
  CallK,
  IdK
} ExpKind;

/* TypeSpecifier is used for type checking */
typedef enum
{
  Void,
  Integer,
} TypeSpecifier;


#define MAXCHILDREN 3

typedef struct array{
  char name[MAXTOKENLEN + 1];
  unsigned int len;
} Array;

typedef struct treeNode
{
  struct treeNode *child[MAXCHILDREN];
  struct treeNode *sibling;
  int lineno;
  NodeKind nodekind;
  union {
    StmtKind stmt;// 语句
    ExpKind exp;// 表达式
    DclrKind dclr;
  } kind;
  union {
    TokenType op;// 如果是操作节点
    int val; // 如果是常数节点
    char *name;// 如果是id 或者函数节点
    Array* arr;// 如果是数组节点
  } attr;
  int index;

  TypeSpecifier type; /* for type checking of exps */
} TreeNode;

/**************************************************/
/***********   Flags for tracing       ************/
/**************************************************/

/* EchoSource = TRUE causes the source program to
 * be echoed to the listing file with line numbers
 * during parsing
 */
extern int EchoSource;

/* TraceScan = TRUE causes token information to be
 * printed to the listing file as each token is
 * recognized by the scanner
 */
extern int TraceScan;

/* TraceParse = TRUE causes the syntax tree to be
 * printed to the listing file in linearized form
 * (using indents for children)
 */
extern int TraceParse;

/* TraceAnalyze = TRUE causes symbol table inserts
 * and lookups to be reported to the listing file
 */
extern int TraceAnalyze;

/* TraceCode = TRUE causes comments to be written
 * to the TM code file as code is generated
 */
extern int TraceCode;

/* Error = TRUE prevents further passes if an error occurs */
extern int Error;
#endif
