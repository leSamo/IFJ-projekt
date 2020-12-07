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

Token FillInputStack(TokenBuffer *InStack, Token overlapIn) {
    Token currentToken, overlapOut;
    // check if overlapIn token is present
    if (overlapIn.type != TOK_Empty) {
        currentToken = overlapIn;
    }
    else {
        currentToken = getToken();
    }

    // get tokens until symbol is not valid and push them to input stack
    do {
        TokenBufferPush(InStack, currentToken);
        currentToken = getToken();

    } while (isValidExpToken(currentToken.type));

    // retrun overlaped token
    overlapOut = currentToken;
    currentToken.type = TOK_P_$;
    TokenBufferPush(InStack, currentToken);
    return overlapOut;
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

void ExpCreateOperationNode(ASTNodeType nodeType, Token firstTok, Token secondTok, ASTNode **nodes) {
    ASTNode *Node = AST_CreateNode(NULL, nodeType);
    Node->isOperatorResult = true;

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
        Child1_Node->valueType = TAG_Int;
        nodes[nodeCounter] = Child1_Node;
    }
    else if (firstTok.oldType == TOK_Float_Literal) {
        ASTNode *Child1_Node = AST_CreateFloatNode(NULL, NODE_Literal_Float, firstTok.f);
        Child1_Node->valueType = TAG_Float;
        nodes[nodeCounter] = Child1_Node;
    }
    else if (firstTok.oldType == TOK_String_Literal) {
        ASTNode *Child1_Node = AST_CreateStringNode(NULL, NODE_Literal_String, firstTok.str);
        Child1_Node->valueType = TAG_String;
        nodes[nodeCounter] = Child1_Node;
    }
    else if (firstTok.oldType == TOK_Identifier) {
        ASTNode *Child1_Node = AST_CreateStringNode(NULL, NODE_Identifier, firstTok.str);
        Child1_Node->valueType = TAG_Unknown;
        nodes[nodeCounter] = Child1_Node;
    }
}

