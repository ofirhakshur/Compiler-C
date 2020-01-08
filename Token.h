#ifndef TOKEN_H
#define TOKEN_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
extern FILE *yyin, *yyout;

typedef enum eTOKENS
{
    INTEGER_NUMBER=0,//0
    REAL_NUMBER=1,//1
    KEY_WORD_BLOCK=2,//2/*key word tokens*/
    KEY_WORD_BEGIN=3,//3
    KEY_WORD_END=4,//4
    KEY_WORD_TYPE=5,//5
    KEY_WORD_INTEGER=6,//6
    KEY_WORD_REAL=7,//7
    KEY_WORD_ARRAY=8,//8
    KEY_WORD_OF=9,//9
    KEY_WORD_WHEN=10,//10
    KEY_WORD_DO=11,//11
    KEY_WORD_DEFAULT=12,//12
    KEY_WORD_END_WHEN=13,//13
    KEY_WORD_FOR=14,//14
    KEY_WORD_END_FOR=15,//15
    KEY_WORD_MALLOC=16,//16
    KEY_WORD_SIZE_OF=17,//17
    KEY_WORD_FREE=18,//18/*end of keyword tokens*/
    TOKEN_ID=19,//19
    TOKEN_PLUS=20,//20 /*ar_op tokens*/
    TOKEN_MINUS=21,//21
    TOKEN_MULT=22,//22
    TOKEN_DIV=23,//23
    TOKEN_POWER=24,//24 /*end of ar_op tokens*/
    TOKEN_VAL_UP=25,//25
    TOKEN_REL_OP=26,//26
    TOKEN_IS_EQUAL=27,//27 /*rel_op tokens*/
    TOKEN_BIG_EQUAL=28,//28
    TOKEN_LOW_EQUAL=29,//29
    TOKEN_BIGGER=30,//
    TOKEN_LOWER=31,//
    TOKEN_INEQUAL=32,///*end of rel_op tokens*/
    TOKEN_ASSIGNMENT=33,//
    TOKEN_POINTER_INDECATOR=34,//
    TOKEN_ADDRES=35,//
    TOKEN_COMMA=36, //
    TOKEN_SEMICOLON=37,//
    TOKEN_R_BRACKETS_LEFT=38,//
    TOKEN_R_BRACKETS_RIGHT=39, //
    TOKEN_S_BRACKETS_LEFT=40,//
    TOKEN_S_BRACKETS_RIGHT=41,//
    TOKEN_COLON=42,//
    KEY_WORD_IS=43,  //
    TOKEN_EOF=44//
}eTOKENS;

typedef struct Token
{
	eTOKENS kind;
	char *lexeme;
	int lineNumber;
}Token;

typedef struct Node
{
	Token *tokensArray;
	struct Node *prev;
	struct Node *next;
    int arrayLogicSize;  //I add this, this var will help to print every token from the array of the Node
    int counter;   //helping for next_token(); initiallized to zero in every node at token.c
} Node;

void create_and_store_token(eTOKENS kind, char* lexeme, int numOfLine);
Token *next_token();
Token *back_token();
Token *actual_token();
void freeList();
char* getTokenNameByNum(eTOKENS kind);

#endif
