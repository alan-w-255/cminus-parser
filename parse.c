/****************************************************/
/* File: parse.c                                    */
/* The parser implementation for the TINY compiler  */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include "parse.h"
#include "globals.h"
#include "scan.h"
#include "util.h"
#include <stdarg.h>

static TokenNode *token; /* holds current token */

// todo: 更改为c-minus的递归调用
static TreeNode *program(void);
static TreeNode *Declaration_List(void);
static TreeNode *Declaration(void);
static TreeNode *localDeclarations(void);
static TreeNode *var_declaration(void);
static TreeNode *fun_declaration(void);
static TreeNode *type_specifier(void);
static TreeNode *params(void);
static TreeNode *param(void);
static TreeNode *compound_stmt(void);
static TreeNode *statement_list(void);
static TreeNode *statement(void);
static TreeNode *selection_stmt(void);
static TreeNode *iteration_stmt(void);
static TreeNode *return_stmt(void);
static TreeNode *additive_exp(void);
static TreeNode *term(void);
static TreeNode *factor(void);
static TreeNode *args(void);
static TreeNode *expression_stmt(void);
static TreeNode *expression(void);
static TreeNode *simple_exp(void);
static void match(TokenType);
static void unmatch();
static void syntaxError(char *);
static void assignName(char *destination, char *source);
static TypeSpecifier tokenTypetoTypeSpecifier(TokenType);
static void promissType(int num, ...);

void promissType(int num, ...)
{
  va_list varlist;
  va_start(varlist, num);
  int i;
  for (i = 0; i < num; i++)
  {
    if (token->type == va_arg(varlist, TokenType))
    {
      va_end(varlist);
      return;
    }
  }

  va_end(varlist);
  char *errMsg = (char *)malloc(sizeof(char) * 100);
  sscanf(errMsg, "unknonw token '%s'", tokenString);
  syntaxError(errMsg);
}

TypeSpecifier tokenTypetoTypeSpecifier(TokenType tt)
{
  if (tt == INT)
  {
    return Integer;
  }
  else if (tt == VOID)
  {
    return Void;
  }
  else
  {
    perror("unable to convert this TokenType to TypeSpecifier!");
  }
}

TreeNode *program()
{
  TreeNode *t = Declaration_List();
  return t;
}

TreeNode *Declaration_List()
{
  TreeNode *t = Declaration();
  TreeNode *p = t;
  while (token->type != ENDFILE)
  {
    p->sibling = Declaration();
    if (p->sibling == NULL)
    {
      break;
    }
    p = p->sibling;
  }
  return t;
}

TreeNode *Declaration()
{
  TreeNode *tr = NULL;
  promissType(2, INT, VOID);
  match(token->type);
  match(ID);
  if (token->type == LPAREN)
  {
    unmatch();
    unmatch();
    tr = fun_declaration();
  }
  else
  {
    unmatch();
    unmatch();
    tr = var_declaration();
  }
  return tr;
}

TreeNode *var_declaration()
{
  TreeNode *tr = NULL;
  promissType(2, INT, VOID);

  TypeSpecifier ts = tokenTypetoTypeSpecifier(token->type);
  match(token->type);
  char *idName = token->tokenString;
  match(ID);

  if (token->type == LBRACKET)
  {
    match(LBRACKET);
    char *len_str = token->tokenString;
    match(NUM);
    int len = atoi(len_str);
    tr = newDclrNode(VarArrK, ts, idName, len, NULL, NULL, token->lineno);
    match(RBRACKET);
  }
  else
  {
    tr = newDclrNode(VarK, ts, idName, 0, NULL, NULL, token->lineno);
  }
  match(SEMI);

  return tr;
}

TreeNode *fun_declaration()
{
  TreeNode *tr;
  promissType(2, INT, VOID);
  TypeSpecifier ts = tokenTypetoTypeSpecifier(token->type);
  match(token->type);
  char *idName = token->tokenString;
  match(ID);
  match(LPAREN);
  TreeNode *_params = params();
  match(RPAREN);
  TreeNode *_compound_stmt = compound_stmt();
  tr = newDclrNode(FunK, ts, idName, 0, _params, _compound_stmt, token->lineno);
  return tr;
}

