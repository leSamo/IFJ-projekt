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

bool handleExpression(ASTNode *expRoot, Token overlapTokenIn, Token *overlapTokenOut) {
    TokenBuffer *operatorStack = TokenBufferCreate();
    TokenBuffer *outputQueue = TokenBufferCreate();

    Token currentToken = EMPTY_TOKEN, previousToken = EMPTY_TOKEN;

    for (int i = 0; ; i++) {
        if (i == 0 && overlapTokenIn.type != TOK_Empty){
            currentToken = overlapTokenIn;
        }
        else {
            previousToken = currentToken;
            currentToken = getToken();
        }

        if (currentToken.type == TOK_Newline) {
            if (!canHaveNewlineAfter(previousToken.type)) {
                // I got one more token then I should have, give it back
                *overlapTokenOut = currentToken;
                break;
            }
        }
        else if (!isValidExpToken(currentToken.type)) {
            // I got one more token then I should have, give it back
            *overlapTokenOut = currentToken;
            break;
        }

        if (currentToken.type == TOK_Int_Literal || currentToken.type == TOK_Float_Literal ||
            currentToken.type == TOK_String_Literal || currentToken.type == TOK_Identifier) {
            TokenBufferPush(outputQueue, currentToken);
        }
        else if (currentToken.type == TOK_L_Paren) {
            if (previousToken.type != TOK_Empty) {
                if (previousToken.type == TOK_Int_Literal || previousToken.type == TOK_Float_Literal || previousToken.type == TOK_String_Literal || previousToken.type == TOK_Identifier ) {
                    throwError(SYNTAX_ERROR, "Invalid expression.\n", true);

                    TokenBufferDispose(&operatorStack);
                    TokenBufferDispose(&outputQueue);

                    return false;
                }
            }

            TokenBufferPush(operatorStack, currentToken);
        }
        else if (isOperator(currentToken.type)) {
            if (operatorStack->count > 0) {
                Token previous = TokenBufferPop(operatorStack);

                if (previous.type != TOK_L_Paren && getPriority(currentToken.type) <= getPriority(previous.type)) {
                    TokenBufferPush(outputQueue, previous);
                }
                else {
                    TokenBufferPush(operatorStack, previous);
                }
            }

            TokenBufferPush(operatorStack, currentToken);
        }
        else if (currentToken.type == TOK_R_Paren) {
            if (previousToken.type != TOK_Empty) {
                if (isOperator(previousToken.type)) {
                    throwError(SYNTAX_ERROR, "Invalid expression.\n", true);

                    TokenBufferDispose(&operatorStack);
                    TokenBufferDispose(&outputQueue);

                    return false;
                }
            }

            if (operatorStack->count == 0) {
                throwError(SYNTAX_ERROR, "Unpaired parentheses error.\n", true);

                TokenBufferDispose(&operatorStack);
                TokenBufferDispose(&outputQueue);

                return false;
            }

            while (TokenBufferTop(operatorStack).type != TOK_L_Paren) {
                Token token = TokenBufferPop(operatorStack);
                TokenBufferPush(outputQueue, token);

                if (operatorStack->count == 0) {
                    throwError(SYNTAX_ERROR, "Unpaired parentheses error.\n", true);

                    TokenBufferDispose(&operatorStack);
                    TokenBufferDispose(&outputQueue);

                    return false;
                }
            }

            // pop left paren
            TokenBufferPop(operatorStack);
        }        
    }

    while (operatorStack->count > 0) {
        Token token = TokenBufferPop(operatorStack);

        if (token.type == TOK_L_Paren) {
            throwError(SYNTAX_ERROR, "Unpaired parentheses error.\n", true);

            TokenBufferDispose(&operatorStack);
            TokenBufferDispose(&outputQueue);

            return false;
        }

        TokenBufferPush(outputQueue, token);
    }

    ASTNode *expHead = verifyOutput(outputQueue);

    bool ret = false;

    if (expHead != NULL) {
        ret = true;
        AST_AttachNode(expRoot, expHead);
    }

    TokenBufferDispose(&operatorStack);
    TokenBufferDispose(&outputQueue);

    return ret;
}

