/* ========= expressions.c =========
 * Project: IFJ 2020/21 project
 * Team: 067, variant I
 * Author: Emma Krompaščíková (xkromp00), Samuel Olekšák (xoleks00)
 * Date: November 2020
 * Description: Expression syntactic analysis with postfix conversion using shunting-yard algorithm
 * ================================= */
/*
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "constants.h"
#include "tokenBuffer.c"
#include "expressions.h"
#include "leScanner.h"
#include "AST.c"

typedef enum {
    SHIFTT = 1, // >
    REDUCE = 2,    // <
    EQUALL = 3,   // =
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

Token PA_Table[7][7] = {
    {SHIFTT, REDUCE, REDUCE, SHIFTT, REDUCE, SHIFTT, SHIFTT},
    {SHIFTT, SHIFTT, REDUCE, SHIFTT, REDUCE, SHIFTT, SHIFTT},
    {REDUCE, REDUCE, REDUCE, EQUALL, REDUCE, REDUCE, ERRORR},
    {SHIFTT, SHIFTT, ERRORR, SHIFTT, ERRORR, SHIFTT, SHIFTT},
    {SHIFTT, SHIFTT, ERRORR, SHIFTT, ERRORR, SHIFTT, SHIFTT},
    {REDUCE, REDUCE, REDUCE, SHIFTT, REDUCE, ERRORR, SHIFTT},
    {REDUCE, REDUCE, REDUCE, ERRORR, REDUCE, REDUCE, ERRORR}};

Token PAT_GetSign(Token Row, Token Column) {
    return PA_Table[Row.type][Column.type];
}

PAT_Header PAT_GetFromTok(tokenType token) {
    switch (token) {
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
    default:
        return EOLT;
        break;
    }
}

bool handleExpression(ASTNode *expRoot, Token overlapTokenIn) {
    // buffer initialization
    TokenBuffer *Stack = TokenBufferCreate();
   TokenBufferPush(Stack, EOLT );
    // token initialization
    Token currentToken = EMPTY_TOKEN;

    while (currentToken.type != TOK_EOF) {
        currentToken = getToken();
        //printf("%d\n", TokenBufferTop(Stack).type);    
        
        switch (PAT_GetSign(PAT_GetFromTok(currentToken.type), TokenBufferTop(Stack).type)) {

        case SHIFTT:
            
            break;
        case REDUCE:
            break;
        case EQUALL:
            break;
        case ERRORR:
            break;
        default:
            break;
        }
    }



    TokenBufferDispose(&Stack);
    return true;
}

int reduce(TokenBuffer* stack){
    Token tmp = TokenBufferTop(stack);

   
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
*/