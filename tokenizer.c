/*
 * tokenizer.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



//states of different types
enum states{
  decimal, //0
  string, //1
  octal, //2
  hex, //3
  digit,//4
  operator,//5
  empty,//6
  decimale, //7
  endoftok, //8
  escape, //9
  endescape, //10
  decimalep,
  eop,
  illegal
};



/*
 *Struct TokenizerT will hold the whole input argument along with
 *the position the tokenizer is at and the length of the input arg
 */

struct TokenizerT_{
  char *inputString;
  int pos, len, state;
};
/*
 *CurrToken will hold the current token and the state, which is a 
 *small portion of the original string (in TokenizerT
 */
struct CurrToken_{
  char *token;
  int currState;
};



typedef struct TokenizerT_ TokenizerT;
typedef struct CurrToken_ CurrToken;

/*
 * TKCreate creates a new TokenizerT object for a given token stream
 * (given as a string).
 * 
 * TKCreate should copy the arguments so that it is not dependent on
 * them staying immutable after returning.  (In the future, this may change
 * to increase efficiency.)
 *
 * If the function succeeds, it returns a non-NULL TokenizerT.
 * Else it returns NULL.
 *
 * You need to fill in this function as part of your implementation.
 */
TokenizerT *TKCreate( char * ts ) {

  TokenizerT* tokenizer = (TokenizerT*)malloc(sizeof(TokenizerT));

  tokenizer->inputString = ts;
  tokenizer->len = strlen(ts);
  tokenizer->pos = 0;
  tokenizer->state = 6;
  return tokenizer;
}


/*
 *Construct is a basic constructor method for the CurrToken struct.
 *It takes no arguments and returns a struct pointer to an empty
 *initialized struct.
 */
CurrToken *Construct () {
  CurrToken * temp = (CurrToken *) malloc(sizeof(CurrToken));
  temp->token = NULL;
  temp->currState = empty;
  return temp;
}


/*
 *Will take an input as a character and return the enum (int)
 *value that corresponds to it's data type. Own interpretation
 *of ctype.h
 */
int dataType(char data) {
    
    //type cast from char to int to get ascii value
    int ascii = (int) data;
    //char temp[2] = {data, '\0'};

    //if we know that it is a number
    if (ascii >= 48 && ascii <= 57){
      return digit;
    }
    if(data=='e'||data=='E')
      {
	return eop;
      }
    //if we know that it is either upper or lower case letter
    else if ((data >= 'a' && data <= 'z') || (data >= 'A' && data <= 'Z')){
      return string;
    }
    else if (data == ' ' || data == '\0') {
      return endescape;
    }
    else if (data =='\\'||data == '\t' || data == '\r' || data == '\f' || data == '\n' || data == '\v'){
      return escape;
    }
    else{
      return illegal;
    }
    return empty;
}


/*
 * TKDestroy destroys a TokenizerT object.  It should free all dynamically
 * allocated memory that is part of the object being destroyed.
 *
 * You need to fill in this function as part of your implementation.
 */
void TKDestroy( TokenizerT * tk ) {
  free(tk);
}


/*
 *travere the tokenizer through the tree, where it will print out the 
 *identifier with the string
 */
int isLegal(char *token, TokenizerT *tk, int nextData, char next){
  char lastchar;

  //printf("token: %s\n nextData: %d\n next: %c\n",token,nextData,next);
   
  if(token!=NULL){
     lastchar = token[strlen(token)-1];       
    }

  switch(tk->state)
    {
      //if current state is empty (waiting for floats or operators or escape chars
    case empty:
      if(next == '0'){
	return octal;
      }
      else if(next == '0')
	{
	  return decimal;
	}
      switch (nextData) {
      case digit:
	return digit;
      case string: 
	return string;
      case eop:
	return string;
      case endescape:
	return endescape;
      case illegal:
	return illegal;
      default:
	return empty;
      }
      
  //end of token
    case endoftok:
      return endoftok;
  
  //octal
    case octal:
      if(strlen(token) == 1)
	if(next =='x' || next =='X')
	  return hex;
      if(next<'8' && next>='0')
	return octal;
      if(next>='8' && next <= '9')
	return digit;
      if(nextData==string)
	return endoftok;
      if(next =='.')
	return decimal;
      else
	return endoftok;
    case illegal:
      return empty;
      
  //digit
    case digit:
      if(nextData == digit)
	return digit;
      else if(next == '.')
	return decimal;
      else
	return endoftok;
  
  //string
    case string:
      return endoftok;

  //escape
    case escape:
      return endescape;
            
  //decimal
    case decimal:
      if(lastchar =='.' && nextData != digit){
	return empty;
      }
    if(nextData == digit)
      return decimal;
    else if(nextData == eop)
      return decimale;
    else
      {
	return endoftok;
      }

  //decimale
    case decimale:
    if(next == '+' || next =='-')
      return decimalep;
    if(nextData == digit)
      return decimalep;
    else
      return empty;

    case decimalep:
      if(nextData == digit)
	return decimalep;
      if(lastchar == '+' || lastchar =='-') {
	if(nextData!=digit)
	  return empty;
      }
      else
	return endoftok;      
      
      //hex
    case hex:
      //else if(tk->state == hex){
      if(nextData == digit)
	return hex;
      if((next >= 'a' && next<='f')|| (next>='A'&& next<='F'))
	return hex;
      if(lastchar == 'x' || lastchar =='X')
	{
	  if(nextData == escape)
	    {
	      return empty;
	    }
	}
      else
	return endoftok;



    }
  return empty;
}