bool Reduce(TokenBuffer *Stack, Token currentToken, ASTNode **nodes) {
    Token firstOperand, operator, secondOperand;

    // Reduce identifier to EXP_token (Identif -> Exp)
    if (PAT_GetHeaderFromToken(TokenBufferTop(Stack)) == Identifier && TokenBufferNTop(Stack, 2).type == TOK_P_Less) {
        firstOperand = TokenBufferPop(Stack); // pop identifier
        TokenBufferPop(Stack);                // pop <
        nodeCounter++;
        secondOperand.nodeNum = nodeCounter;
        secondOperand.oldType = firstOperand.type;
        secondOperand.type = TOK_P_Ex;

        if (firstOperand.type == TOK_Int_Literal) {
            secondOperand.i = firstOperand.i;
            ExpCreateIdentifNode(NODE_Literal_Int, secondOperand, nodes);
        }
        else if (firstOperand.type == TOK_Float_Literal) {
            secondOperand.f = firstOperand.f;
            ExpCreateIdentifNode(NODE_Literal_Float, secondOperand, nodes);
        }
        else if (firstOperand.type == TOK_String_Literal) {
            secondOperand.str = firstOperand.str;
            ExpCreateIdentifNode(NODE_Literal_String, secondOperand, nodes);
        }
        else if (firstOperand.type == TOK_Identifier) {
            secondOperand.str = firstOperand.str;
            ExpCreateIdentifNode(NODE_Identifier, secondOperand, nodes);
        }

        // push token back to stack for further processing
        TokenBufferPush(Stack, secondOperand);
        return true;
    }
    // Multiplication or division reduction (Exp /* Exp -> Exp)
    else if (TokenBufferNTop(Stack, 1).type == TOK_P_Ex && PAT_GetHeaderFromToken(TokenBufferNTop(Stack, 2)) == Multiply && TokenBufferNTop(Stack, 3).type == TOK_P_Ex && TokenBufferNTop(Stack, 4).type == TOK_P_Less) {
        firstOperand = TokenBufferPop(Stack);  // Pop EXP
        operator= TokenBufferPop(Stack);       // Pop operation
        secondOperand = TokenBufferPop(Stack); // Pop EXP
        TokenBufferPop(Stack);                 // Pop <

        // check if operands are not strings
        if (firstOperand.oldType == TOK_String_Literal && secondOperand.oldType == TOK_String_Literal) {
            return false;
        }

        // check if operands are same type
        if (firstOperand.oldType != secondOperand.oldType) {
            return false;
        }

        //create operation node
        nodeCounter++;
        if (operator.type == TOK_Mul) {
            ExpCreateOperationNode(NODE_Mul, firstOperand, secondOperand, nodes);
        }
        else if (operator.type == TOK_Div) {
            // check for zero division
            if ((firstOperand.oldType == TOK_Int_Literal && firstOperand.i == 0) || (firstOperand.oldType == TOK_Float_Literal && firstOperand.i == 0.0)) {
                throwError(ZERO_DIVISION_ERROR, "Invalid expression.\n", true);
                return false;
            }
            ExpCreateOperationNode(NODE_Div, firstOperand, secondOperand, nodes);
        }

        // push token back to stack for further processing
        firstOperand.type = TOK_P_Ex;
        firstOperand.oldType = secondOperand.oldType;
        firstOperand.nodeNum = nodeCounter;
        TokenBufferPush(Stack, firstOperand);
        return true;
    }
    // Addition or subtraction reduction (Exp +- Exp -> Exp)
    else if (TokenBufferNTop(Stack, 1).type == TOK_P_Ex && PAT_GetHeaderFromToken(TokenBufferNTop(Stack, 2)) == Plus && TokenBufferNTop(Stack, 3).type == TOK_P_Ex && TokenBufferNTop(Stack, 4).type == TOK_P_Less) {
        firstOperand = TokenBufferPop(Stack);  // Pop EXP
        operator= TokenBufferPop(Stack);       // Pop operator
        secondOperand = TokenBufferPop(Stack); // Pop EXP
        TokenBufferPop(Stack);                 // Pop <

        // check if string operation is only addition
        if (firstOperand.oldType == TOK_String_Literal && secondOperand.oldType == TOK_String_Literal && operator.type == TOK_Sub) {
            return false;
        }

        // chek if operands have same type
        if (firstOperand.oldType != secondOperand.oldType) {
            return false;
        }

        // create operation node and attach leafs
        if (operator.type == TOK_Add) {
            nodeCounter++;
            ExpCreateOperationNode(NODE_Add, firstOperand, secondOperand, nodes);
        }
        else if (operator.type == TOK_Sub) {
            nodeCounter++;
            ExpCreateOperationNode(NODE_Sub, firstOperand, secondOperand, nodes);
        }

        // push token back to stack for further processing
        firstOperand.type = TOK_P_Ex;
        firstOperand.oldType = secondOperand.oldType;
        firstOperand.nodeNum = nodeCounter;
        TokenBufferPush(Stack, firstOperand);
        return true;
    }
    // Comparation operators reduction (Exp <>!= Exp -> Exp)
    else if (TokenBufferNTop(Stack, 1).type == TOK_P_Ex && PAT_GetHeaderFromToken(TokenBufferNTop(Stack, 2)) == GreaterThan && TokenBufferNTop(Stack, 3).type == TOK_P_Ex && TokenBufferNTop(Stack, 4).type == TOK_P_Less) {
        firstOperand = TokenBufferPop(Stack);  // Pop EXP
        operator= TokenBufferPop(Stack);       // Pop comparation operator
        secondOperand = TokenBufferPop(Stack); // Pop EXP
        TokenBufferPop(Stack);                 // Pop <

        // check if operands are same type
        if (firstOperand.oldType != secondOperand.oldType) {
            return false;
        }

        nodeCounter++;
        if (operator.type == TOK_More_Then) {
            ExpCreateOperationNode(NODE_More_Then, firstOperand, secondOperand, nodes);
        }
        else if (operator.type == TOK_Less_Then) {
            ExpCreateOperationNode(NODE_Less_Then, firstOperand, secondOperand, nodes);
        }
        else if (operator.type == TOK_More_Equal_Then) {
            ExpCreateOperationNode(NODE_More_Equal_Then, firstOperand, secondOperand, nodes);
        }
        else if (operator.type == TOK_Less_Equal_Then) {
            ExpCreateOperationNode(NODE_Less_Equal_Then, firstOperand, secondOperand, nodes);
        }
        else if (operator.type == TOK_Equal) {
            ExpCreateOperationNode(NODE_Equal, firstOperand, secondOperand, nodes);
        }
        else if (operator.type == TOK_Not_Equal) {
            ExpCreateOperationNode(NODE_Not_Equal, firstOperand, secondOperand, nodes);
        }

        // push token back to stack for further processing
        firstOperand.type = TOK_P_Ex;
        firstOperand.oldType = operator.type;
        firstOperand.nodeNum = nodeCounter;
        TokenBufferPush(Stack, firstOperand);
        return true;
    }
    // Parens reduction
    else if (TokenBufferNTop(Stack, 1).type == TOK_R_Paren && TokenBufferNTop(Stack, 2).type == TOK_P_Ex && TokenBufferNTop(Stack, 3).type == TOK_L_Paren) {
        TokenBufferPop(Stack);                // Pop rparen
        firstOperand = TokenBufferPop(Stack); // Pop exp
        TokenBufferPop(Stack);                // Pop lparen
        TokenBufferPop(Stack);                // pop <
        TokenBufferPush(Stack, firstOperand);
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

    // find top terminal on stack and insert Less sign
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

            // put current token on top of the stack
            TokenBufferPush(Stack, currentToken);
            TokenBufferDispose(&tmpStack);
            break;
        }
        i++;
        tmp = TokenBufferNTop(Stack, i);
    } while (i != 50);

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
    } while (i != 50);
}

