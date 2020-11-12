/* ========= expressions.h =========
 * Project: IFJ 2020/21 project
 * Team: 067, variant I
 * Author: Emma Krompaščíková (xkromp00), Samuel Olekšák (xoleks00)
 * Date: November 2020
 * Description: Precedence syntactic analysis for expressions using shunting-yard algorithm
 * ================================= */

#define MAX_STACK_LENGTH 50

typedef struct TokenStack {
    Token tokens[MAX_STACK_LENGTH];
    int count;
} TokenStack;

/*
typedef struct ExpressionNode {
    Token token;
    ExpressionNode* lPtr;
    ExpressionNode* rPtr;    
} ExpressionNode;
*/

/* From tokens array in infix notation creates postfix notation */
bool handleExpression(Token *overlapTokenIn, Token *overlapTokenOut);

/* Verify output queue syntax */
bool verifyOutput(TokenStack *outputQueue);

/* */
bool TokenStackCollapse(TokenStack *stack, int pos);

/* Pushes one token into supplied stack */
void TokenStackPush(TokenStack *stack, Token token);

/* Returns token on the top of the stack */
Token TokenStackTop(TokenStack *stack);

/* Returns token on the top of the stack and removes it from the stack */
Token TokenStackPop(TokenStack *stack);

/* Debug function to print out all the items from the stack */
void TokenStackPrint(TokenStack *stack);

/* Returns priority for that token (1,2,3) */
int getPriority(tokenType type);

/* Returns true if a token is valid for expression */
bool isValidExpToken(tokenType type);

bool isOperator(tokenType type);
