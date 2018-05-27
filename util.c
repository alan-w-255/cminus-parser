/****************************************************/
/* File: util.c                                     */
/* Utility function implementation                  */
/* for the TINY compiler                            */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include "util.h"
#include "globals.h"

/* Procedure printToken prints a token
 * and its lexeme to the listing file
 */
void printToken(TokenType token, const char *tokenString)
{
  switch (token)
  {
  case IF:
  case ELSE:
  case WHILE:
  case RETURN:
  case VOID:
  case INT:
    fprintf(listing, "reserved word: %s\n", tokenString);
    break;
  case ASSIGN:
    fprintf(listing, "=\n");
    break;
  case LT:
    fprintf(listing, "<\n");
    break;
  case LE:
    fprintf(listing, "<=\n");
    break;
  case GE:
    fprintf(listing, ">=\n");
    break;
  case GT:
    fprintf(listing, ">\n");
    break;
  case LBRACE:
    fprintf(listing, "{\n");
    break;
  case RBRACE:
    fprintf(listing, "}\n");
    break;
  case LBRACKET:
    fprintf(listing, "[\n");
    break;
  case RBRACKET:
    fprintf(listing, "]\n");
    break;
  case COMMA:
    fprintf(listing, ",\n");
    break;
  case EQ:
    fprintf(listing, "==\n");
    break;
  case LPAREN:
    fprintf(listing, "(\n");
    break;
  case RPAREN:
    fprintf(listing, ")\n");
    break;
  case SEMI:
    fprintf(listing, ";\n");
    break;
  case PLUS:
    fprintf(listing, "+\n");
    break;
  case SUB:
    fprintf(listing, "-\n");
    break;
  case MUL:
    fprintf(listing, "*\n");
    break;
  case DIV:
    fprintf(listing, "/\n");
    break;
  case ENDFILE:
    fprintf(listing, "EOF\n");
    break;
  case NUM:
    fprintf(listing, "NUM, val= %s\n", tokenString);
    break;
  case ID:
    fprintf(listing, "ID, name= %s\n", tokenString);
    break;
  case ERROR:
    fprintf(listing, "ERROR: %s\n", tokenString);
    break;
  case ERRORENDFILE:
    fprintf(listing, "ERROR: comment is not closed!\n");
    break;
  default: /* should never happen */
    fprintf(listing, "Unknown token: %d\n", token);
  }
}

/* Function newStmtNode creates a new statement
 * node for syntax tree construction
 */

// todo: 创建新的声明节点
TreeNode *newDclrNode(DclrKind kind, TypeSpecifier type, char *idName, int len, TreeNode *prms, TreeNode *CompoundStmt, int lineNo)
{
  TreeNode *t = (TreeNode *)malloc(sizeof(TreeNode));
  int i;
  if (t == NULL)
  {
    fprintf(listing, "Out of memeory error at line %d\n", lineNo);
  }
  else
  {
    for (i = 0; i < MAXCHILDREN; i++)
    {
      t->child[i] = NULL;
    }
    t->sibling = NULL;
    t->nodekind = DclrK;
    t->kind.dclr = kind;
    t->lineno = lineNo;
    t->type = type;
    if (kind == VarK)
    {
      t->attr.name = idName;
    }
    else if (kind == VarArrK)
    {
      t->attr.arr = (Array *)malloc(sizeof(Array));
      strcpy(t->attr.arr->name, idName);
      t->attr.arr->len = len;
    }
    else if (kind == FunK)
    {
      t->attr.name = idName;
      t->child[0] = prms;
      t->child[1] = CompoundStmt;
    }
  }
  return t;
}

TreeNode *newStmtNode(StmtKind kind, int lineNo)
{
  TreeNode *t = (TreeNode *)malloc(sizeof(TreeNode));
  int i;
  if (t == NULL)
    fprintf(listing, "Out of memory error at line %d\n", lineNo);
  else
  {
    for (i = 0; i < MAXCHILDREN; i++)
      t->child[i] = NULL;
    t->sibling = NULL;
    t->nodekind = StmtK;
    t->kind.stmt = kind;
    t->lineno = lineNo;
  }
  return t;
}

/* Function newExpNode creates a new expression
 * node for syntax tree construction
 */
TreeNode *newExpNode(ExpKind kind, TypeSpecifier type, int lineNo)
{
  TreeNode *t = (TreeNode *)malloc(sizeof(TreeNode));
  int i;
  if (t == NULL)
    fprintf(listing, "Out of memory error at line %d\n", lineNo);
  else
  {
    for (i = 0; i < MAXCHILDREN; i++)
    {
      t->child[i] = NULL;
    }
    t->sibling = NULL;
    t->nodekind = ExpK;
    t->kind.exp = kind;
    t->lineno = lineNo;
    t->type = type;
  }
  return t;
}

