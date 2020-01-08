#include <stdio.h>
#include "Token.h"
#include "parse.h"
#include "SymbolTable.h"

//Global variable
SymbolTable *cur_table=NULL;
Type type;
FILE *semantic_yyout;

void match(eTOKENS token, eTOKENS *first, eTOKENS *follow, int size_first, int size_follow)
{
    Token *t = next_token();
    if(t->kind != token)
    {
        
        errorRecovery(first,follow,size_first,size_follow,t);
    }
}
//Rel_op could be: ==, >=, <=, >, <, !=
void match_relopFromArray(eTOKENS *first, eTOKENS *follow, int size_first, int size_follow)
{
    Token *t = next_token();
    if(t->kind!=TOKEN_IS_EQUAL && t->kind!=TOKEN_BIG_EQUAL && t->kind!=TOKEN_LOW_EQUAL && t->kind!=TOKEN_BIGGER && t->kind!=TOKEN_LOWER && t->kind!=TOKEN_INEQUAL )
    {
        errorRecovery(first,follow,size_first,size_follow,t);
        return;
        
    }
}
//Erro recovery function
void errorRecovery(eTOKENS *first, eTOKENS *follow, int firstVal, int followVal, Token *t)
{
    int i;
    fprintf(yyout,"Expected one of tokens ");
    for(i=0; i<firstVal; i++)
    {
        fprintf(yyout, "%s,", getTokenNameByNum(first[i]));
    }
      fprintf(yyout, " at line:%d, Actual token %s, lexema: %s\n",t->lineNumber, getTokenNameByNum(t->kind), t->lexeme );
    
    while(t->kind != TOKEN_EOF)
    {
        for(i=0; i<followVal; i++)
        {
            if(t->kind == follow[i])
            {
                t=back_token();
                return;
            }
        }//for
        t=next_token();
    } //while
    
    exit(0);
}


void parser(FILE *semanticFile)
{
    semantic_yyout=semanticFile;
    parse_program();
    match(TOKEN_EOF,NULL,NULL,0,0);
}

void parse_program()
{
    fprintf(yyout,"PROGRAM ---> BLOCK\n");
    parse_block();
}
void parse_block()
{
    eTOKENS first[1]={KEY_WORD_BLOCK};
    eTOKENS follow[2]={TOKEN_EOF,TOKEN_SEMICOLON};
    
    Token *t = next_token();
    switch(t->kind)
    {
        case KEY_WORD_BLOCK:
        {
            //Open new symbol table
            cur_table=make_table(cur_table);
            
            fprintf(yyout,"BLOCK ---> block DEFINITIONS ; begin COMMANDS ; end\n");
            parse_definitions();
            match(TOKEN_SEMICOLON,first,follow,1,2);
            match(KEY_WORD_BEGIN,first,follow,1,2);
            //printf("DEBUG: @@@@@@@@@@@@@ begin @@@@@@@@@@@@@@\n");
            parse_commands();
            match(TOKEN_SEMICOLON,first,follow,1,2);
            match(KEY_WORD_END,first,follow,1,2);
            
            //Pop out current symbol table
            cur_table=pop_table(cur_table);
            break;
        }
        default:
        {
            errorRecovery(first,follow,1,2,t);
            break;
        }
    }
}
void parse_definitions()
{
    fprintf(yyout,"DEFINITIONS ---> DEFINITION A\n");
    parse_definition();
    parse_A();
}
void parse_A()
{
    eTOKENS first[1]={TOKEN_SEMICOLON};
    eTOKENS follow[1]={TOKEN_SEMICOLON};
    
    eTOKENS firstSemicolon[3] = {KEY_WORD_BEGIN, KEY_WORD_TYPE, TOKEN_ID};
    eTOKENS followSemicolon[1]={TOKEN_SEMICOLON};
    
    Token *t = next_token();
    switch(t->kind)
    {
        case TOKEN_SEMICOLON:
        {
            
            t = next_token();
            switch(t->kind)
            {
                case TOKEN_ID:
                case KEY_WORD_TYPE:
                {
                    fprintf(yyout,"A ---> ; DEFINITION A\n");
                    t=back_token();
                    parse_definition();
                    parse_A();
                    break;
                }
               
                case KEY_WORD_BEGIN:
                {
                    fprintf(yyout,"A ---> EPSILON\n");
                    t=back_token();
                    t=back_token();
                    break;
                }
                    
                default:
                {
                    errorRecovery(firstSemicolon,followSemicolon,3,1,t);
                    break;
                }
            
            }
            break;  //break semicolon case
        }
        default:
        {
            errorRecovery(first,follow,1,1,t);
            break;
        }
    }
}

