/* ========= expressions.h =========
 * Project: IFJ 2020/21 project
 * Team: 067, variant I
 * Author: Emma Krompaščíková (xkromp00), Samuel Olekšák (xoleks00)
 * Date: November 2020
 * Description: Precedence syntactic analysis for expressions using shunting-yard algorithm
 * ================================= */

#define INITIAL_BUFFER_LENGTH 10

typedef struct TokenBuffer {
    int count;
    int capacity;
    Token *tokens; // flexible array member
} TokenBuffer;

/* From tokens array in infix notation creates postfix notation */
bool handleExpression(Token *overlapTokenIn, Token *overlapTokenOut);

/* Verify if output queue syntax is correct */
bool verifyOutput(TokenBuffer *outputQueue);

/* Allocates, initates and returns a token buffer */
TokenBuffer* TokenBufferCreate();

/* If possible collapse items inside buffer on pos, pos-1 and pos-2 into one item */
bool TokenBufferCollapse(TokenBuffer *buffer, int pos);

/* Pushes one token into supplied buffer */
void TokenBufferPush(TokenBuffer *buffer, Token token);

/* Returns token on the top of the buffer */
Token TokenBufferTop(TokenBuffer *buffer);

/* Returns token on the top of the buffer and removes it from the buffer */
Token TokenBufferPop(TokenBuffer *buffer);

/* Debug function to print out all the items from the buffer */
void TokenBufferPrint(TokenBuffer *buffer);

/* Deallocated token buffer */
void TokenBufferDispose(TokenBuffer *buffer);

/* Returns priority for that token (1,2,3) */
int getPriority(tokenType type);

/* Returns true if token type is valid for expression */
bool isValidExpToken(tokenType type);

/* Returns true if token type is operator (+,-,*,/,<,<=,>,>=,==,!=) */
bool isOperator(tokenType type);