void printToken(CurrToken * token)
{
  int len = strlen(token->token);
  char i [len];
  strcpy(i, token->token);
  char temp = i[0];
  //  printf("FINAL:  ");
  switch (token->currState){
  case 0:
    printf("Float: \"%s\"\n",token->token);
    break;
  case 1:
    printf("[0x%02x]\n",temp);
    break;
  case 2:
    /*
    if((i[0]>='a'&&i[0]<='z')||(i[0]>='A'&&i[0]<='Z'))
      {
	break;
      }
*/
    if(strcmp(token->token,"0")==0)
      {
	printf("Zero\n");
	break;
      }
    printf("Octal: \"%s\"\n", token->token);
    break;
  case 3:
    printf("Hexadecimal: \"%s\"\n", token->token);
    break;
  case 4:
    printf("Decimal: \"%s\"\n", token->token);
    break;
  case 6:
    printf("Invalid: \"%s\"\n", token->token);
    break;
  case 7:
    printf("Float: \"%s\"\n", token->token);
  case 11:
    printf("Float: \"%s\"\n",token->token);
  default:
    //printf("%d\t%s\n", token->currState, token->token);
    break;
  }
}


/*
 * TKGetNextToken returns the next token from the token stream as a
 * character string.  Space for the returned token should be dynamically
 * allocated.  The caller is responsible for freeing the space once it is
 * no longer needed.
 *
 * If the function succeeds, it returns a C string (delimited by '\0')
 * containing the token.  Else it returns 0.
 *
 * You need to fill in this function as part of your implementation.
 */
char *TKGetNextToken( TokenizerT * tk ) {

    int type;
    char * nextToken = (char *) malloc(sizeof(char)*256);
    strcpy(nextToken, "");
    //char local[100];
    char next;

    while (tk->pos <= tk->len) { 
      next = tk->inputString[tk->pos];
      //call function to see what kind of data type next char is
      type = dataType(next);
      
      //call isLegal function to determine if this is a legal token
      int legal =isLegal (nextToken, tk, type, next);
      
      //if it's an escape character other than a space
      if (type == escape){
	char temp = tk->inputString[tk->pos+1];
	if (temp == 't' || temp == 'r' || temp == 'f' || temp == 'n' || temp == 'v'){
	  if (strcmp(nextToken, "") != 0)
	    return nextToken;
	  else{
	    tk->pos+=2;
	    break; //to return 0
	  }
	}
	else{
	  strcpy(nextToken, "\\");
	  tk->pos++;
	  return nextToken;
	}
	if (tk->pos >= tk->len) //no more in the token stream
	  break;
	continue; //keep looping, token not done
      }
      
      //if not legal token, print CurrToken, continue (do not advance tk->pos)
      if (legal == empty && strcmp(nextToken, "") != 0){
	//else print bad token
	printf("Invalid: \"%s\"\n", nextToken);
	//tk->pos++;
	return 0;
	
	if (tk->pos == tk->len) //no more in token stream
	  break;
	continue; //keep looping, token not done
      }
      
      //if it's a space
      if (legal == endescape) {
	if (strcmp(nextToken, "") != 0) {
	  tk->pos++;
	  return nextToken;
	}
	tk->pos++;
	if (tk->pos == tk->len) //no more in token stream
	  break;
	
	continue; //keep looping, token not done
      }
      
      //if we are at the end of a token
      if (legal == endoftok) {
	if (type == endoftok || endescape)
	  //tk->pos++;
	return nextToken;

	if (tk->pos == tk->len) //no more in token stream
	  break;	
	continue; //keep looping, token not done
      }    

      
      //only assign token->currState after checks for bad token or endoftok
      tk->state = legal;
     

      //else if legal token, append char to CurrToken, advance tk->pos
      char local[2] = {next, '\0'};
      if (strcmp(nextToken, "") == 0)
	strcpy(nextToken, local);
      else
	  strcat(nextToken, local);

      //printf("token data: %s %d\n", nextToken, tk->state);
      tk->pos++;
    }
    return 0;
}

/*
 * main will have a string argument (in argv[1]).
 * The string argument contains the tokens.
 * Print out the tokens in the second string in left-to-right order.
 * Each token should be printed on a separate line.
 */

int main(int argc, char **argv) {    
  if (argc != 2) {
    printf("invalid input\n");
    return 0;
  }
  TokenizerT* ptr = TKCreate(argv[1]);
  
  //create currToken with initial string "" and state EMPTY
  CurrToken *token = Construct();

  int loop = 0;
  while (loop < ptr->len) {
    token->token = TKGetNextToken(ptr);
    token->currState = ptr->state;
    loop = ptr->pos;
    if (token->token != 0)
      printToken(token);
    free(token->token);
    ptr->state = empty;
  }
  TKDestroy(ptr);
  free(token);
  return 0;
}
