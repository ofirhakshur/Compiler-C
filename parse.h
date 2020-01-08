#ifndef parse_h
#define parse_h
#include "Token.h"
#include "SymbolTable.h"
#include <stdio.h>
extern FILE *yyin, *yyout;



void parse_program();
void parse_block();
void parse_definitions();
void parse_A();
void parse_definition();
void parse_var_definition();

Type parse_B();
void parse_type_definition();

Type parse_type_indicator(SymbolEntry *entry);
Type parse_basic_type();
Type parse_array_type(SymbolEntry *entry);
Type parse_pointer_type(SymbolEntry *entry);

Type parse_size();
void parse_commands();
void parse_D();
void parse_command();
void parse_command_helper(SymbolEntry *entry);
Type parse_Z(SymbolEntry *entry);

Type parse_expression(SymbolEntry *entry);
Type parse_F(SymbolEntry *entry);
void parser();




char* getTokenNameByNum(eTOKENS kind);
void errorRecovery(eTOKENS *first, eTOKENS *follow, int firstVal, int followVal, Token *t);
void match_relopFromArray(eTOKENS *first, eTOKENS *follow, int size_first, int size_follow);
void match(eTOKENS token, eTOKENS *first, eTOKENS *follow, int size_first, int size_follow);
#endif /* parse_h */
