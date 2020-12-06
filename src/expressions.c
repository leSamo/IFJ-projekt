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
    case TOK_P_$:
    default:
        return EOLT;
        break;
    }
}

PAT_Element PAT_GetSign(PAT_Header Row, PAT_Header Column) {
    return PA_Table[Row][Column];
}

bool Reduce(TokenBuffer *Stack, Token currentToken) {
    Token tmp;
    Token tmpp;

    // Reduce identifier to EXP_token
    if (PAT_GetFromTok(TokenBufferTop(Stack)) == Identifier && TokenBufferSecondTop(Stack).type == TOK_P_Less) {
        tmpp = TokenBufferPop(Stack); // pop identifier
        TokenBufferPop(Stack);        // pop <
        if (tmpp.type == TOK_Int_Literal) {
            tmp.i = tmpp.i;
        }
        tmp.type = TOK_P_Ex;
        TokenBufferPush(Stack, tmp);
        printf("Reducing identifier\n");
        return true;
    }
    // Reduce multiplication or division
    else if (TokenBufferNTop(Stack, 1).type == TOK_P_Ex &&
             PAT_GetFromTok(TokenBufferNTop(Stack, 2)) == Multiply &&
             TokenBufferNTop(Stack, 3).type == TOK_P_Ex &&
             TokenBufferNTop(Stack, 4).type == TOK_P_Less) {
        printf("Reducing multiplication or division\n");
        TokenBufferPop(Stack); // Pop EXP
        TokenBufferPop(Stack); // Pop
        TokenBufferPop(Stack); // Pop EXP
        TokenBufferPop(Stack); // Pop <
        tmp.type = TOK_P_Ex;
        TokenBufferPush(Stack, tmp);
        return true;
    }
     // Reduce addition or subtraction
    else if (TokenBufferNTop(Stack, 1).type == TOK_P_Ex &&
             PAT_GetFromTok(TokenBufferNTop(Stack, 2)) == Plus &&
             TokenBufferNTop(Stack, 3).type == TOK_P_Ex &&
             TokenBufferNTop(Stack, 4).type == TOK_P_Less) {
        printf("Reducing addition or subtraction\n");
        TokenBufferPop(Stack); // Pop EXP
        TokenBufferPop(Stack); // Pop
        TokenBufferPop(Stack); // Pop EXP
        TokenBufferPop(Stack); // Pop <
        tmp.type = TOK_P_Ex;
        TokenBufferPush(Stack, tmp);
        return true;
    }
    return false;
}

void InsertShiftAfterTopTerminal(TokenBuffer *Stack, Token currentToken) {
    int i = 1;
    TokenBuffer *tmpStack = TokenBufferCreate();
    Token tmp = TokenBufferNTop(Stack, i);
    Token tmpp;

    do {
        if (tmp.type == TOK_Int_Literal ||
            tmp.type == TOK_Float_Literal ||
            tmp.type == TOK_String_Literal ||
            tmp.type == TOK_Add ||
            tmp.type == TOK_Sub ||
            tmp.type == TOK_Mul ||
            tmp.type == TOK_Div ||
            tmp.type == TOK_P_$) {

            for (int j = 1; j < i; j++) {
                tmpp = TokenBufferPop(Stack);
                TokenBufferPush(tmpStack, tmpp);
            }

            tmpp.type = TOK_P_Less;
            TokenBufferPush(Stack, tmpp);

            for (int j = 1; j < i; j++) {
                tmpp = TokenBufferPop(tmpStack);
                TokenBufferPush(Stack, tmpp);
            }
            TokenBufferPush(Stack, currentToken);
            TokenBufferDispose(&tmpStack);
            break;
        }
        i++;
        tmp = TokenBufferNTop(Stack, i);
    } while (i != 10);

    TokenBufferDispose(&tmpStack);
}

Token GetTopTerminal(TokenBuffer *Stack) {
    int i = 1;
    Token tmp = TokenBufferNTop(Stack, i);
    do {
        if (tmp.type == TOK_Int_Literal ||
            tmp.type == TOK_Float_Literal ||
            tmp.type == TOK_String_Literal ||
            tmp.type == TOK_Add ||
            tmp.type == TOK_Sub ||
            tmp.type == TOK_Mul ||
            tmp.type == TOK_Div ||
            tmp.type == TOK_P_$) {
            return tmp;
            break;
        }
        i++;
        tmp = TokenBufferNTop(Stack, i);
    } while (i != 40);
}

void FillInputStack(TokenBuffer *InStack, Token ovrlapIn) {
    Token tmp = getToken();
    do {
        TokenBufferPush(InStack, tmp);
        tmp = getToken();
    } while (tmp.type != TOK_Newline);
    tmp.type = TOK_P_$;
    TokenBufferPush(InStack, tmp);
}

bool handleExpression(ASTNode *expRoot, Token overlapTokenIn) {
    // buffer initialization
    TokenBuffer *Stack = TokenBufferCreate();   
    TokenBuffer *Input = TokenBufferCreate();
    printf("\n================\n\n");

    FillInputStack(Input, overlapTokenIn);

    // insert $ as bottom of stack
    Token tmp;
    Token tmpp;
    tmp.type = TOK_P_$;
    TokenBufferPush(Stack, tmp);

    // token initialization
    Token currentToken = TokenBufferPopFront(&Input);
   
    PAT_Element currentElement;

    //while (currentToken.type != TOK_Newline) {
    int i = 0;

    while (i != 50) {

        //printf("\n%s, %s, %d\n", getTokenName(GetTopTerminal(Stack).type), getTokenName(currentToken.type), currentElement);
        currentElement = PAT_GetSign(PAT_GetFromTok(GetTopTerminal(Stack)), PAT_GetFromTok(currentToken));
        //printf("%s, %s, %d\n", getTokenName(GetTopTerminal(Stack).type), getTokenName(currentToken.type), currentElement);

        if (TokenBufferTop(Stack).type == TOK_P_Ex && currentToken.type == TOK_P_$) {
            currentElement = REDUCE;
            printf("%s, %s, %d\n", getTokenName(TokenBufferTop(Stack).type), getTokenName(currentToken.type), currentElement);
        }

        if (currentElement == SHIFTT) {
            InsertShiftAfterTopTerminal(Stack, currentToken);
            currentToken = TokenBufferPopFront(&Input);            
        }

        if (currentElement == REDUCE) {

            Reduce(Stack, currentToken);           
            TokenBufferPrint(Stack);
            printf("\n");
        }

        if (currentToken.type == TOK_P_$ && TokenBufferTop(Stack).type == TOK_P_Ex && TokenBufferNTop(Stack, 2).type == TOK_P_$)
            break;
        i++;
    }

    printf("\n");
    TokenBufferPrint(Stack);

    //printf("%s\n", getTokenName(TokenBufferPop(Stack).type));

    TokenBufferDispose(&Stack);
    TokenBufferDispose(&Input);

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