void parse_definition()
{
    eTOKENS first[2]={TOKEN_ID,KEY_WORD_TYPE};
    eTOKENS follow[1]={TOKEN_SEMICOLON};
    
    Token *t = next_token();
    switch(t->kind)
    {
        case TOKEN_ID:
        {
            fprintf(yyout,"DEFINITION ---> VAR_DEFINITION\n");
            t=back_token();
            parse_var_definition();
            break;
        }
        case KEY_WORD_TYPE:
        {
            fprintf(yyout,"DEFINITION ---> TYPE_DEFINITION\n");
            t=back_token();
            parse_type_definition();
            break;
        }
        default:
        {
            errorRecovery(first,follow,2,1,t);
            break;
        }
    }
}
void parse_var_definition()
{
    SymbolEntry *cur_entry;
    eTOKENS first[1]= {TOKEN_ID};
    eTOKENS follow[1]={TOKEN_SEMICOLON};
    
    Token *t = next_token();
    switch(t->kind)
    {
        case TOKEN_ID:
        {
            fprintf(yyout,"VAR_DEFINITION ---> id : B\n");
            //Insert current lexeme into current Symbol Table
            cur_entry=insert(cur_table,t->lexeme);
            if(!cur_entry)  //Id already exist!
            {
                fprintf(semantic_yyout,"Semantic ERROR in line %d: duplicate definition of  %s\n",t->lineNumber,t->lexeme);
            }
            match(TOKEN_COLON,first,follow,1,1);
            type=parse_B();
            if(type != Undefined) //Type that gets back from parse_B is not Undefined
            {
                set_id_type(cur_entry,type);  //Insert type into entry
            }
            
            break;
        }
        default:
        {
            errorRecovery(first,follow,1,1,t);
            break;
        }
    }
}

Type parse_B()
{
    SymbolEntry *cur_entry;
    eTOKENS first[3]={TOKEN_ID,KEY_WORD_INTEGER,KEY_WORD_REAL};
    eTOKENS follow[1]={TOKEN_SEMICOLON};
    
    Token *t = next_token();
    switch(t->kind)
    {
        case TOKEN_ID:
        {
            fprintf(yyout,"B ---> type_name\n");
            cur_entry=find(cur_table,t->lexeme);  //Find if type_name is defined
            if(cur_entry) //Type_name was defined
            {
                type=get_id_type(cur_entry);
                return type;
            }
            else  //Type_name was not declared
            {
                fprintf(semantic_yyout,"Semantic ERROR in line %d: %s is undefined\n",t->lineNumber, t->lexeme);
            }
            return Undefined;
            break;
        }
        case KEY_WORD_INTEGER:
        case KEY_WORD_REAL:
        {
            fprintf(yyout,"B ---> BASIC_TYPE\n");
            t=back_token();
            return (parse_basic_type());
            break;
        }
        default:
        {
            errorRecovery(first,follow,3,1,t);
            return Undefined;
            break;
        }
    }
}
void parse_type_definition()
{
    SymbolEntry *cur_entry;
    eTOKENS first[1]={KEY_WORD_TYPE};
    eTOKENS follow[1]={TOKEN_SEMICOLON};
    
    Token *t = next_token();
    Token *actualToken;
    switch(t->kind)
    {
        case KEY_WORD_TYPE:
        {
            fprintf(yyout,"TYPE_DEFINITION ---> type type_name is TYPE_INDICATOR\n");
            match(TOKEN_ID,first,follow,1,1);
            actualToken=actual_token(); //Help to get "type_name" lexeme
            //Insert lexeme into Symbol Table
            
            //fprintf(yyout,"DEBUG TYPE_DEFINITION: actualToken is {%s}\n\n", actualToken->lexeme);
            
            cur_entry=insert(cur_table,actualToken->lexeme);
            if(!cur_entry) //id is already exist
            {
                fprintf(semantic_yyout,"Semantic ERROR in line %d: duplicate definition of  %s\n", actualToken->lineNumber,actualToken->lexeme);
            }
            match(KEY_WORD_IS,first,follow,1,1);
            type=parse_type_indicator(cur_entry);
            
            //fprintf(yyout,"DEBUG TYPE_DEFINITION: type is {%s}\n\n", checkType(type));
            
            if(type!=Undefined)
            {
                set_id_type(cur_entry,type); //Set type to the id
            }
            
            //fprintf(yyout,"DEBUG TYPE_DEFINITION AFTER SET TYPE: id=%s, type= %s\n\n", cur_entry->name, checkType(get_id_type(cur_entry)));
            if(type==Pointer || type==Array)
            {
                //fprintf(yyout,"DEBUG: subType is %s\n\n", checkType(cur_entry->subType));
            }
            
            break;
        }
        default:
        {
            errorRecovery(first,follow,1,1,t);
            break;
        }
    }
}

