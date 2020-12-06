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

int nodeCounter = 0;

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

bool isValidExpToken(tokenType type) {
    switch (type) {
    case TOK_Identifier:
    case TOK_Int_Literal:
    case TOK_Float_Literal:
    case TOK_String_Literal:
    case TOK_L_Paren:
    case TOK_R_Paren:
    case TOK_Mul:
    case TOK_Div:
    case TOK_Add:
    case TOK_Sub:
    case TOK_Less_Then:
    case TOK_Less_Equal_Then:
    case TOK_More_Then:
    case TOK_More_Equal_Then:
    case TOK_Equal:
    case TOK_Not_Equal:
    case TOK_P_$:
        return true;
    default:
        return false;
    }
}

void ExpCreateNode(ASTNodeType nodeType, Token firstTok, Token secondTok, ASTNode **nodes) {
    ASTNode *Node = AST_CreateNode(NULL, nodeType);

    if (secondTok.oldType == TOK_Int_Literal && secondTok.nodeNum == 0) {
        ASTNode *Child2_Node = AST_CreateIntNode(Node, NODE_Literal_Int, secondTok.i);
    }
    else if (secondTok.nodeNum != 0) {
        AST_AttachNode(Node, nodes[secondTok.nodeNum]);
    }

    if (firstTok.oldType == TOK_Int_Literal && firstTok.nodeNum == 0) {
        ASTNode *Child1_Node = AST_CreateIntNode(Node, NODE_Literal_Int, firstTok.i);
    }
    else if (firstTok.nodeNum != 0) {
        AST_AttachNode(Node, nodes[firstTok.nodeNum]);
    }

    
    nodes[nodeCounter] = Node;
}

bool Reduce(TokenBuffer *Stack, Token currentToken, ASTNode *parentNode, ASTNode **nodes) {
    Token tmp, tmp2, tmp3;

    // Reduce identifier to EXP_token
    if (PAT_GetFromTok(TokenBufferTop(Stack)) == Identifier && TokenBufferSecondTop(Stack).type == TOK_P_Less) {
        tmp2 = TokenBufferPop(Stack); // pop identifier
        TokenBufferPop(Stack);        // pop <
        if (tmp2.type == TOK_Int_Literal) {
            tmp.i = tmp2.i;
        }
        tmp.oldType = tmp2.type;
        tmp.type = TOK_P_Ex;
        tmp.nodeNum = 0;
        TokenBufferPush(Stack, tmp);
        printf("Reducing identifier\n");
        return true;
    }
    // Multiplication or division reduction
    else if (TokenBufferNTop(Stack, 1).type == TOK_P_Ex &&
             PAT_GetFromTok(TokenBufferNTop(Stack, 2)) == Multiply &&
             TokenBufferNTop(Stack, 3).type == TOK_P_Ex &&
             TokenBufferNTop(Stack, 4).type == TOK_P_Less) {
        printf("Reducing multiplication or division\n");
        tmp = TokenBufferPop(Stack);  // Pop EXP
        tmp2 = TokenBufferPop(Stack); // Pop operation
        tmp3 = TokenBufferPop(Stack); // Pop EXP
        TokenBufferPop(Stack);        // Pop <

        nodeCounter++;
        if (tmp2.type == TOK_Mul) {
            ExpCreateNode(NODE_Mul, tmp, tmp3, nodes);
        }
        else if (tmp2.type == TOK_Div) {
            ExpCreateNode(NODE_Div, tmp, tmp3, nodes);
        }

        tmp.type = TOK_P_Ex;
        tmp.oldType = TOK_P_Empty;
        tmp.nodeNum = nodeCounter;

        TokenBufferPush(Stack, tmp);
        return true;
    }
    // Addition or subtraction reduction
    else if (TokenBufferNTop(Stack, 1).type == TOK_P_Ex &&
             PAT_GetFromTok(TokenBufferNTop(Stack, 2)) == Plus &&
             TokenBufferNTop(Stack, 3).type == TOK_P_Ex &&
             TokenBufferNTop(Stack, 4).type == TOK_P_Less) {
        printf("Reducing addition or subtraction\n");
        tmp = TokenBufferPop(Stack);  // Pop EXP
        tmp2 = TokenBufferPop(Stack); // Pop operator
        tmp3 = TokenBufferPop(Stack); // Pop EXP
        TokenBufferPop(Stack);        // Pop <

        nodeCounter++;
        if (tmp2.type == TOK_Add) {
            ExpCreateNode(NODE_Add, tmp, tmp3, nodes);
        }
        else if (tmp2.type == TOK_Sub) {
            ExpCreateNode(NODE_Sub, tmp, tmp3, nodes);
        }

        tmp.type = TOK_P_Ex;
        tmp.oldType = TOK_P_Empty;
        tmp.nodeNum = nodeCounter;

        TokenBufferPush(Stack, tmp);
        return true;
    }
    // Comparation operators reduction
    else if (TokenBufferNTop(Stack, 1).type == TOK_P_Ex &&
             PAT_GetFromTok(TokenBufferNTop(Stack, 2)) == GreaterThan &&
             TokenBufferNTop(Stack, 3).type == TOK_P_Ex &&
             TokenBufferNTop(Stack, 4).type == TOK_P_Less) {
        printf("Reducing compare\n");
        TokenBufferPop(Stack); // Pop EXP
        TokenBufferPop(Stack); // Pop comparation operator
        TokenBufferPop(Stack); // Pop EXP
        TokenBufferPop(Stack); // Pop <
        tmp.type = TOK_P_Ex;
        TokenBufferPush(Stack, tmp);
        return true;
    }
    // Parens reduction
    else if (TokenBufferNTop(Stack, 1).type == TOK_R_Paren &&
             TokenBufferNTop(Stack, 2).type == TOK_P_Ex &&
             TokenBufferNTop(Stack, 3).type == TOK_L_Paren) {
        printf("Reducing parens\n");
        TokenBufferPop(Stack);       // Pop rparen
        tmp = TokenBufferPop(Stack); // Pop exp
        TokenBufferPop(Stack);       // Pop lparen
        TokenBufferPop(Stack);       // pop <
        TokenBufferPush(Stack, tmp);
        return true;
    }

    // if no reduction was made return false
    return false;
}

