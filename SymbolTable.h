#ifndef SymbolTable_h
#define SymbolTable_h

#include "HashTable.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef enum Type
{
    Integer,
    Real,
    Array,
    Pointer,
    Undefined
}Type;

typedef struct SymbolEntry
{
    char *name;
    Type type;
    Type subType;
}SymbolEntry;

typedef struct SymboleTable
{
    HashTable *table;
    struct SymboleTable *father;
    
}SymbolTable;

SymbolTable *make_table(SymbolTable *current_table);
SymbolTable *pop_table(SymbolTable *current_table);
SymbolEntry *insert(SymbolTable *current_table, char *id_name);
SymbolEntry *lookup(SymbolTable *current_table, char *id_name);
SymbolEntry *find(SymbolTable *current_table, char *id_name);
void set_id_type (SymbolEntry *id_entry, Type id_type);
Type get_id_type (SymbolEntry *id_entry);
char* checkType(Type type); //Getting Type and return char* that translate the type

#endif /* SymbolTable_h */