Type parse_type_indicator(SymbolEntry *entry)
{
   
    eTOKENS first[4] = {KEY_WORD_INTEGER, KEY_WORD_REAL, KEY_WORD_ARRAY, TOKEN_POINTER_INDECATOR};
    eTOKENS follow[1]={TOKEN_SEMICOLON};
    
    Token *t = next_token();
    switch(t->kind)
    {
        case KEY_WORD_INTEGER:
        case KEY_WORD_REAL:
        {
            fprintf(yyout,"TYPE_INDICATOR ---> BASIC_TYPE\n");
            t=back_token();
            return (parse_basic_type());
            break;
        }
        case KEY_WORD_ARRAY:
        {
            fprintf(yyout,"TYPE_INDICATOR ---> ARRAY_TYPE\n");
            t=back_token();
            return (parse_array_type(entry));
            break;
        }
        case TOKEN_POINTER_INDECATOR:
        {
            fprintf(yyout,"TYPE_INDICATOR ---> POINTER_TYPE\n");
            t=back_token();
            return (parse_pointer_type(entry));
            break;
        }
        default:
        {
            errorRecovery(first,follow,4,1,t);
            return Undefined;
            break;
        }
    }
}

Type parse_basic_type()
{
    eTOKENS first[2]={KEY_WORD_INTEGER,KEY_WORD_REAL};
    eTOKENS follow[1]={TOKEN_SEMICOLON};
    
    Token *t = next_token();
    switch(t->kind)
    {
        case KEY_WORD_INTEGER:
        {
            fprintf(yyout,"BASIC_TYPE -> integer\n");
            return Integer;
            break;
        }
        case KEY_WORD_REAL:
        {
            fprintf(yyout,"BASIC_TYPE -> real\n");
            return Real;
            break;
        }
        default:
        {
            errorRecovery(first,follow,2,1,t);
            return Undefined;
            break;
        }
    }
}

Type parse_array_type(SymbolEntry *entry)
{
  
    eTOKENS first[1] ={KEY_WORD_ARRAY};
    eTOKENS follow[1]={TOKEN_SEMICOLON};
    
    Token *t = next_token();
    switch(t->kind)
    {
        case KEY_WORD_ARRAY:
        {
            fprintf(yyout,"ARRAY_TYPE ---> array [SIZE] of BASIC_TYPE\n");
            match(TOKEN_S_BRACKETS_LEFT,first,follow,1,1);
            type=parse_size();
            if(type!=Integer) //If size is not an Integer
            {
                fprintf(semantic_yyout,"Semantic ERROR in line %d: Array size must be of type Integer\n", t->lineNumber);
            }
            match(TOKEN_S_BRACKETS_RIGHT,first,follow,1,1);
            match(KEY_WORD_OF,first,follow,1,1);
            type=parse_basic_type();
            if(entry)
            {
                entry->subType=type;
                return Array;
            }
            else //If we get inside here, thats mean that the id was undefined!
            {
                fprintf(semantic_yyout,"Semantic ERROR: Type not recognized\n");
            }
            return Undefined;
            break;
        }
        default:
        {
            errorRecovery(first,follow,1,1,t);
            return Undefined;
            break;
        }
    }
}

Type parse_pointer_type(SymbolEntry *entry)
{
    eTOKENS first[1] ={TOKEN_POINTER_INDECATOR};
    eTOKENS follow[1]={TOKEN_SEMICOLON};
    
    Token *t = next_token();
    switch(t->kind)
    {
        case TOKEN_POINTER_INDECATOR:
        {
            fprintf(yyout,"POINTER_TYPE ---> ^ B\n");
            type=parse_B();
            if(entry)  //If the id is defined
            {
                entry->subType=type; //Insert into id enrty the subtype
                
                return Pointer;    //return Pointer means that id now is a Pointer
            }
            else //Get inside here means that id was undefined!
            {
                fprintf(semantic_yyout,"Semantic ERROR: Type not recognized\n");
            }
            return Undefined;
            break;
        }
        default:
        {
            errorRecovery(first,follow,1,1,t);
            return Undefined;
            break;
        }
    }
}

Type parse_size()
{
    eTOKENS first[1] = {INTEGER_NUMBER};
    eTOKENS follow[1]={TOKEN_S_BRACKETS_RIGHT};
    
    Token *t = next_token();
    switch(t->kind)
    {
        case INTEGER_NUMBER:
        {
            fprintf(yyout,"SIZE ---> int_num\n");
            return Integer;
            break;
        }
        default:
        {
            errorRecovery(first,follow,1,1,t);
            return Undefined;
            break;
        }
    }
}


