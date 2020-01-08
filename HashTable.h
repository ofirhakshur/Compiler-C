#ifndef HashTable_h
#define HashTable_h
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define SIZE 101


typedef struct HashTableEntry
{
    char *key;
    void *value;
    struct HashTableEntry *next;
}HashTableEntry;

typedef struct HashTable
{
    HashTableEntry *array[SIZE];
    
}HashTable;


HashTable *allocateHashTable(void);
void deleteHashTable(HashTable *table);
int hashTableInsert(HashTable *table, char*key, void* value);
void *hashTableLookUp(HashTable *table, char* key);
int hashFunction(char *key);

#endif