TreeNode *params()
{
  TreeNode *tr = NULL;
  promissType(2, INT, VOID);
  TypeSpecifier idType;
  idType = tokenTypetoTypeSpecifier(token->type);

  match(token->type); // match then type specifier
  if (token->type != ID)
  {
    // 无参数
    unmatch();
    match(VOID);
    tr = newDclrNode(VarK, Void, NULL, 0, NULL, NULL, token->lineno);
  }
  else
  {
    // 有参数
    unmatch(); // unmatch then type specifier
    TreeNode *p = param();
    tr = p;
    while (token->type == COMMA)
    {
      match(COMMA);
      TreeNode *q = param();
      if (q != NULL)
      {
        p->sibling = q;
        p = q;
      }
    }
  }
  return tr;
}

TreeNode *param()
{
  TreeNode *tr = NULL;
  promissType(2, INT, VOID);
  TypeSpecifier ts = tokenTypetoTypeSpecifier(token->type);
  match(token->type);
  char *idName = token->tokenString;
  match(ID);
  if (token->type == LBRACKET)
  {
    match(LBRACKET);
    tr = newDclrNode(VarArrK, ts, idName, 0, NULL, NULL, token->lineno);
    match(RBRACKET);
  }
  else
  {
    tr = newDclrNode(VarK, ts, idName, 0, NULL, NULL, token->lineno);
  }
  return tr;
}

TreeNode *compound_stmt()
{
  TreeNode *cs = newStmtNode(CompoundK, token->lineno);
  match(LBRACE);
  if (token->type == INT || token->type == VOID)
  {
    cs->child[0] = localDeclarations();
  }
  cs->child[1] = statement_list();
  match(RBRACE);
  return cs;
}

TreeNode *localDeclarations()
{
  TreeNode *tr;
  promissType(2, INT, VOID);
  TreeNode *p = var_declaration();
  tr = p;
  while (token->type == INT || token->type == VOID)
  {
    TreeNode *q = var_declaration();
    if (q != NULL)
    {
      p->sibling = q;
      p = q;
    }
  }
  return tr;
}

TreeNode *statement_list()
{
  TreeNode *tr = NULL;
  TreeNode *p = NULL;
  if (token->type == LBRACE || token->type == IF || token->type == WHILE || token->type == RETURN || token->type == ID || token->type == LPAREN || token->type == NUM)
  {
    tr = statement();
    p = tr;
  }
  while (token->type == LBRACE || token->type == IF || token->type == WHILE || token->type == RETURN || token->type == ID || token->type == LPAREN || token->type == NUM)
  {
    TreeNode *q = statement();
    p->sibling = q;
    p = q;
  }
  return tr;
}

TreeNode *statement()
{
  TreeNode *tr = NULL;
  TokenType tt = token->type;
  switch (tt)
  {
  case LBRACE:
    tr = compound_stmt();
    break;
  case IF:
    tr = selection_stmt();
    break;
  case WHILE:
    tr = iteration_stmt();
    break;
  case RETURN:
    tr = return_stmt();
    break;
  default:
    tr = expression_stmt();
  }
  return tr;
}

TreeNode *selection_stmt()
{
  TreeNode *tr = newStmtNode(SelectionK, token->lineno);
  match(IF);
  match(LPAREN);
  tr->child[0] = expression();
  match(RPAREN);
  tr->child[1] = statement();
  if (token->type == ELSE)
  {
    match(ELSE);
    tr->child[2] = statement();
  }
  return tr;
}

TreeNode *iteration_stmt()
{
  TreeNode *tr = newStmtNode(IterationK, token->lineno);
  match(WHILE);
  match(LPAREN);
  tr->child[0] = expression();
  match(RPAREN);
  tr->child[1] = statement();
  return tr;
}

TreeNode *return_stmt()
{
  TreeNode *tr = newStmtNode(ReturnK, token->lineno);
  match(RETURN);
  tr->child[0] = expression();
  match(SEMI);
  return tr;
}

TreeNode *expression_stmt()
{
  TreeNode *tr = expression();
  match(SEMI);
  return tr;
}

TreeNode *expression()
{
  TreeNode *tr = NULL;
  if (token->type == ID)
  {
    char *idName = token->tokenString;
    TokenNode *backpoint = token;
    match(ID);
    if (token->type == ASSIGN) // ID赋值语句
    {
      tr = newStmtNode(ASSIGNK, token->lineno);
      tr->child[0] = newExpNode(IdK, Integer, token->lineno);
      tr->child[0]->attr.name = idName;
      match(ASSIGN);
      tr->child[1] = expression();
    }
    else
    {
      if (token->type == LBRACKET) // 数组
      {
        TreeNode *tp = newExpNode(IdArrK, Integer, token->lineno);
        tp->attr.name = idName;
        match(LBRACKET);
        TreeNode *tt = expression();
        tp->child[0] = tt;
        match(RBRACKET);
        if (token->type == ASSIGN) // 数组下标赋值语句
        {
          tr = newStmtNode(ASSIGNK, token->lineno);
          tr->child[0] = tp;
          match(ASSIGN);
          tr->child[1] = expression();
          return tr;
        }
        else
        {
          token = backpoint;
          tr = simple_exp();
          return tr;
        }
      }
      else // simple expression
      {
        unmatch();
        tr = simple_exp();
        return tr;
      }
    }
  }
  else
  {
    tr = simple_exp();
    return tr;
  }
}

