/* ========= expressions.h =========
 * Project: IFJ 2020/21 project
 * Team: 067, variant I
 * Author: Emma Krompaščíková (xkromp00), Samuel Olekšák (xoleks00)
 * Date: November 2020
 * Description: Precedence syntactic analysis for expressions using shunting-yard algorithm
 * ================================= */

/* From tokens array in infix notation creates postfix notation */
bool handleExpression(Token overlapTokenIn, Token *overlapTokenOut);

/* Verify if output queue syntax is correct */
bool verifyOutput(TokenBuffer *outputQueue);

/* Returns priority for that token (1,2,3) */
int getPriority(tokenType type);

/* Returns true if token type is valid for expression */
bool isValidExpToken(tokenType type);

/* Returns true if token type is operator (+,-,*,/,<,<=,>,>=,==,!=) */
bool isOperator(tokenType type);
