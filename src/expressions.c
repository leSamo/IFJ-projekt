/* ========= expressions.c =========
 * Project: IFJ 2020/21 project
 * Team: 067, variant I
 * Author: Emma Krompaščíková (xkromp00), Samuel Olekšák (xoleks00), Michal Findra (xfindr00)
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

int nodeCounter;

PAT_Header PAT_GetHeaderFromToken(Token token) {
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

void ExpCreateOperationNode(ASTNodeType nodeType, Token firstTok, Token secondTok, ASTNode **nodes) {
    ASTNode *Node = AST_CreateNode(NULL, nodeType);

    if (secondTok.nodeNum != 0) {
        AST_AttachNode(Node, nodes[secondTok.nodeNum]);
    }

    if (firstTok.nodeNum != 0) {
        AST_AttachNode(Node, nodes[firstTok.nodeNum]);
    }

    nodes[nodeCounter] = Node;
}

void ExpCreateIdentifNode(ASTNodeType nodeType, Token firstTok, ASTNode **nodes) {

    if (firstTok.oldType == TOK_Int_Literal) {
        ASTNode *Child1_Node = AST_CreateIntNode(NULL, NODE_Literal_Int, firstTok.i);
        nodes[nodeCounter] = Child1_Node;
    }
    else if (firstTok.oldType == TOK_Float_Literal) {
        ASTNode *Child1_Node = AST_CreateFloatNode(NULL, NODE_Literal_Float, firstTok.f);
        nodes[nodeCounter] = Child1_Node;
    }
    else if (firstTok.oldType == TOK_String_Literal) {
        ASTNode *Child1_Node = AST_CreateStringNode(NULL, NODE_Literal_String, firstTok.str);
        nodes[nodeCounter] = Child1_Node;
    }
    else if (firstTok.oldType == TOK_Identifier) {
        ASTNode *Child1_Node = AST_CreateStringNode(NULL, NODE_Identifier, firstTok.str);
        nodes[nodeCounter] = Child1_Node;
    }
}

bool Reduce(TokenBuffer *Stack, Token currentToken, ASTNode *parentNode, ASTNode **nodes) {
    Token tmp, tmp2, tmp3;

    // Reduce identifier to EXP_token
    if (PAT_GetHeaderFromToken(TokenBufferTop(Stack)) == Identifier && TokenBufferSecondTop(Stack).type == TOK_P_Less) {
        tmp2 = TokenBufferPop(Stack); // pop identifier
        TokenBufferPop(Stack);        // pop <

        nodeCounter++;

        tmp.nodeNum = nodeCounter;
        tmp.oldType = tmp2.type;
        tmp.type = TOK_P_Ex;

        if (tmp2.type == TOK_Int_Literal) {
            tmp.i = tmp2.i;
            ExpCreateIdentifNode(NODE_Literal_Int, tmp, nodes);
        }
        else if (tmp2.type == TOK_Float_Literal) {
            tmp.f = tmp2.f;
            ExpCreateIdentifNode(NODE_Literal_Float, tmp, nodes);
        }
        else if (tmp2.type == TOK_String_Literal) {
            tmp.str = tmp2.str;
            ExpCreateIdentifNode(NODE_Literal_String, tmp, nodes);
        }
        else if (tmp2.type == TOK_Identifier) {
            tmp.str = tmp2.str;
            ExpCreateIdentifNode(NODE_Identifier, tmp, nodes);
        }

        TokenBufferPush(Stack, tmp);
        return true;
    }
    // Multiplication or division reduction
    else if (TokenBufferNTop(Stack, 1).type == TOK_P_Ex &&
             PAT_GetHeaderFromToken(TokenBufferNTop(Stack, 2)) == Multiply &&
             TokenBufferNTop(Stack, 3).type == TOK_P_Ex &&
             TokenBufferNTop(Stack, 4).type == TOK_P_Less) {
        tmp = TokenBufferPop(Stack);  // Pop EXP
        tmp2 = TokenBufferPop(Stack); // Pop operation
        tmp3 = TokenBufferPop(Stack); // Pop EXP
        TokenBufferPop(Stack);        // Pop <

        if (tmp.oldType != tmp3.oldType) {
            return false;
        }

        nodeCounter++;
        if (tmp2.type == TOK_Mul) {
            ExpCreateOperationNode(NODE_Mul, tmp, tmp3, nodes);
        }
        else if (tmp2.type == TOK_Div) {
            ExpCreateOperationNode(NODE_Div, tmp, tmp3, nodes);
        }

        tmp.type = TOK_P_Ex;
        tmp.oldType = TOK_P_Empty;
        tmp.nodeNum = nodeCounter;

        TokenBufferPush(Stack, tmp);
        return true;
    }
    // Addition or subtraction reduction
    else if (TokenBufferNTop(Stack, 1).type == TOK_P_Ex &&
             PAT_GetHeaderFromToken(TokenBufferNTop(Stack, 2)) == Plus &&
             TokenBufferNTop(Stack, 3).type == TOK_P_Ex &&
             TokenBufferNTop(Stack, 4).type == TOK_P_Less) {
        tmp = TokenBufferPop(Stack);  // Pop EXP
        tmp2 = TokenBufferPop(Stack); // Pop operator
        tmp3 = TokenBufferPop(Stack); // Pop EXP
        TokenBufferPop(Stack);        // Pop <

        if (tmp.oldType != tmp3.oldType) {
            return false;
        }

        nodeCounter++;
        if (tmp2.type == TOK_Add) {
            ExpCreateOperationNode(NODE_Add, tmp, tmp3, nodes);
        }
        else if (tmp2.type == TOK_Sub) {
            ExpCreateOperationNode(NODE_Sub, tmp, tmp3, nodes);
        }

        tmp.type = TOK_P_Ex;
        tmp.oldType = TOK_P_Empty;
        tmp.nodeNum = nodeCounter;

        TokenBufferPush(Stack, tmp);
        return true;
    }
    // Comparation operators reduction
    else if (TokenBufferNTop(Stack, 1).type == TOK_P_Ex &&
             PAT_GetHeaderFromToken(TokenBufferNTop(Stack, 2)) == GreaterThan &&
             TokenBufferNTop(Stack, 3).type == TOK_P_Ex &&
             TokenBufferNTop(Stack, 4).type == TOK_P_Less) {
        tmp = TokenBufferPop(Stack);  // Pop EXP
        tmp2 = TokenBufferPop(Stack); // Pop comparation operator
        tmp3 = TokenBufferPop(Stack); // Pop EXP
        TokenBufferPop(Stack);        // Pop <

        if (tmp.oldType != tmp3.oldType) {
            return false;
        }

        nodeCounter++;
        if (tmp2.type == TOK_More_Then) {
            ExpCreateOperationNode(NODE_More_Then, tmp, tmp3, nodes);
        }
        else if (tmp2.type == TOK_Less_Then) {
            ExpCreateOperationNode(NODE_Less_Then, tmp, tmp3, nodes);
        }
        else if (tmp2.type == TOK_More_Equal_Then) {
            ExpCreateOperationNode(NODE_More_Equal_Then, tmp, tmp3, nodes);
        }
        else if (tmp2.type == TOK_Less_Equal_Then) {
            ExpCreateOperationNode(NODE_Less_Equal_Then, tmp, tmp3, nodes);
        }
        else if (tmp2.type == TOK_Equal) {
            ExpCreateOperationNode(NODE_Equal, tmp, tmp3, nodes);
        }
        else if (tmp2.type == TOK_Not_Equal) {
            ExpCreateOperationNode(NODE_Not_Equal, tmp, tmp3, nodes);
        }

        tmp.type = TOK_P_Ex;
        tmp.oldType = TOK_P_Empty;
        tmp.nodeNum = nodeCounter;

        TokenBufferPush(Stack, tmp);
        return true;
    }
    // Parens reduction
    else if (TokenBufferNTop(Stack, 1).type == TOK_R_Paren &&
             TokenBufferNTop(Stack, 2).type == TOK_P_Ex &&
             TokenBufferNTop(Stack, 3).type == TOK_L_Paren) {
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
    Token currentToken, overlapOut, previousToken;
    previousToken.type = TOK_Empty;

    // check if overlapIn token is present
    if (overlapIn.type != TOK_Empty) {
        currentToken = overlapIn;
    }
    else {
        currentToken = getToken();
    }

    // check if operator isnt unary
    if (isOperator(currentToken.type)) {
        overlapOut.type = TOK_Empty;
        return overlapOut;
    }

    // get tokens until symbol is not valid and push them to input stack
    do {
        TokenBufferPush(InStack, currentToken);
        currentToken = getToken();

        if (isOperator(currentToken.type) && isOperator(previousToken.type)) {
            // if two operators follow, return error
            overlapOut.type = TOK_Empty;
            return overlapOut;
        }
        previousToken = currentToken;

    } while (isValidExpToken(currentToken.type));

    // retrun overlaped token
    overlapOut = currentToken;
    currentToken.type = TOK_P_$;
    TokenBufferPush(InStack, currentToken);

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
    //printf("\n================\n\n");
    nodeCounter = 0;
    // buffers initialization
    TokenBuffer *Stack = TokenBufferCreate();
    TokenBuffer *Input = TokenBufferCreate();
    ASTNode *nodes[20];

    // fill input stack
    *overlapTokenOut = FillInputStack(Input, overlapTokenIn);
    if (overlapTokenOut->type == TOK_Empty) {
        TokenBufferDispose(&Stack);
        TokenBufferDispose(&Input);
        return false;
    }

    // insert $ as bottom of working stack
    Token tmp;
    Token tmpp;
    tmp.type = TOK_P_$;
    TokenBufferPush(Stack, tmp);

    // token initialization
    Token currentToken = TokenBufferPopFront(&Input);

    PAT_Element currentElement;

    int i = 0;

    while (i != 200) {

        // printf("\n%s, %s, %d\n", getTokenName(GetTopTerminal(Stack).type), getTokenName(currentToken.type), currentElement);
        currentElement = PAT_GetSign(PAT_GetHeaderFromToken(GetTopTerminal(Stack)), PAT_GetHeaderFromToken(currentToken));
        //printf("%s, %s, %d\n", getTokenName(GetTopTerminal(Stack).type), getTokenName(currentToken.type), currentElement);

        if (currentElement == ERRORR) {
            TokenBufferDispose(&Stack);
            TokenBufferDispose(&Input);
            return false;
        }

        if (currentElement == SHIFTT) {
            InsertShiftAfterTopTerminal(Stack, currentToken);
            currentToken = TokenBufferPopFront(&Input);
        }
        else if (currentElement == REDUCE) {
            if (Reduce(Stack, currentToken, expRoot, nodes) == false) {
                throwError(INCOMPATIBLE_TYPE_ERROR, "Invalid expression.\n", true);
                TokenBufferDispose(&Stack);
                TokenBufferDispose(&Input);
                return false;
            }
            // printf("\n");
            // TokenBufferPrint(Stack);
        }
        else if (currentElement == EQUALL) {
            TokenBufferPush(Stack, currentToken);
            currentToken = TokenBufferPopFront(&Input);
        }

        if (currentToken.type == TOK_P_$ && TokenBufferTop(Stack).type == TOK_P_Ex && TokenBufferNTop(Stack, 2).type == TOK_P_$)
            break;

        i++;
    }

    //  printf("\n");
    // TokenBufferPrint(Stack);

    TokenBufferDispose(&Stack);
    TokenBufferDispose(&Input);
    attachASTToRoot(expRoot, nodes);
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

bool isOperator(tokenType type) {
    switch (type) {
    case TOK_Add:
    case TOK_Sub:
    case TOK_Mul:
    case TOK_Div:
    case TOK_Equal:
    case TOK_Not_Equal:
    case TOK_More_Equal_Then:
    case TOK_More_Then:
    case TOK_Less_Equal_Then:
    case TOK_Less_Then:
        return true;

    default:
        return false;
    }
}