/* Function copyString allocates and makes a new
 * copy of an existing string
 */
char *copyString(char *s)
{
  int n;
  char *t;
  if (s == NULL)
    return NULL;
  n = strlen(s) + 1;
  t = (char *)malloc(n);
  if (t == NULL)
    fprintf(listing, "Out of memory error at line %d\n", lineno);
  else
    strcpy(t, s);
  return t;
}

/* Variable indentno is used by printTree to
 * store current number of spaces to indent
 */
static int indentno = 0;

/* macros to increase/decrease indentation */
#define INDENT indentno += 2
#define UNINDENT indentno -= 2

/* printSpaces indents by printing spaces */
static void printSpaces(void)
{
  int i;
  for (i = 0; i < indentno; i++)
    fprintf(listing, " ");
}

/* procedure printTree prints a syntax tree to the
 * listing file using indentation to indicate subtrees
 */
// todo: 完成打印语法树的函数
void printTree(TreeNode *tree)
{
  int i;
  INDENT;
  while (tree != NULL)
  {
    printSpaces();
    if (tree->nodekind == StmtK)
    {
      switch (tree->kind.stmt)
      {
      case SelectionK:
        fprintf(listing, "if:\n");
        INDENT;
        printSpaces();
        fprintf(listing, "Condition:\n");
        printTree(tree->child[0]);
        printSpaces();
        fprintf(listing, "Body:\n");
        printTree(tree->child[1]->child[0]);
        printTree(tree->child[1]->child[1]);
        UNINDENT;
        break;
      case IterationK:
        fprintf(listing, "while:\n");
        INDENT;
        printSpaces();
        fprintf(listing, "Condition:\n");
        printTree(tree->child[0]);
        printSpaces();
        fprintf(listing, "Body:\n");
        printTree(tree->child[1]->child[0]);
        printTree(tree->child[1]->child[1]);
        UNINDENT;
        break;
      case ASSIGNK:
        fprintf(listing, "assign:\n");
        INDENT;
        printSpaces();
        fprintf(listing, "Left:\n");
        printTree(tree->child[0]);
        printSpaces();
        fprintf(listing, "Right:\n");
        printTree(tree->child[1]);
        UNINDENT;
        break;
      case ReturnK:
        fprintf(listing, "return:\n");
        printTree(tree->child[0]);
        break;
      default:
        fprintf(listing, "Unknown ExpNode kind\n");
        break;
      }
    }
    else if (tree->nodekind == ExpK)
    {
      switch (tree->kind.exp)
      {
      case OpK:
        fprintf(listing, "Op: ");
        printToken(tree->attr.op, "\0");
        INDENT;
        printSpaces();
        fprintf(listing, "Left:\n");
        printTree(tree->child[0]);
        printSpaces();
        fprintf(listing, "Right:\n");
        printTree(tree->child[1]);
        UNINDENT;
        break;
      case ConstK:
        fprintf(listing, "Const: %d\n", tree->attr.val);
        break;
      case IdK:
        fprintf(listing, "Id: %s\n", tree->attr.name);
        break;
      case IdArrK:
        fprintf(listing, "Array[Index]: %s\n", tree->attr.arr->name);
        INDENT;
        printSpaces();
        fprintf(listing, "Index:\n");
        printTree(tree->child[0]);
        UNINDENT;
        break;
      case CallK:
        fprintf(listing, "Call: %s\n", tree->attr.name);
        INDENT;
        printSpaces();
        fprintf(listing, "Args:\n");
        printTree(tree->child[0]);
        UNINDENT;
        break;
      default:
        fprintf(listing, "Unknown ExpNode kind\n");
        break;
      }
    }
    else if (tree->nodekind == DclrK)
    {
      switch (tree->kind.dclr)
      {
      case VarK:
        fprintf(listing, "Declare variable: %s\n", tree->attr.name);
        break;
      case VarArrK:
        if (tree->attr.arr->len == 0)
        {
          fprintf(listing, "Declare array: %s[]\n", tree->attr.arr->name);
        }
        else
        {
          fprintf(listing, "Declare array: %s[%d]\n", tree->attr.arr->name, tree->attr.arr->len);
        }
        break;
      case FunK:
        fprintf(listing, "Declare function: %s\n", tree->attr.name);
        INDENT;
        printSpaces();
        fprintf(listing, "params:\n");
        printTree(tree->child[0]);
        printSpaces();
        fprintf(listing, "Function Body:\n");
        printTree(tree->child[1]->child[0]);
        printTree(tree->child[1]->child[1]);
        UNINDENT;
        break;
      }
    }
    else
    {
      fprintf(listing, "Unknown node kind\n");
    }
    tree = tree->sibling;
  }
  UNINDENT;
}