void parse_commands()
{
    fprintf(yyout,"COMMANDS ---> COMMAND D\n");
    parse_command();
    parse_D();
}
void parse_D()
{
    eTOKENS first[1] = {TOKEN_SEMICOLON};
    eTOKENS follow[1]={TOKEN_SEMICOLON};
    
    eTOKENS firstSemicolon[9]={KEY_WORD_BLOCK, KEY_WORD_FREE, KEY_WORD_FOR,
        KEY_WORD_WHEN,TOKEN_ID,KEY_WORD_END_FOR,KEY_WORD_END_WHEN,KEY_WORD_DEFAULT,
        KEY_WORD_END
    };
    eTOKENS followSemicolon[1]={TOKEN_SEMICOLON};
    
    
    Token *t = next_token();
    switch(t->kind)
    {
        case TOKEN_SEMICOLON:
        {
            t=next_token();
            switch(t->kind)
            {
                case TOKEN_ID:
                case KEY_WORD_WHEN:
                case KEY_WORD_FOR:
                case KEY_WORD_FREE:
                case KEY_WORD_BLOCK:
                {
                    fprintf(yyout,"D ---> ; COMMAND D\n");
                    t=back_token();
                    parse_command();
                    parse_D();
                    break;
                }
                
                case KEY_WORD_END:
                case KEY_WORD_DEFAULT:
                case KEY_WORD_END_WHEN:
                case KEY_WORD_END_FOR:
                {
                    fprintf(yyout,"D ---> EPSILON\n");
                    t=back_token();
                    t=back_token();
                    break;
                }
                    
                default:
                {
                    errorRecovery(firstSemicolon,followSemicolon,9,1,t);
                    break;
                }
               
            }
            break; //break of case semi colon
        }
        default:
        {
            errorRecovery(first,follow,1,1,t);
            break;
        }
    }
}
void parse_command()
{
    SymbolEntry *cur_entry;
    Type typeLeft, typeRight;
    
    eTOKENS first[5] = {KEY_WORD_WHEN,KEY_WORD_FOR, TOKEN_ID, KEY_WORD_FREE,KEY_WORD_BLOCK};
    eTOKENS follow[1] = {TOKEN_SEMICOLON};
    
    Token *t = next_token();
    Token *actualToken;
    switch(t->kind)
    {
        case KEY_WORD_WHEN:
        {
            fprintf(yyout,"COMMAND ---> when(EXPRESSION rel_op EXPRESSION) do COMMANDS; default COMMANDS; end_when\n");
            match(TOKEN_R_BRACKETS_LEFT,first,follow,5,1);
            typeLeft=parse_expression(NULL); //Type of first EXPRESSION
            match_relopFromArray(first,follow,5,1);
            typeRight=parse_expression(NULL); //Type of seconde EXPRESSION
            
            //Semantic checking : if leftType isnt equal to rightType then it is semantic error
            //Arithmetic operations could be only between Real and Integer!
            if(!( (typeLeft==Integer && typeRight==Integer)||(typeLeft==Real && typeRight==Real) ) )
            {
                fprintf(semantic_yyout,"Semantic ERROR in line %d: miss match types in comparison\n",t->lineNumber);
            }
            match(TOKEN_R_BRACKETS_RIGHT,first,follow,5,1);
            match(KEY_WORD_DO,first,follow,5,1);
            parse_commands();
            match(TOKEN_SEMICOLON,first,follow,5,1);
            match(KEY_WORD_DEFAULT,first,follow,5,1);
            parse_commands();
            match(TOKEN_SEMICOLON,first,follow,5,1);
            match(KEY_WORD_END_WHEN,first,follow,5,1);
            break;
            
        }
        case KEY_WORD_FOR:
        {
            fprintf(yyout,"COMMAND ---> for(id=EXPRESSION; id rel_op EXPRESSION; id++)COMMANDS; end_for\n");
            match(TOKEN_R_BRACKETS_LEFT,first,follow,5,1);
            match(TOKEN_ID,first,follow,5,1);
            
            actualToken=actual_token();
            cur_entry=find(cur_table,actualToken->lexeme);
            if(!cur_entry) //If id is undefined
            {
                fprintf(semantic_yyout,"Semantic ERROR in line %d: %s is undefined\n",actualToken->lineNumber,actualToken->lexeme);
            }
            else //Id is indeed defined - get the type of id
            {
                typeLeft=get_id_type(cur_entry);
            }
            match(TOKEN_ASSIGNMENT,first,follow,5,1);
            typeRight=parse_expression(cur_entry);
            if(typeLeft != typeRight) //Semantic error checking
            {
                fprintf(semantic_yyout,"Semantic ERROR in line %d: miss match types in comparison\n",actualToken->lineNumber);
            }
            match(TOKEN_SEMICOLON,first,follow,5,1);
            match(TOKEN_ID,first,follow,5,1);
            actualToken=actual_token();
            
            cur_entry=find(cur_table, actualToken->lexeme);
            if(!cur_entry)  //Semantic error - if id is undefiend
            {
                fprintf(semantic_yyout,"Semantic ERROR in line %d: %s is undefined\n",actualToken->lineNumber, actualToken->lexeme);
            }
            else //Id is ok and defined - get the type of the id
            {
                typeLeft=get_id_type(cur_entry);
            }
            
            match_relopFromArray(first,follow,5,1);
            typeRight=parse_expression(cur_entry);
            if(!( (typeLeft==Integer && typeRight==Integer)||(typeLeft==Real && typeRight==Real) ) ) //Semantic error checking
            {
                fprintf(semantic_yyout,"Semantic ERROR in line %d: miss match types in comparison\n",actualToken->lineNumber);
            }
            match(TOKEN_SEMICOLON,first,follow,5,1);
            match(TOKEN_ID,first,follow,5,1);
            actualToken=actual_token();
            cur_entry=find(cur_table, actualToken->lexeme);
            if(!cur_entry) //Semantic error; id is undeined
            {
                fprintf(semantic_yyout,"Semantic ERROR in line %d: %s is undefined\n",actualToken->lineNumber, actualToken->lexeme);
            }
            else //id is defined
            {
                type=get_id_type(cur_entry); //Get the type of the id
                if(type!=Integer) //Cant make val up to variable that is not integer!
                {
                    fprintf(semantic_yyout,"Semantic ERROR in line %d: type of %s have to be an Integer\n", actualToken->lineNumber, actualToken->lexeme);
                }
            }
            match(TOKEN_VAL_UP,first,follow,5,1);
            match(TOKEN_R_BRACKETS_RIGHT,first,follow,5,1);
            parse_commands();
            match(TOKEN_SEMICOLON,first,follow,5,1);
            match(KEY_WORD_END_FOR,first,follow,5,1);
            break;
        }
        case KEY_WORD_FREE:
        {
            fprintf(yyout,"COMMAND ---> free(id)\n");
            match(TOKEN_R_BRACKETS_LEFT,first,follow,5,1);
            match(TOKEN_ID,first,follow,5,1);
            
            actualToken=actual_token();
            cur_entry=find(cur_table, actualToken->lexeme);
            
            if(!cur_entry)  //Id is undefined!
            {
                fprintf(semantic_yyout,"Semantic ERROR in line %d: %s is undefined\n", actualToken->lineNumber, actualToken->lexeme);
            }
            else //Id is defined
            {
                type=get_id_type(cur_entry);
                //Cant deallocate something that is not pointer or array!
                if( !(/*type==Array || */type==Pointer) )
                {
                    fprintf(semantic_yyout,"Semantic ERROR in line %d: type of %s must be Pointer\n",actualToken->lineNumber, actualToken->lexeme);
                }
            }
            match(TOKEN_R_BRACKETS_RIGHT,first,follow,5,1);
            break;
        }
        case KEY_WORD_BLOCK:
        {
            fprintf(yyout,"COMMAND ---> BLOCK\n");
            t=back_token();
            parse_block();
            break;
        }
        case TOKEN_ID:
        {
            fprintf(yyout, "COMMAND ---> id COMMAND_HELPER\n");
            actualToken=actual_token();
            cur_entry=find(cur_table, actualToken->lexeme);
            if(!cur_entry)  //Id is undefined!
            {
                fprintf(semantic_yyout,"Semantic ERROR in line %d: %s is undefined\n", actualToken->lineNumber, actualToken->lexeme);
            }
            parse_command_helper(cur_entry);
            break;
        }
            
        default:
        {
            errorRecovery(first,follow,5,1,t);
            break;
        }
        
    } //switch case
}