ASTNode* verifyOutput(TokenBuffer *outputQueue) {
    NodeBuffer *nodeBuffer = malloc(sizeof(NodeBuffer));
    nodeBuffer->count = outputQueue->count;
    nodeBuffer->nodes = malloc(sizeof(ASTNode*) * outputQueue->count);

    for (int i = 0; i < outputQueue->count; i++) {
        switch (outputQueue->tokens[i].type) {
            case TOK_Int_Literal:
                nodeBuffer->nodes[i] = AST_CreateIntNode(NULL, NODE_Literal_Int, outputQueue->tokens[i].i);
                nodeBuffer->nodes[i]->valueType = TAG_Int;
                break;
            case TOK_Float_Literal:
                nodeBuffer->nodes[i] = AST_CreateFloatNode(NULL, NODE_Literal_Float, outputQueue->tokens[i].f);
                nodeBuffer->nodes[i]->valueType = TAG_Float;
                break;
            case TOK_String_Literal:
                nodeBuffer->nodes[i] = AST_CreateStringNode(NULL, NODE_Literal_String, outputQueue->tokens[i].str);
                nodeBuffer->nodes[i]->valueType = TAG_String;
                break;
            case TOK_Identifier:
                nodeBuffer->nodes[i] = AST_CreateStringNode(NULL, NODE_Identifier, outputQueue->tokens[i].str);
                nodeBuffer->nodes[i]->valueType = TAG_Unknown;
                break;
            case TOK_Add:
                nodeBuffer->nodes[i] = AST_CreateNode(NULL, NODE_Add);
                break;
            case TOK_Sub:
                nodeBuffer->nodes[i] = AST_CreateNode(NULL, NODE_Sub);
                break;
            case TOK_Mul:
                nodeBuffer->nodes[i] = AST_CreateNode(NULL, NODE_Mul);
                break;
            case TOK_Div:
                nodeBuffer->nodes[i] = AST_CreateNode(NULL, NODE_Div);
                break;
            case TOK_Less_Then:
                nodeBuffer->nodes[i] = AST_CreateNode(NULL, NODE_Less_Then);
                break;
            case TOK_More_Then:
                nodeBuffer->nodes[i] = AST_CreateNode(NULL, NODE_More_Then);
                break;
            case TOK_Less_Equal_Then:
                nodeBuffer->nodes[i] = AST_CreateNode(NULL, NODE_Less_Equal_Then);
                break;
            case TOK_More_Equal_Then:
                nodeBuffer->nodes[i] = AST_CreateNode(NULL, NODE_More_Equal_Then);
                break;
            case TOK_Equal:
                nodeBuffer->nodes[i] = AST_CreateNode(NULL, NODE_Equal);
                break;
            case TOK_Not_Equal:
                nodeBuffer->nodes[i] = AST_CreateNode(NULL, NODE_Not_Equal);
                break;
        }
    }

    for (int i = 0; nodeBuffer->count > 1; i++) {
        ASTNode *currentNode = nodeBuffer->nodes[i];

        if (i >= nodeBuffer->count ) {
            free(nodeBuffer->nodes);
            free(nodeBuffer);

            return NULL;
        }

        if (isNodeOperator(currentNode->type) && !currentNode->isOperatorResult) {
            if (!NodeBufferCollapse(nodeBuffer, i)) {
                free(nodeBuffer->nodes);
                free(nodeBuffer);

                return NULL;
            }
            i = 0;
        }
    }    

    if (nodeBuffer->count == 1) {
        ASTNode *returnNode = nodeBuffer->nodes[0];

        free(nodeBuffer->nodes);
        free(nodeBuffer);

        return returnNode;
    }
    else {
        free(nodeBuffer->nodes);
        free(nodeBuffer);

        return NULL;
    }
}

bool NodeBufferCollapse(NodeBuffer *buffer, int pos) {
    if (pos < 2 || buffer->count < 3) {
        return false;
    }
    else {
        // both should be numbers
        if ((isNodeOperator(buffer->nodes[pos - 2]->type) && !buffer->nodes[pos - 2]->isOperatorResult)
         || (isNodeOperator(buffer->nodes[pos - 1]->type) && !buffer->nodes[pos - 1]->isOperatorResult)) {
            return false;
        } 

        ASTNode *leftOperand  = buffer->nodes[pos - 2];
        ASTNode *rightOperand = buffer->nodes[pos - 1];
        ASTNode *operator     = buffer->nodes[  pos  ];

        // check division by zero
        if (rightOperand->type == NODE_Literal_Int && rightOperand->content.i == 0) {
            throwError(ZERO_DIVISION_ERROR, "Division by zero error\n", true);
        }
        else if (rightOperand->type == NODE_Literal_Float && rightOperand->content.f == 0.0) {
            throwError(ZERO_DIVISION_ERROR, "Division by zero error\n", true);
        }

        AST_AttachNode(operator, leftOperand);
        AST_AttachNode(operator, rightOperand);

        operator->isOperatorResult = true;
    
        for (int i = pos + 1; i < buffer->count; i++) {
            buffer->nodes[i - 2] = buffer->nodes[i];
        }

        buffer->nodes[pos - 2] = operator;

        buffer->count -= 2;

        return true;
    }
}

// priorities are reversed to project assignment
int getPriority(tokenType type) {
    switch (type) {
        case TOK_Mul:
        case TOK_Div:
            return 3;
        case TOK_Add:
        case TOK_Sub:
            return 2;
        case TOK_Less_Then:
        case TOK_Less_Equal_Then:
        case TOK_More_Then:
        case TOK_More_Equal_Then:
        case TOK_Equal:
        case TOK_Not_Equal:
            return 1;
    }
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

bool canHaveNewlineAfter(tokenType type) {
    return isOperator(type) || type == TOK_L_Paren || type == TOK_Newline;
}
