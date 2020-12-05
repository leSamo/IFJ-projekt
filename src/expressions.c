/* ========= expressions.c =========
 * Project: IFJ 2020/21 project
 * Team: 067, variant I
 * Author: Emma Krompaščíková (xkromp00), Samuel Olekšák (xoleks00)
 * Date: November 2020
 * Description: Expression syntactic analysis with postfix conversion using shunting-yard algorithm
 * ================================= */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "constants.h"
#include "tokenBuffer.c"
#include "expressions.h"
#include "leScanner.h"
#include "AST.c"

typedef enum {
    SHIFTT = 1, // <
    REDUCE = 2, // >
    EQUALL = 3, // =
    ERRORR = -1
} PAT_Element;

typedef enum {
    Plus = 0,
    Minus = 0,
    Multiply = 1,
    Divide = 1,
    LeftBracket = 2,
    RightBracket = 3,
    Identifier = 4,
    GreaterThan = 5,
    LessThan = 5,
    GreaterEqualThan = 5,
    LessEqualThan = 5,
    Equal = 5,
    NotEqual = 5,
    EOLT = 6
} PAT_Header;

PAT_Element PA_Table[7][7] = {
    /* +-      * /      (       )      id      rel      $  */
    {REDUCE, SHIFTT, SHIFTT, REDUCE, SHIFTT, REDUCE, REDUCE},  // +-
    {REDUCE, REDUCE, SHIFTT, REDUCE, SHIFTT, REDUCE, REDUCE},  // /*
    {SHIFTT, SHIFTT, SHIFTT, EQUALL, SHIFTT, SHIFTT, ERRORR},  // (
    {REDUCE, REDUCE, ERRORR, REDUCE, ERRORR, REDUCE, REDUCE},  // )
    {REDUCE, REDUCE, ERRORR, REDUCE, ERRORR, REDUCE, REDUCE},  // id
    {SHIFTT, SHIFTT, SHIFTT, REDUCE, SHIFTT, ERRORR, REDUCE},  // rel
    {SHIFTT, SHIFTT, SHIFTT, ERRORR, SHIFTT, SHIFTT, ERRORR}}; // $

PAT_Header PAT_GetFromTok(Token token) {
    switch (token.type) {
    case TOK_Add:
    case TOK_Sub:
        return Plus;
        break;
    case TOK_Mul:
    case TOK_Div:
        return Multiply;
        break;
    case TOK_R_Paren:
        return RightBracket;
        break;
    case TOK_L_Paren:
        return LeftBracket;
        break;

    case TOK_Identifier:
    case TOK_Int_Literal:
    case TOK_String_Literal:
    case TOK_Float_Literal:
        return Identifier;
        break;
    case TOK_More_Then:
    case TOK_More_Equal_Then:
    case TOK_Less_Then:
    case TOK_Less_Equal_Then:
    case TOK_Equal:
    case TOK_Not_Equal:
        return GreaterThan;
        break;
    case TOK_Newline:
    default:
        return EOLT;
        break;
    }
}

PAT_Element PAT_GetSign(PAT_Header Row, PAT_Header Column) {
    return PA_Table[Row][Column];
}

