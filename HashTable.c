#include "HashTable.h"

//allocate and initiallize a new hash table
HashTable *allocateHashTable()
{
    //printf("Debug: Enter into hashTable : allocate\n");
    int i;
    
    HashTable *table = (HashTable*)malloc(sizeof(HashTable));
    if (!table)  //allocate failed
        return NULL;
    
    //initiallize every cell in the array
    for(i=0; i<SIZE; i++)
        table->array[i] = NULL;
    
    return table;
}

//Delete every list that inside every cell in hash table
//Then, delete the table itself
void deleteHashTable(HashTable *table)
{
     //printf("Debug: Enter into hashTable : delete\n");
    int i;
    HashTableEntry *curr, *temp;
    for(i=0; i<SIZE; i++) //going over every cell in the table
    {
        curr = table->array[i]; //Get the list head
        while(curr)
        {
            temp = curr->next;
            
            free(curr->key);  //Delete key of hashEntry
            
            free(curr->value); //Delete value (Pointer) of hashEntry
            
            free(curr); //Delete the hashEntry
            curr=temp; // move on
        }
    }
    free(table);
}
//Insert a new key name and a vlue into some cell in the table
//Return 0 if the insert was succesfull
//Retrun -1 if key already exist
int hashTableInsert(HashTable *table, char*key, void* value)
{
     //printf("Debug: Enter into hashTable : insert\n");
    int index = hashFunction(key);  //Get the right index
    HashTableEntry **curr = &(table->array[index]);
    HashTableEntry **temp=NULL;
    if(! (*curr) )  //There is no list in current cell
    {
        *curr = (HashTableEntry*)malloc(sizeof(HashTableEntry)); //Allocate new entry
        if(! (*curr) )  //Allocate faild
        {
            printf("Allocate hashTableEntry faild!\n");
            //exit(-1);
            return -1;
        }
        (*curr)->key = (char*)malloc((strlen(key)+1) * sizeof(char));
        
        strcpy((*curr)->key,key);  //Copy the key
        (*curr)->value=value;    //Copy the value
        (*curr)->next = NULL;
        return 0;
    }
    
    //else - there is some list in current cell
    
    while(*curr)
    {
        if(!strcmp((*curr)->key,key)) //if the key already exist
            return -1;
        
        //else
        temp=curr;
        curr=&((*curr)->next);
    }
    
    curr=temp;  //Now curr hold the last node in the list
    
    (*curr)->next= (HashTableEntry*)malloc(sizeof(HashTableEntry));
    if(!((*curr)->next))   //Allocate faild
    {
        printf("Allocate hashTableEntry faild!\n");
        //exit(-1);
        return -1;
    }
    (*curr)->next->key = (char*)malloc((strlen(key)+1) * sizeof(char));
    strcpy((*curr)->next->key, key);  //Copy the key
    (*curr)->next->value = value;
    (*curr)->next->next=NULL;
    return 0;
}

//Look in current table if the key is exist
//If it does, return the void *value (which will be symbole entry)
//If it doesnt, return NULL
void *hashTableLookUp(HashTable *table, char *key)
{
     //printf("Debug: Enter into hashTable : lookUp\n");
    int index = hashFunction(key);
    HashTableEntry *curr = table->array[index];
    while(curr)
    {
        if(!strcmp(curr->key, key))  //If found the key in the list
        {
            return curr->value;  //return the void *value (which will be symbole entry)
        }
        curr=curr->next;
    }
    return NULL;
}

int hashFunction(char *key)
{
     //printf("Debug: Enter into hashTable : hashFunction\n");
    unsigned int mul = 1;
    unsigned int len = (unsigned int)strlen(key);
    unsigned int i;
    for (i = 0; i < len; i++)
    {
        char x = key[i];
        mul *= x;
    }
    return (mul % SIZE);
}





