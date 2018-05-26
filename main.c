/****************************************************/
/* File: main.c                                     */
/* Main program for TINY compiler                   */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include "globals.h"

/* set NO_PARSE to TRUE to get a scanner-only compiler */
#define NO_PARSE FALSE
/* set NO_ANALYZE to TRUE to get a parser-only compiler */
#define NO_ANALYZE TRUE

/* set NO_CODE to TRUE to get a compiler that does not
 * generate code
 */
#define NO_CODE TRUE

/* set SAVE_SCAN_RESULT_TOFILE to TRUE to save scan
 * result to file "scan_result.txt"
 */
#define SAVE_RESULT_TO_FILE FALSE

#include "util.h"
#if NO_PARSE
#include "scan.h"
#else
#include "parse.h"
#if !NO_ANALYZE
#include "analyze.h"
#if !NO_CODE
#include "cgen.h"
#endif
#endif
#endif

/* allocate global variables */
int lineno = 0;
FILE *source;
FILE *listing; // 保存token的文件.
FILE *code;

/* allocate and set tracing flags */
int EchoSource = FALSE;
int TraceScan = FALSE;
int TraceParse = TRUE;
int TraceAnalyze = FALSE;
int TraceCode = FALSE;

int Error = FALSE;

int main(int argc, char *argv[]) {
  TreeNode *syntaxTree;

  // 读取输入的文件名, 并拷贝到pgm字符数组里
  char pgm[120]; /* source code file name */
  if (argc != 2) // 参数不正确
  {
    fprintf(stderr, "usage: %s <filename>\n", argv[0]);
    exit(1);
  }
  strcpy(pgm, argv[1]);

  // 打开输入文件.
  if (strchr(pgm, '.') == NULL)
    strcat(pgm, ".tny");
  source = fopen(pgm, "r");
  if (source == NULL) {
    fprintf(stderr, "File %s not found\n", pgm);
    exit(1);
  }

  // 打开输出文件
#if SAVE_RESULT_TO_FILE
  char* result_filename = {"result.txt"};
  listing = fopen(result_filename, "w+");
  fprintf(listing, "C-minus COMPILATION: %s\n", pgm);
#else
  listing = stdout; /* send listing to screen */
  fprintf(listing, "\nTINY COMPILATION: %s\n", pgm);
#endif

// 如果没有语法分析, 仅做词法扫描
#if NO_PARSE
  while (getToken() != ENDFILE)
    ;
  fclose(listing);
#else
  scan();
  syntaxTree = parse();
  if (TraceParse) {
    fprintf(listing, "\nSyntax tree:\n");
    printTree(syntaxTree);
  }
#if !NO_ANALYZE
  if (!Error) {
    if (TraceAnalyze)
      fprintf(listing, "\nBuilding Symbol Table...\n");
    buildSymtab(syntaxTree);
    if (TraceAnalyze)
      fprintf(listing, "\nChecking Types...\n");
    typeCheck(syntaxTree);
    if (TraceAnalyze)
      fprintf(listing, "\nType Checking Finished\n");
  }
#if !NO_CODE
  if (!Error) {
    char *codefile;
    int fnlen = strcspn(pgm, ".");
    codefile = (char *)calloc(fnlen + 4, sizeof(char));
    strncpy(codefile, pgm, fnlen);
    strcat(codefile, ".tm");
    code = fopen(codefile, "w");
    if (code == NULL) {
      printf("Unable to open %s\n", codefile);
      exit(1);
    }
    codeGen(syntaxTree, codefile);
    fclose(code);
  }
#endif
#endif
#endif
  destroyTokenTable();
  fclose(source);
  fclose(listing);
  return 0;
}