bool handleExpression(ASTNode *expRoot, Token overlapTokenIn) {
    // buffer initialization
    TokenBuffer *Stack = TokenBufferCreate();
    printf("\n================\n\n");

    // insert $ as bottom of stack
    Token tmp;
    Token tmpp;
    tmp.type = TOK_P_$;
    TokenBufferPush(Stack, tmp);

    // token initialization
    Token currentToken = getToken();

    PAT_Element currentElement;

    //while (currentToken.type != TOK_Newline) {
    int i = 0;

    while (i != 30) {

        //printf("\n%s, %s, %d\n", getTokenName(TokenBufferTop(Stack).type), getTokenName(currentToken.type), currentElement);
        currentElement = PAT_GetSign(PAT_GetFromTok(TokenBufferTop(Stack)), PAT_GetFromTok(currentToken));
        
        if (TokenBufferTop(Stack).type == TOK_P_Ex && currentToken.type == TOK_Newline && currentElement == -1)
            currentElement = REDUCE;

        if (TokenBufferTop(Stack).type == TOK_P_Ex && currentToken.type == TOK_Newline  && TokenBufferSecondTop(Stack).type == TOK_P_$)
            break;
        //printf("%s, %s, %d\n", getTokenName(TokenBufferTop(Stack).type), getTokenName(currentToken.type), currentElement);

        if (currentElement == SHIFTT) {
            tmp.type = TOK_P_Less;
            // shift first identifier
            if (TokenBufferTop(Stack).type == TOK_P_$ && PAT_GetFromTok(currentToken) == Identifier) {
                TokenBufferPush(Stack, tmp);
                TokenBufferPush(Stack, currentToken);
                currentToken = getToken();
            }
            // shift identifier
            else if (PAT_GetFromTok(currentToken) == Identifier) {
                TokenBufferPush(Stack, tmp);
                TokenBufferPush(Stack, currentToken);
                currentToken = getToken();
            }
            // shift add or sub operation
            else if (TokenBufferTop(Stack).type == TOK_P_Ex && PAT_GetFromTok(currentToken) == Plus) {
                tmpp = TokenBufferPop(Stack);
                TokenBufferPush(Stack, tmp);
                TokenBufferPush(Stack, tmpp);
                TokenBufferPush(Stack, currentToken);
                currentToken = getToken();
            }
            // shift mul or div operation
            else if (TokenBufferTop(Stack).type == TOK_P_Ex && PAT_GetFromTok(currentToken) == Multiply) {
                tmpp = TokenBufferPop(Stack);
                TokenBufferPush(Stack, tmp);
                TokenBufferPush(Stack, tmpp);
                TokenBufferPush(Stack, currentToken);
                currentToken = getToken();
            }
        }

        if (currentElement == REDUCE) {

            // reduce identifier -> Exp
            if (PAT_GetFromTok(TokenBufferTop(Stack)) == Identifier && TokenBufferSecondTop(Stack).type == TOK_P_Less) {
                TokenBufferPop(Stack);
                TokenBufferPop(Stack);
                tmp.type = TOK_P_Ex;
                
                TokenBufferPush(Stack, tmp);
                //printf("%d\n", currentElement);
            }
            // reduce Exp */ Exp -> Exp
            else if (currentToken.type == TOK_Newline && PAT_GetFromTok(TokenBufferNTop(Stack, 2)) == Multiply) {
                //printf("ml\n");                
                TokenBufferPop(Stack); // pop EXPz
                TokenBufferPop(Stack); // Pop
                TokenBufferPop(Stack); // Pop EXP
                TokenBufferPop(Stack); // Pop <
                tmp.type = TOK_P_Ex;
                TokenBufferPush(Stack, tmp);
                /*
               if (tmp.type == TOK_Add){
                   expRoot->type = NODE_Add;
               } else {
                   expRoot->type = NODE_Sub;
               }
               
               expRoot->childrenCount = 2;         */
            }
            // reduce Exp +- Exp -> Exp
            else if (currentToken.type == TOK_Newline && PAT_GetFromTok(TokenBufferNTop(Stack, 2)) == Plus) {
                //printf("pl\n");

                TokenBufferPop(Stack); // pop EXP
                TokenBufferPop(Stack); // Pop
                TokenBufferPop(Stack); // Pop EXP
                TokenBufferPop(Stack); // Pop <
                tmp.type = TOK_P_Ex;
                TokenBufferPush(Stack, tmp);
                /*
               if (tmp.type == TOK_Mul){
                   expRoot->type = NODE_Mul;
               } else {
                   expRoot->type = NODE_Div;
               }

                expRoot->childrenCount = 2;*/
            }
        }

        i++;
    }

    printf("\n");
    TokenBufferPrint(Stack);

    //printf("%s\n", getTokenName(TokenBufferPop(Stack).type));

    TokenBufferDispose(&Stack);
    printf("\n================\n\n");
    return true;
}

bool isNodeOperator(ASTNodeType type) {
    switch (type) {
    case NODE_Identifier:
    case NODE_Literal_String:
    case NODE_Literal_Int:
    case NODE_Literal_Float:
        return false;

    default:
        return true;
    }
}
