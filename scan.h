/****************************************************/
/* File: scan.h                                     */
/* The scanner interface for the TINY compiler      */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#ifndef _SCAN_H_
#define _SCAN_H_
#include "globals.h"

/* MAXTOKENLEN is the maximum size of a token */

/* tokenString array stores the lexeme of each token */
extern char tokenString[MAXTOKENLEN + 1];

// 扫描源文件生成符号表
void scan(void);

/* function getToken returns the 
 * next token in source file
 */

TokenType getToken(void);

TokenNode *getNextToken(void);

#endif