void  parse_command_helper(SymbolEntry *entry)
{
    eTOKENS first[3]={TOKEN_S_BRACKETS_LEFT, TOKEN_POINTER_INDECATOR, TOKEN_ASSIGNMENT};
    eTOKENS follow[1]={TOKEN_SEMICOLON};
    
    
    Token *t= next_token();
    switch(t->kind)
    {
       
        case TOKEN_S_BRACKETS_LEFT:
        {
            fprintf(yyout,"COMMAND_HELPER ---> [EXPRESSION] = EXPRESSION\n");
            //Semantic checking if type of id that came inside of SymbolEntry *entry, is Array
            if( (get_id_type(entry))!= Array) //Id type is not array
            {
                fprintf(semantic_yyout,"Semantic ERROR in line %d: type of Id is not an Array\n", t->lineNumber);
            }
            
            type=parse_expression(entry);
            if(type!=Integer)
            {
                fprintf(semantic_yyout,"Semantic ERROR in line %d: Index in array must be Integer!\n",t->lineNumber);
            }
            match(TOKEN_S_BRACKETS_RIGHT,first,follow,3,1);
            match(TOKEN_ASSIGNMENT,first,follow,3,1);
            type=parse_expression(entry);
            
            //If subType of the array is not match to the right type
            //We will check sub types only if ID is really an array
            if( entry && (get_id_type(entry)==Array) )
            {
                //DEBUG ONLY
                //printf("DEBUG:Enter a danger place\n");
                
                if( !((entry->subType==Integer && type==Integer) || (entry->subType==Real && type==Real)) )
                {
                    fprintf(semantic_yyout,"Semantic ERROR in line %d: Assignment to a different subType\n",t->lineNumber );
                }
            }
            break;
        }
        case TOKEN_POINTER_INDECATOR:
        {
            fprintf(yyout,"COMMAND_HELPER ---> ^ = ESPRESSION\n");
            //Semantic error: id type should be here Pointer
            if( (get_id_type(entry)) != Pointer )
            {
                fprintf(semantic_yyout,"Semantic ERROR in line %d: Id is not a Pointer\n", t->lineNumber);
            }
            match(TOKEN_ASSIGNMENT,first,follow,3,1);
            type=parse_expression(entry);

            //Semantic error: id subtype is not equal to right type
            if(entry && (get_id_type(entry)) == Pointer)
            {
                //DEBUG ONLY
                //printf("DEBUG:Enter a danger place\n");
                
                if( !((entry->subType==Integer && type==Integer) || (entry->subType==Real && type==Real)) )
                {
                    fprintf(semantic_yyout,"Semantic ERROR in line %d: subType of Pointer %s is not %s\n", t->lineNumber, entry->name, checkType(type) );
                }
            }
            break;
        }
        case TOKEN_ASSIGNMENT:
        {
            fprintf(yyout,"COMMAND_HELPER ---> = Z\n");
            type=parse_Z(entry);
            //If id type is not equal to Z type -> Semantic Error
            if( get_id_type(entry) != type )
            {
                fprintf(semantic_yyout,"Semantic ERROR in line %d: miss match types comparison\n", t->lineNumber);
            }
            break;
        }
        default:
        {
            errorRecovery(first,follow,3,1,t);
            break;
        }
    }//switch
}

