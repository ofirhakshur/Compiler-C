#include "Token.h"


/*This var "currentIndex is soppused to be current index for every token array in each node*/
int currentIndex;// = 0;  //was changed, it should be equal to zero at this line in original version!
Node* currentNode = NULL;

int line;        //Var that helping to know what line the token called from -initialize at main to 1


#define TOKEN_ARRAY_SIZE 100


/*
* This function creates a token and stores it in the storage.
*/
void create_and_store_token(eTOKENS kind, char* lexeme, int numOfLine)
{ 
	// case 1: there is still no tokens in the storage.
	if (currentNode == NULL)
	{
        
		currentNode = (Node*)malloc(sizeof(Node));
        
		if(currentNode == NULL)
		{
			fprintf(yyout,"\nUnable to allocate memory! \n"); 
			exit(0);
		}
		currentNode->tokensArray = (Token*)malloc(sizeof(Token)*TOKEN_ARRAY_SIZE);
		if(currentNode->tokensArray == NULL)
		{
			fprintf(yyout,"\nUnable to allocate memory! \n"); 
			exit(0);
		}
		currentNode->prev = NULL;
		currentNode->next = NULL;
        currentNode->arrayLogicSize = 1;  //Initiallize logic array size
        currentNode->counter=0;
	}

	// case 2: at least one token exsits in the storage.
	else
	{
		// the array (the current node) is full, need to allocate a new node
		if (currentIndex == TOKEN_ARRAY_SIZE - 1)
		{
			currentIndex = 0;
			currentNode->next = (Node*)malloc(sizeof(Node));

			if(currentNode == NULL)
			{
				fprintf(yyout,"\nUnable to allocate memory! \n"); 
				exit(0);
			}
			currentNode->next->prev = currentNode;
			currentNode = currentNode->next;
			currentNode->tokensArray = (Token*)malloc(sizeof(Token)*TOKEN_ARRAY_SIZE);
            
            currentNode->counter=0;
            
			if(currentNode->tokensArray == NULL)
			{
				fprintf(yyout,"\nUnable to allocate memory! \n"); 
				exit(0);
			}
			currentNode->next = NULL;
            currentNode->arrayLogicSize = 1; //Initialiize logic array size of a new array of a new node link
		}

		// the array (the current node) is not full
		else
		{
			currentIndex++;
            currentNode->arrayLogicSize++; //I add this, val up of logic size array
		}
	}
    
	currentNode->tokensArray[currentIndex].kind = kind;
	currentNode->tokensArray[currentIndex].lexeme = (char*)malloc(sizeof(char)*(strlen(lexeme)+1));
	strcpy(currentNode->tokensArray[currentIndex].lexeme, lexeme);
	currentNode->tokensArray[currentIndex].lineNumber = numOfLine;
   
}



/*
* This function returns the token in the storage that is stored immediately before the current token (if exsits).
*/

Token *back_token()
{
    Token *res;
    if(currentNode->counter <= 0)
        currentNode=currentNode->prev;
    
    currentNode->counter--;
    res=&(currentNode->tokensArray[currentNode->counter]);
    
    return res;
    
}

/*
* If the next token already exists in the storage (this happens when back_token was called before this call to next_token): 
*  this function returns the next stored token.
* Else: continues to read the input file in order to identify, create and store a new token (using yylex function);
*  returns the token that was created.
*/

Token *next_token()
{
    Token* res;
    if(currentNode->counter > 99)   //if all tokens were poped
        currentNode=currentNode->next;  //go to next node
    
    res=&(currentNode->tokensArray[currentNode->counter]);
    currentNode->counter++;
    //currentToken=res;   //for helping!
    return res;
    //If current node is null - should exit from this function
}

//Add to help for stage 3 in the project
//This function get the actual token before moving the counter index
Token *actual_token()
{
    return (&(currentNode->tokensArray[((currentNode->counter)-1)]));
}