void attachASTToRoot(ASTNode *expRoot, ASTNode **nodes) {
    int i = 1;
    while (nodes[i]->parent != NULL) {
        i++;
    }
    AST_AttachNode(expRoot, nodes[i]);
}

bool handleExpression(ASTNode *expRoot, Token overlapTokenIn, Token *overlapTokenOut) {
    nodeCounter = 0;
    // buffers initialization
    TokenBuffer *Stack = TokenBufferCreate();
    TokenBuffer *Input = TokenBufferCreate();
    ASTNode *nodes[30];

    // fill input stack
    *overlapTokenOut = FillInputStack(Input, overlapTokenIn);

    // insert $ as bottom of work stack
    Token tmp;
    tmp.type = TOK_P_$;
    TokenBufferPush(Stack, tmp);

    // token initialization
    Token currentToken = TokenBufferPopFront(&Input);
    PAT_Element currentElement;

    int counter = 0; // counter to check for internal error
    while ((currentToken.type == TOK_P_$ && TokenBufferTop(Stack).type == TOK_P_Ex && TokenBufferNTop(Stack, 2).type == TOK_P_$) != true) {
        currentElement = PAT_GetSign(PAT_GetHeaderFromToken(GetTopTerminal(Stack)), PAT_GetHeaderFromToken(currentToken));

        if (currentElement == ERRORR) {
            TokenBufferDispose(&Stack);
            TokenBufferDispose(&Input);
            return false;
        }
        else if (currentElement == SHIFTT) {
            InsertShiftAfterTopTerminal(Stack, currentToken);
            currentToken = TokenBufferPopFront(&Input);
        }
        else if (currentElement == REDUCE) {
            if (Reduce(Stack, currentToken, nodes) == false) {
                throwError(INCOMPATIBLE_TYPE_ERROR, "Invalid expression.\n", true);
                TokenBufferDispose(&Stack);
                TokenBufferDispose(&Input);
                return false;
            }
        }
        else if (currentElement == EQUALL) {
            TokenBufferPush(Stack, currentToken);
            currentToken = TokenBufferPopFront(&Input);
        }

        if (counter == 300) {
            TokenBufferDispose(&Stack);
            TokenBufferDispose(&Input);
            return false;
        }
        counter++;
    }

    TokenBufferDispose(&Stack);
    TokenBufferDispose(&Input);
    attachASTToRoot(expRoot, nodes);
    return true;
}
