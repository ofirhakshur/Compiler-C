#include "SymbolTable.h"

//Creat a new symbole table and link it to the current table
//Return the new symbole table or Null if allocation faild
SymbolTable *make_table(SymbolTable *current_table)
{
    //printf("Debug: @@@@@@@@@@@@Enter into make table@@@@@@@@@@@@\n");
    SymbolTable *symbolTable = (SymbolTable*)malloc(sizeof(SymbolTable));
    if(!symbolTable) //Allocate faild
        return NULL;
    symbolTable->table = allocateHashTable();
    symbolTable->father = current_table;
    
    return symbolTable;
}

//Delete the hash table thats inside current symbol table
//Then, delete the current symbol table and return his father
SymbolTable *pop_table(SymbolTable *current_table)
{
    //printf("Debug: @@@@@@@@@@@@@Enter into pop table@@@@@@@@@@@@@@\n");
    SymbolTable *temp = current_table->father;  //save the father of current symbol table
    deleteHashTable(current_table->table);     //delete the hash table from current symbol table
    free(current_table);

    return temp;
}

//Find if id_name is already exist in hash table -if it is, return null
//if its not - allocate new symbol entry and isert it into the hash table
SymbolEntry *insert(SymbolTable *current_table, char *id_name)
{
     //printf("Debug: Enter into insert\n");
    SymbolEntry *symbolEntry;
    if( hashTableLookUp(current_table->table,id_name) == NULL)  //If id_name is not inside symbole table -> hashtable - insert id_name
    {
        symbolEntry = (SymbolEntry*)malloc(sizeof(symbolEntry));
        if(!symbolEntry) //allocation faild
        {
            printf("Allocate symbolentry faild!!!!!\n");
            return NULL;
        }
        //Insert id_name into SymbolEntry->char *name
        symbolEntry->name = (char*)malloc( (strlen(id_name)+1)*sizeof(char) );
        strcpy(symbolEntry->name, id_name);
        /*****************************/
        
        hashTableInsert(current_table->table, id_name,symbolEntry);
        return symbolEntry;
    }
    //else - if id_name is already exist
    return NULL;
}
//Function is search inside SymbolTable->HashTable the id_name using
//hashTableLookUp function and return the entry, or NULL if entry wasnt find
SymbolEntry *lookup(SymbolTable *current_table, char *id_name)
{
     //printf("Debug: Enter into lookup\n");
    SymbolEntry *symbolEntry = (SymbolEntry*)hashTableLookUp(current_table->table, id_name);
    if(symbolEntry)
        return symbolEntry;
    
    return NULL;
}
//Try to find id_name in current SymbolTable(SymbolTable->table)
//If id_name not found, try to move to all previous SymbolTable
//If found in some SymbolTable - return the SymbolEntry, NULL otherwise
SymbolEntry *find(SymbolTable *current_table, char *id_name)
{
     //printf("Debug: Enter into find\n");
    SymbolTable *curr = current_table;
    SymbolEntry *res;
    while(curr)
    {
        res = lookup(curr, id_name);
        if(res)
            return res;
        curr=curr->father;
    }
    return NULL;
}

//Also set hashEntry->char *key into SymbolEntry->char *name
void set_id_type (SymbolEntry *id_entry, Type id_type)
{
        // printf("Debug: Enter into set_id_type\n");
    if(id_entry)
    {
        id_entry->type = id_type;
    }
}

Type get_id_type (SymbolEntry *id_entry)
{    // printf("Debug: Enter into get_id_type\n");
    if(id_entry)
    {
        
        return id_entry->type;
    }
    else
    {
       
        //fprintf(stdout,"Now the entry is null\n");
        return Undefined;
    }
}

char* checkType(Type type)
{
    switch(type)
    {
        case Integer: {
            return "Integer";
            break; }
        case Real: {
            return "Real";
            break; }
            
        case Array: {
            return "Array";
            break;
        }
        case Pointer:{
            return "Pointer";
            break;
        }
        case Undefined: {
            return "Undefined";
            break;
        }
    }
}