//Need to add remove char* from each token
void freeList()
{
    int i;
    Node* del;
    while(currentNode)
    {
        del=currentNode;
        //@@@@@@@@@
        for(i=0;i<currentNode->arrayLogicSize; i++)
            free(currentNode->tokensArray[i].lexeme);
        //@@@@@@@@@
        free(del->tokensArray);
        currentNode=currentNode->next;
        free(del);
    }
}
//function get enum token and return string
char* getTokenNameByNum(eTOKENS kind)
{
    switch (kind)
    {
        case (INTEGER_NUMBER ):
            return ("INTEGER_NUMBER");
        case (REAL_NUMBER ):
            return ("REAL_NUMBER");
        case (KEY_WORD_BLOCK):
            return ("KEY_WORD_BLOCK");
        case (KEY_WORD_BEGIN):
            return ("KEY_WORD_BEGIN");
        case (KEY_WORD_END):
            return ("KEY_WORD_END");
        case (KEY_WORD_TYPE):
            return ("KEY_WORD_TYPE");
        case (KEY_WORD_INTEGER):
            return ("KEY_WORD_INTEGER");
        case (KEY_WORD_REAL):
            return ("KEY_WORD_REAL");
        case (KEY_WORD_ARRAY):
            return ("KEY_WORD_ARRAY");
        case (KEY_WORD_OF):
            return ("KEY_WORD_OF");
        case (KEY_WORD_WHEN):
            return ("KEY_WORD_WHEN");
        case (KEY_WORD_DO):
            return ("KEY_WORD_DO");
        case (KEY_WORD_DEFAULT):
            return ("KEY_WORD_DEFAULT");
        case (KEY_WORD_END_WHEN):
            return ("KEY_WORD_END_WHEN");
        case (KEY_WORD_FOR):
            return ("KEY_WORD_FOR");
        case (KEY_WORD_END_FOR):
            return ("KEY_WORD_END_FOR");
        case (KEY_WORD_MALLOC):
            return ("KEY_WORD_MALLOC");
        case (KEY_WORD_SIZE_OF):
            return ("KEY_WORD_SIZE_OF");
        case (KEY_WORD_FREE):
            return ("KEY_WORD_FREE");
        case (TOKEN_ID):
            return ("TOKEN_ID");
        case (TOKEN_PLUS):
            return ("TOKEN_PLUS");
        case (TOKEN_MINUS):
            return ("TOKEN_MINUS");
        case (TOKEN_MULT):
            return ("TOKEN_MULT");
        case (TOKEN_DIV):
            return ("TOKEN_DIV");
        case (TOKEN_POWER):
            return ("TOKEN_POWER");
        case (TOKEN_VAL_UP):
            return ("TOKEN_VAL_UP");
        case (TOKEN_REL_OP):
            return ("TOKEN_REL_OP");
        case (TOKEN_IS_EQUAL):
            return ("TOKEN_IS_EQUAL");
        case (TOKEN_BIG_EQUAL):
            return ("TOKEN_BIG_EQUAL");
        case (TOKEN_LOW_EQUAL):
            return ("TOKEN_LOW_EQUAL");
        case (TOKEN_BIGGER):
            return ("TOKEN_BIGGER");
        case (TOKEN_LOWER):
            return ("TOKEN_LOWER");
        case (TOKEN_INEQUAL):
            return ("TOKEN_INEQUAL");
        case (TOKEN_ASSIGNMENT):
            return ("TOKEN_ASSIGNMENT");
        case (TOKEN_POINTER_INDECATOR):
            return ("TOKEN_POINTER_INDECATOR");
        case (TOKEN_ADDRES):
            return ("TOKEN_ADDRES");
        case (TOKEN_COMMA):
            return ("TOKEN_COMMA");
        case (TOKEN_SEMICOLON):
            return ("TOKEN_SEMICOLON");
        case (TOKEN_R_BRACKETS_LEFT):
            return ("TOKEN_R_BRACKETS_LEFT");
        case (TOKEN_R_BRACKETS_RIGHT):
            return ("TOKEN_R_BRACKETS_RIGHT");
        case (TOKEN_S_BRACKETS_LEFT):
            return ("TOKEN_S_BRACKETS_LEFT");
        case (TOKEN_S_BRACKETS_RIGHT):
            return ("TOKEN_S_BRACKETS_RIGHT");
        case (TOKEN_COLON):
            return ("TOKEN_COLON");
        case (KEY_WORD_IS):
            return ("KEY_WORD_IS");
        case (TOKEN_EOF):
            return ("TOKEN_EOF");
    }
}