Type parse_Z(SymbolEntry *entry)
{
    Type type1;
    
    eTOKENS first[6] ={INTEGER_NUMBER, REAL_NUMBER, TOKEN_ID, TOKEN_ADDRES, KEY_WORD_SIZE_OF, KEY_WORD_MALLOC};
    eTOKENS follow[1]={TOKEN_SEMICOLON};
    
    Token *t=next_token();
    Token *actualToken;
    switch(t->kind)
    {
        case KEY_WORD_MALLOC:
        {
            fprintf(yyout, "Z ---> malloc(size_of(type_name))\n");
            type=get_id_type(entry);
            
            match(TOKEN_R_BRACKETS_LEFT,first,follow,6,1);
            match(KEY_WORD_SIZE_OF,first,follow,6,1);
            match(TOKEN_R_BRACKETS_LEFT,first,follow,6,1);
            match(TOKEN_ID,first,follow,6,1);
            
            actualToken=actual_token();
            //If id is undefined
            if( !(find(cur_table, actualToken->lexeme)) )
            {
                fprintf(semantic_yyout,"Semantic ERROR in line %d: %s is Undefined\n",actualToken->lineNumber, actualToken->lexeme);
            }
            match(TOKEN_R_BRACKETS_RIGHT,first,follow,6,1);
            match(TOKEN_R_BRACKETS_RIGHT,first,follow,6,1);
            //If id is not a pointer - ERROR ! can allocate space for variable that is not Pointer!
            if(type != Pointer)
            {
                fprintf(semantic_yyout,"Semantic ERROR in line %d: Id is not a Pointer\n", t->lineNumber);
                return Undefined;
            }
            else
            {
                return Pointer;
            }
            
            break;
        }
        case INTEGER_NUMBER:
        case REAL_NUMBER:
        case TOKEN_ID:
        case TOKEN_ADDRES:
        case KEY_WORD_SIZE_OF:
            
        {
            fprintf(yyout,"Z ---> EXPRESSION\n");
            t=back_token();
            return (parse_expression(entry));
            break;
        }
        default:
        {
            errorRecovery(first,follow,6,1,t);
            return Undefined;
            break;
        }
            
    }
}



