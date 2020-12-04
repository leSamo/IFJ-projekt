/* ======== precedenceTable.c ========
 * Project: IFJ 2020/21 project
 * Team: 067, variant I
 * Author: 
 * Date: November 2020
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
    GreaterSign = 1, // >
    LessSign = 2,    // <
    EqualSign = 3,   // =
    Empty = -1
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
    {GreaterSign, LessSign, LessSign, GreaterSign, LessSign, GreaterSign, GreaterSign},
    {GreaterSign, GreaterSign, LessSign, GreaterSign, LessSign, GreaterSign, GreaterSign},
    {LessSign, LessSign, LessSign, Equal, LessSign, LessSign, Empty},
    {GreaterSign, GreaterSign, Empty, GreaterSign, Empty, GreaterSign, GreaterSign},
    {GreaterSign, GreaterSign, Empty, GreaterSign, Empty, GreaterSign, GreaterSign},
    {LessSign, LessSign, LessSign, GreaterSign, LessSign, Empty, GreaterSign},
    {LessSign, LessSign, LessSign, Empty, LessSign, LessSign, Empty}};

PAT_Element PAT_GetSign(PAT_Header Row, PAT_Header Column) {
    return PA_Table[Row][Column];
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
        break;
    }
}

// EXAMPLE: ifj08-cz.pdf 7

bool handleExpressionnn(ASTNode *expRoot, Token overlapTokenIn, Token *overlapTokenOut) {
    // buffer initialization
    TokenBuffer *Stack = TokenBufferCreate();

    // token initialization
    Token currentToken = EMPTY_TOKEN;

    while (currentToken.type != TOK_EOF) {
        currentToken = getToken();

        switch (PAT_GetSign(PAT_GetFromTok(currentToken.type), TokenBufferTop(Stack).type)) {
        case GreaterSign:
            break;
        case LessSign:
            break;
        case EqualSign:
            break;
        case Empty:
            break;
        default:
            break;
        }
    }
    return false;
}