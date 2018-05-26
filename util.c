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
    fprintf(listing, "<\n");
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
  } // TODO: add some state !done
}

/* Function newStmtNode creates a new statement
 * node for syntax tree construction
 */

// todo: 创建新的声明节点
TreeNode *newDclrNode(DclrKind kind, TypeSpecifier type, char* idName, int len, TreeNode* prms, TreeNode* CompoundStmt)
{
  TreeNode *t = (TreeNode *)malloc(sizeof(TreeNode));
  int i;
  if (t == NULL)
  {
    fprintf(listing, "Out of memeory error at line %d\n", lineno);
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
    t->lineno = lineno;
    t->type = type;
    if (kind == VarK) {
      t->attr.name = idName;
    } else if (kind == VarArrK) {
      t->attr.arr = (Array*)malloc(sizeof(Array));
      strcpy(t->attr.arr->name, idName);
      t->attr.arr->len = len;
    } else if (kind == FunK) {
      t->attr.name = idName;
      t->child[0] = prms;
      t->child[1] = CompoundStmt;
    }
  }
  return t;
}

TreeNode *newStmtNode(StmtKind kind)
{
  TreeNode *t = (TreeNode *)malloc(sizeof(TreeNode));
  int i;
  if (t == NULL)
    fprintf(listing, "Out of memory error at line %d\n", lineno);
  else
  {
    for (i = 0; i < MAXCHILDREN; i++)
      t->child[i] = NULL;
    t->sibling = NULL;
    t->nodekind = StmtK;
    t->kind.stmt = kind;
    t->lineno = lineno;
  }
  return t;
}

/* Function newExpNode creates a new expression
 * node for syntax tree construction
 */
TreeNode *newExpNode(ExpKind kind, TypeSpecifier type)
{
  TreeNode *t = (TreeNode *)malloc(sizeof(TreeNode));
  int i;
  if (t == NULL)
    fprintf(listing, "Out of memory error at line %d\n", lineno);
  else
  {
    for (i = 0; i < MAXCHILDREN; i++)
    {

      t->child[i] = NULL;
    }
    t->sibling = NULL;
    t->nodekind = ExpK;
    t->kind.exp = kind;
    t->lineno = lineno;
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
        fprintf(listing, "if\n");
        break;
      case IterationK:
        fprintf(listing, "while\n");
        break;
      case ASSIGNK:
        fprintf(listing, "Assign to: %s\n", tree->attr.name);
        break;
      case ReturnK:
        fprintf(listing, "Read: %s\n", tree->attr.name);
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
        break;
      case ConstK:
        fprintf(listing, "Const: %d\n", tree->attr.val);
        break;
      case IdK:
        fprintf(listing, "Id: %s\n", tree->attr.name);
        break;
      case IdArrK:
        fprintf(listing, "Array: %s\n Index: \n", tree->attr.name);
        break;
      default:
        fprintf(listing, "Unknown ExpNode kind\n");
        break;
      }
    }
    else
      fprintf(listing, "Unknown node kind\n");
    for (i = 0; i < MAXCHILDREN; i++)
      printTree(tree->child[i]);
    tree = tree->sibling;
  }
  UNINDENT;
}