//This parse is get entry only for the case of TOKEN_ADDRES!!!
Type parse_expression(SymbolEntry *entry)
{
    SymbolEntry *cur_entry;
    SymbolEntry *tempEntry;
    eTOKENS first[5] ={INTEGER_NUMBER, REAL_NUMBER, TOKEN_ID, TOKEN_ADDRES, KEY_WORD_SIZE_OF};
    eTOKENS follow[9] ={TOKEN_SEMICOLON,TOKEN_IS_EQUAL,TOKEN_BIG_EQUAL,TOKEN_LOW_EQUAL,TOKEN_BIGGER,TOKEN_LOWER,TOKEN_INEQUAL,TOKEN_R_BRACKETS_RIGHT,TOKEN_S_BRACKETS_RIGHT};
   
    Token *t = next_token();
    Token *actualToken;
    switch(t->kind)
    {
        case INTEGER_NUMBER:
        {
            fprintf(yyout,"EXPRESSION ---> int_num\n");
            return Integer;
            break;
        }
        case REAL_NUMBER:
        {
            fprintf(yyout,"EXPRESSION ---> real_num\n");
            return Real;
            break;
        }
        case TOKEN_ID:
        {
            
            fprintf(yyout,"EXPRESSION ---> id F\n");
            actualToken=actual_token();
            cur_entry=find(cur_table, actualToken->lexeme);
            
            //If the id that came before F is undefined
            if(!cur_entry)
            {
               
                fprintf(semantic_yyout,"Semantic ERROR in line %d: %s is Undefined\n", t->lineNumber, actualToken->lexeme);
            }
            
            return (parse_F(cur_entry));
            break;
        }
        case TOKEN_ADDRES:
        {
            fprintf(yyout,"EXPRESSION ---> &id\n");
            match(TOKEN_ID,first,follow,5,9);
            
            actualToken=actual_token();
            cur_entry=find(cur_table, actualToken->lexeme);
            if(cur_entry) //Id is defined
            {
                if(get_id_type(entry)==Pointer)
                {
                    //printf("DEBUG: enter danger place\n");
                    entry->subType=get_id_type(cur_entry);
                }
                return Pointer;
                
            }
            else //Id is undefinded
            {
                fprintf(semantic_yyout,"Semantic error in line %d: %s is undefined\n", actualToken->lineNumber, actualToken->lexeme);
                
                return Undefined;
            }
            
            break;
        }
            
        case KEY_WORD_SIZE_OF:
        {
            fprintf(yyout,"EXPRESSION ---> size_of(type_name)\n");
            match(TOKEN_R_BRACKETS_LEFT,first,follow,5,9);
            match(TOKEN_ID,first,follow,5,9);
            actualToken=actual_token();
            
            cur_entry=find(cur_table,actualToken->lexeme);
            if(!cur_entry) //id is undefined
            {
                fprintf(semantic_yyout,"Semantic ERROR in line %d: %s is undefined\n", actualToken->lineNumber, actualToken->lexeme);
            }
            else //id is defined
            {
                type=get_id_type(cur_entry);
                match(TOKEN_R_BRACKETS_RIGHT,first,follow,5,9);
                return type;
                
            }
            match(TOKEN_R_BRACKETS_RIGHT,first,follow,5,9);
            return Undefined;
            break;
        }
        default:
        {   //Check if t kind != follow of EXPRESSION and not EOF
            errorRecovery(first,follow,5,9,t);
            return Undefined;
            break;
        }
            
    }
}
Type parse_F(SymbolEntry *entry)
{
    Type typeLeft, typeRight;
    eTOKENS first[7]={TOKEN_S_BRACKETS_LEFT,TOKEN_PLUS,TOKEN_MINUS,TOKEN_MULT,TOKEN_DIV,TOKEN_POWER,TOKEN_POINTER_INDECATOR};
    
    eTOKENS follow[9] ={TOKEN_SEMICOLON,TOKEN_IS_EQUAL,TOKEN_BIG_EQUAL,TOKEN_LOW_EQUAL,TOKEN_BIGGER,TOKEN_LOWER,TOKEN_INEQUAL,TOKEN_R_BRACKETS_RIGHT,TOKEN_S_BRACKETS_RIGHT};
    
    Token *t = next_token();
    switch(t->kind)
    {
        case TOKEN_S_BRACKETS_LEFT:
        {
            fprintf(yyout,"F ---> [EXPRESSION]\n");
            
            type=get_id_type(entry);
            
            if(type == Array)
            {
                //Maybe try to send here null pointer!
                type=parse_expression(entry);
              
                if(type == Integer)
                {
                    match(TOKEN_S_BRACKETS_RIGHT,first,follow,7,9);
                    return (entry->subType);
                }
                else //type is not integer and its error
                {
                    fprintf(semantic_yyout,"Semantic ERROR in line %d: Array index is not Integer\n", t->lineNumber);
                    match(TOKEN_S_BRACKETS_RIGHT,first,follow,7,9);
                    return Undefined;
                }
            }
            else //type is not array and it is an error
            {
                if(entry)
                {
                    //debug!!!@#!@#!@#@!#
                    //printf("DEBUG: danger place!\n");
                fprintf(semantic_yyout,"Semantic ERROR in line %d: %s have to be Array\n", t->lineNumber, entry->name);
                }
                
                type=parse_expression(entry);
                match(TOKEN_S_BRACKETS_RIGHT,first,follow,7,9);
                return Undefined;
            }
            break;
        }
        case TOKEN_PLUS:
        case TOKEN_MINUS:
        case TOKEN_MULT:
        case TOKEN_DIV:
        case TOKEN_POWER:
        {
            fprintf(yyout,"F ---> ar_op EXPRESSION\n");
            
            typeLeft=get_id_type(entry);
            typeRight=parse_expression(entry);
            if(typeLeft==Undefined || typeRight==Undefined)
            {
                return Undefined;
            }
            else
            {
                if(( typeLeft==Integer && typeRight==Integer) || (typeLeft==Real && typeRight==Real))
                {
                    return typeLeft; //could be alsot typeRight
                }
                else
                {
                    fprintf(semantic_yyout,"Semantic ERROR in line %d: left variable and right variable is not type match\n", t->lineNumber);
                    return Undefined;
                }
            }
            break;
        }
        case TOKEN_POINTER_INDECATOR:
        {
            if(entry)
            {
                fprintf(yyout,"F ---> ^\n");
                type=get_id_type(entry);
                if(type==Pointer)
                {//Think should return the subType of the entry
                    return (entry->subType);
                }
                else
                {
                    fprintf(semantic_yyout,"Semantic error in line %d: Id is not Pointer\n", t->lineNumber);
                    return Undefined;
                }
            }
            else
            return Undefined;
            break;
        }

        case TOKEN_SEMICOLON:
        case TOKEN_R_BRACKETS_RIGHT:
        case TOKEN_S_BRACKETS_RIGHT:
        case TOKEN_IS_EQUAL:
        case TOKEN_BIG_EQUAL:
        case TOKEN_LOW_EQUAL:
        case TOKEN_BIGGER:
        case TOKEN_LOWER:
        case TOKEN_INEQUAL:
        {
            
            fprintf(yyout,"F ---> EPSILON\n");
            type = get_id_type(entry);
            t=back_token();
            return type;
            break;
        }
    
        default:
        {
            errorRecovery(first,follow,7,9,t);
            break;
        }
            
    } //switch
    return Undefined;
}



