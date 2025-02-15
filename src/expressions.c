/* ========= expressions.c =========
 * Project: IFJ 2020/21 project
 * Team: 067, variant I
 * Author: Emma Krompaščíková (xkromp00), Samuel Olekšák (xoleks00), Michal Findra (xfindr00)
 * Date: November, December 2020
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
    case TOK_Greater_Than:
    case TOK_Greater_Equal_Than:
    case TOK_Less_Than:
    case TOK_Less_Equal_Than:
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
    case TOK_Less_Than:
    case TOK_Less_Equal_Than:
    case TOK_Greater_Than:
    case TOK_Greater_Equal_Than:
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
    case TOK_Greater_Equal_Than:
    case TOK_Greater_Than:
    case TOK_Less_Equal_Than:
    case TOK_Less_Than:
        return true;

    default:
        return false;
    }
}

bool isNodeMathOperator(ASTNodeType type) {
    switch (type) {
    case NODE_Identifier:
    case NODE_Literal_String:
    case NODE_Literal_Int:
    case NODE_Literal_Float:
    case NODE_Less_Than:
    case NODE_Greater_Than:
    case NODE_Less_Equal_Than:
    case NODE_Greater_Equal_Than:
    case NODE_Equal:
    case NODE_Not_Equal:
        return false;
    default:
        return true;
    }
}

bool isNodeCompOperator(ASTNodeType type) {
    switch (type) {
    case NODE_Identifier:
    case NODE_Literal_String:
    case NODE_Literal_Int:
    case NODE_Literal_Float:
    case NODE_Add:
    case NODE_Sub:
    case NODE_Mul:
    case NODE_Div:
        return false;

    default:
        return true;
    }
}

void ExpCreateOperationNode(ASTNodeType nodeType, Token firstTok, Token secondTok, ASTNode **nodes) {    
    // check if nodes have same operator type (boolean, arithmetic)
    if ((isNodeMathOperator(nodeType) && (isNodeCompOperator(nodes[secondTok.nodeNum]->type) || isNodeCompOperator(nodes[firstTok.nodeNum]->type))) ||
        (isNodeCompOperator(nodeType) && (isNodeMathOperator(nodes[secondTok.nodeNum]->type) || isNodeMathOperator(nodes[firstTok.nodeNum]->type)))) {       
        throwError(SYNTAX_ERROR, "Invalid expression.\n", true);       
    }

    // create operation node
    ASTNode *Node = AST_CreateNode(NULL, nodeType);
    Node->isOperatorResult = true;

    // attach nodes to new created operation node
    AST_AttachNode(Node, nodes[secondTok.nodeNum]);
    AST_AttachNode(Node, nodes[firstTok.nodeNum]);

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
        if (!(firstOperand.oldType == TOK_Identifier || secondOperand.oldType == TOK_Identifier)) {
            if (firstOperand.oldType != secondOperand.oldType) {
                return false;
            }
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
        if (!(firstOperand.oldType == TOK_Identifier || secondOperand.oldType == TOK_Identifier)) {
            if (firstOperand.oldType != secondOperand.oldType) {
                return false;
            }
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
        if (!(firstOperand.oldType == TOK_Identifier || secondOperand.oldType == TOK_Identifier)) {
            if (firstOperand.oldType != secondOperand.oldType) {
                return false;
            }
        }

        nodeCounter++;
        if (operator.type == TOK_Greater_Than) {
            ExpCreateOperationNode(NODE_Greater_Than, firstOperand, secondOperand, nodes);
        }
        else if (operator.type == TOK_Less_Than) {
            ExpCreateOperationNode(NODE_Less_Than, firstOperand, secondOperand, nodes);
        }
        else if (operator.type == TOK_Greater_Equal_Than) {
            ExpCreateOperationNode(NODE_Greater_Equal_Than, firstOperand, secondOperand, nodes);
        }
        else if (operator.type == TOK_Less_Equal_Than) {
            ExpCreateOperationNode(NODE_Less_Equal_Than, firstOperand, secondOperand, nodes);
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
    throwError(SYNTAX_ERROR, "Invalid expression.\n", true);
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
    } while (i != 100);

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
    } while (i != 100);
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
    PA_Stack = TokenBufferCreate();
    PA_Input = TokenBufferCreate();
    ASTNode *nodes[50];

    // fill input stack
    *overlapTokenOut = FillInputStack(PA_Input, overlapTokenIn);

    // insert $ as bottom of work stack
    Token tmp;
    tmp.type = TOK_P_$;
    TokenBufferPush(PA_Stack, tmp);

    // token initialization
    Token currentToken = TokenBufferPopFront(&PA_Input);
    PAT_Element currentElement;
    
    while ((currentToken.type == TOK_P_$ && TokenBufferTop(PA_Stack).type == TOK_P_Ex && TokenBufferNTop(PA_Stack, 2).type == TOK_P_$) != true) {
        //get element from the precedence table
        currentElement = PAT_GetSign(PAT_GetHeaderFromToken(GetTopTerminal(PA_Stack)), PAT_GetHeaderFromToken(currentToken));

        // element action
        if (currentElement == ERRORR) {
            TokenBufferDispose(&PA_Stack);
            TokenBufferDispose(&PA_Input);
            return false;
        }
        else if (currentElement == SHIFTT) {
            InsertShiftAfterTopTerminal(PA_Stack, currentToken);
            currentToken = TokenBufferPopFront(&PA_Input);
        }
        else if (currentElement == REDUCE) {
            if (Reduce(PA_Stack, currentToken, nodes) == false) {                
                throwError(INCOMPATIBLE_TYPE_ERROR, "Invalid expression.\n", true);               
            }
        }
        else if (currentElement == EQUALL) {
            TokenBufferPush(PA_Stack, currentToken);
            currentToken = TokenBufferPopFront(&PA_Input);
        }   
    }    

    TokenBufferDispose(&PA_Stack);
    TokenBufferDispose(&PA_Input);
    attachASTToRoot(expRoot, nodes);
    return true;
}