void InsertShiftAfterTopTerminal(TokenBuffer *Stack, Token currentToken) {
    int i = 1;
    TokenBuffer *tmpStack = TokenBufferCreate();
    Token tmp = TokenBufferNTop(Stack, i);
    Token tmpp;

    do {
        if (isValidExpToken(tmp.type)) {

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
        if (isValidExpToken(tmp.type)) {
            return tmp;
            break;
        }
        i++;
        tmp = TokenBufferNTop(Stack, i);
    } while (i != 40);
}

Token FillInputStack(TokenBuffer *InStack, Token overlapIn) {
    Token tmp, overlapOut;

    if (overlapIn.type != TOK_Empty) {
        tmp = overlapIn;
    }
    else {
        tmp = getToken();
    }

    do {
        TokenBufferPush(InStack, tmp);
        tmp = getToken();
    } while (isValidExpToken(tmp.type));

    overlapOut = tmp;
    tmp.type = TOK_P_$;
    TokenBufferPush(InStack, tmp);
    TokenBufferPrint(InStack);

    return overlapOut;
}

void attachASTToRoot(ASTNode *expRoot, ASTNode **nodes) {
    int i = 1;
    while (nodes[i]->parent != NULL) {
        i++;
    }
    AST_AttachNode(expRoot, nodes[i]);
}

bool handleExpression(ASTNode *expRoot, Token overlapTokenIn, Token *overlapTokenOut) {
    printf("\n================\n\n");

    // buffers initialization
    TokenBuffer *Stack = TokenBufferCreate();
    TokenBuffer *Input = TokenBufferCreate();
    ASTNode *nodes[20];

    // fill input stack
    *overlapTokenOut = FillInputStack(Input, overlapTokenIn);

    // insert $ as bottom of working stack
    Token tmp;
    Token tmpp;
    tmp.type = TOK_P_$;
    TokenBufferPush(Stack, tmp);

    // token initialization
    Token currentToken = TokenBufferPopFront(&Input);

    PAT_Element currentElement;

    int i = 0;

    while (i != 80) {

        printf("\n%s, %s, %d\n", getTokenName(GetTopTerminal(Stack).type), getTokenName(currentToken.type), currentElement);
        currentElement = PAT_GetSign(PAT_GetFromTok(GetTopTerminal(Stack)), PAT_GetFromTok(currentToken));
        printf("%s, %s, %d\n", getTokenName(GetTopTerminal(Stack).type), getTokenName(currentToken.type), currentElement);

        if (currentElement == SHIFTT) {
            InsertShiftAfterTopTerminal(Stack, currentToken);
            currentToken = TokenBufferPopFront(&Input);
        }
        else if (currentElement == REDUCE) {
            Reduce(Stack, currentToken, expRoot, nodes);
            printf("\n");
            TokenBufferPrint(Stack);
        }
        else if (currentElement == EQUALL) {
            TokenBufferPush(Stack, currentToken);
            currentToken = TokenBufferPopFront(&Input);
        }

        if (currentToken.type == TOK_P_$ && TokenBufferTop(Stack).type == TOK_P_Ex && TokenBufferNTop(Stack, 2).type == TOK_P_$)
            break;

        i++;
    }

    printf("\n");
    TokenBufferPrint(Stack);

    TokenBufferDispose(&Stack);
    TokenBufferDispose(&Input);

    attachASTToRoot(expRoot, nodes);

    printf("\n================\n\n");

    printf("%d", nodeCounter);

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