//Language
/*
 PROGRAM->BLOCK
 BLOCK -> block DEFINITIONS;begin COMMANDS;end
 DEFINITIONS -> DEFINITION A
 A -> ; DEFINITION A | EPSILON
 DEFINITION -> VAR_DEFINITION | TYPE_DEFINITION
 VAR_DEFINITION -> id:B
 B -> BASIC_TYPE|type_name
 TYPE_DEFINITION -> type type_name is TYPE_INDICATOR
 TYPE_INDICATOR -> BASIC_TYPE | ARRAY_TYPE | POINTR_TYPE
 BASIC_TYPE -> integer | real
 ARRAY_TYPE -> array[SIZE] of BASIC_TYPE
 POINTER_TYPE -> ^ B
 SIZE -> int_num
 COMMAND -> COMMAND D
 D -> ;COMMAND | EPSILON
 COMMAND -> when(EXPRESSION rel_op EXPRESSION)do COMMANDS;default COMMAND; end_when|
            for(id=EXPRESSION;id rel_op EXPRESSION;id++)COMMANDS;end_for|
            free(id)|BLOCK|id COMMAND_HELPER
 COMMAND_HELPER -> [EXPRESSION]=EXPRESSION | ^=EXPRESSION|=Z
 Z -> EXPRESSION | malloc(size_of(type_name))
 EXPRESSION -> int_num | real_num | id F | &id | size_of(type_name)
 F -> [EXPRESSION] | ar_op ESPRESSION | ^ | EPSILON
 */