TreeNode *simple_exp()
{
  TreeNode *tr = additive_exp();
  while (token->type == LE || token->type == LT || token->type == GT || token->type == GE ||
         token->type == EQ || token->type == NE)
  {
    TreeNode *p = newExpNode(OpK, Integer, token->lineno);
    if (p != NULL)
    {
      p->child[0] = tr;
      p->attr.op = token->type;
      match(token->type);
      p->child[1] = additive_exp();
      tr = p;
    }
  }
  return tr;
}

TreeNode *additive_exp()
{
  TreeNode *tr = term();
  while (token->type == PLUS || token->type == SUB)
  {
    TreeNode *p = newExpNode(OpK, Integer, token->lineno);
    if (p != NULL)
    {
      p->child[0] = tr;
      p->attr.op = token->type;
      tr = p;
      match(token->type);
      p->child[1] = term();
    }
  }
  return tr;
}

TreeNode *term()
{
  TreeNode *t = factor();
  while (token->type == MUL || token->type == DIV)
  {
    TreeNode *p = newExpNode(OpK, Integer, token->lineno);
    if (p != NULL)
    {
      p->child[0] = t;
      p->attr.op = token->type;
      t = p;
      match(token->type);
      p->child[1] = factor();
    }
  }
  return t;
}

TreeNode *factor()
{
  TreeNode *t = NULL;
  char *idName = token->tokenString;
  char *errMsg;
  switch (token->type)
  {
  case LPAREN:
    match(LPAREN);
    t = expression();
    match(RPAREN);
    break;
  case NUM:
    t = newExpNode(ConstK, Integer, token->lineno);
    t->attr.val = atoi(token->tokenString);
    match(NUM);
    break;
  case ID:
    match(ID);
    if (token->type == LBRACKET)
    { //Id [expression]
      match(LBRACKET);
      t = newExpNode(IdArrK, Integer, token->lineno);
      t->child[0] = expression();
      t->attr.name = idName;
      match(RBRACKET);
    }
    else if (token->type == LPAREN)
    { // call
      match(LPAREN);
      t = newExpNode(CallK, Integer, token->lineno);
      t->attr.name = idName;
      t->child[0] = args();
      match(RPAREN);
    }
    else
    { // ID
      t = newExpNode(IdK, Integer, token->lineno);
      t->attr.name = idName;
    }
    break;
  default:
    // error
    sscanf(errMsg, "unknonw token %s", token->tokenString);
    syntaxError(errMsg);
    break;
  }
  return t;
}

TreeNode *args()
{
  TreeNode *tr = NULL;
  if (token->type == RPAREN)
  { // empty arg
  }
  else
  {
    TreeNode *p = expression();
    tr = p;
    while (token->type == COMMA)
    {
      match(COMMA);
      TreeNode *q = expression();
      p->sibling = q;
      p = q;
    }
  }
  return tr;
}

static void syntaxError(char *message)
{
  fprintf(listing, "\n>>> ");
  fprintf(listing, "Syntax error at line %d: %s", token->lineno, message);
  Error = TRUE;
}

static void match(TokenType expected)
{
  if (token->type == expected && token->next != NULL)
    token = token->next;
  else
  {
    syntaxError("unexpected token -> ");
    printToken(token->type, tokenString);
    fprintf(listing, "      ");
  }
}

static void unmatch()
{
  if (token->pre->pre != NULL)
  {
    token = token->pre;
  }
}

static void assignName(char *destination, char *source)
{
  destination = (char *)malloc(sizeof(char) * (MAXTOKENLEN + 1));
  strcpy(destination, source);
}

/****************************************/
/* the primary function of the parser   */
/****************************************/
/* Function parse returns the newly
 * constructed syntax tree
 */
TreeNode *parse(void)
{
  TreeNode *t;
  token = TokenTable->next;
  t = program();
  if (token->type != ENDFILE)
  {
    syntaxError("Code ends before file\n");
  }
  return t;
}
