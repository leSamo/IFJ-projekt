/* ======== codeGenerator.c ========
 * Project: IFJ 2020/21 project
 * Team: 067, variant I
 * Author: Samuel Olekšák (xoleks00)
 * Date: November 2020
 * ================================= */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "codeGenerator.h"
#include "AST.c"
#include "symtable.c"
#include "intBuffer.c"

int consecutiveLabelId = 0;

void generateCode(ASTNode *astRoot, ASTNode *mainNode, ST_Node *symtable) {
    printf(".IFJcode20\n\n");

    ASTNode *mainBlockNode = AST_GetChildOfType(mainNode, NODE_Block);

    IntBuffer *mainScope = IntBufferCreate();
    IntBufferPush(mainScope, mainBlockNode->id);

    for (int i = 0; i < mainBlockNode->childrenCount; i++) {
        generateStructure(mainBlockNode->children[i], symtable, *mainScope);
    }

    //printf("EXIT 0\n");
}

void generateStructure(ASTNode *node, ST_Node *symtable, IntBuffer scope) {
    switch (node->type) {
        case NODE_Func_Call:
            generateFuncCall(node, symtable, scope);
            break;
        case NODE_Define:
            generateDefinition(node, symtable, scope);
            break;
        case NODE_Assign:
            generateAssignment(node, symtable, scope);
            break;
        case NODE_If_Else:
            generateIfElse(node, symtable, scope);
            break;
        case NODE_Block:
            for (int i = 0; i < node->childrenCount; i++) {
                generateStructure(node->children[i], symtable, scope);
            }
            break;
    }
}

void generateIfElse(ASTNode *node, ST_Node *symtable, IntBuffer scope) {
    ASTNode *expNode = AST_GetChildOfType(node, NODE_Exp);
    ASTNode *ifBlock = node->children[1];
    ASTNode *elseBlock = node->children[2];

    char expId[14];
    sprintf(expId, "!exp_%d", consecutiveLabelId);

    printf("DEFVAR GF@%s\n", expId);

    generateExpression(expNode, symtable, scope, expId);
    printf("JUMPIFNEQ !neg_%d GF@!exp_%d bool@true\n", consecutiveLabelId, consecutiveLabelId);

    printf("LABEL !pos_%d\n", consecutiveLabelId);
    generateStructure(ifBlock, symtable, scope);
    printf("JUMP !end_%d\n", consecutiveLabelId);

    printf("LABEL !neg_%d\n", consecutiveLabelId);
    generateStructure(elseBlock, symtable, scope);

    printf("LABEL !end_%d\n", consecutiveLabelId++);
}

void generateDefinition(ASTNode *defineNode, ST_Node *symtable, IntBuffer scope) {
    ASTNode *idNode = AST_GetChildOfType(defineNode, NODE_Identifier);
    ASTNode *expNode = defineNode->children[1];
    
    printf("DEFVAR GF@%s\n", idNode->content.str);
    generateExpression(expNode, symtable, scope, idNode->content.str);
}

void generateAssignment(ASTNode *assignNode, ST_Node *symtable, IntBuffer scope) {
    ASTNode *idNode = AST_GetChildOfType(assignNode, NODE_Identifier);
    ASTNode *rightSideNode = assignNode->children[1];
    
    if (rightSideNode->type == NODE_Exp) {
        generateExpression(rightSideNode, symtable, scope, idNode->content.str);
    }
    // TODO: else can be a func call with one left value or multi left value
}

void generateFuncCall(ASTNode *funcCallNode, ST_Node *symtable, IntBuffer scope) {
    if (strcmp(funcCallNode->content.str, "print") == 0) {
        for (int i = 0; i < funcCallNode->childrenCount; i++) {
            printf("WRITE ");
            printTerm(funcCallNode->children[i]->type, funcCallNode->children[i]->content, "GF");
            printf("\n");
        }
    }
}

void generateExpression(ASTNode *expNode, ST_Node *symtable, IntBuffer scope, char *resultId) {
    if (!isNodeOperator(expNode->children[0]->type)) {
        printf("MOVE GF@%s ", resultId);
        printTerm(expNode->children[0]->type, expNode->children[0]->content, "GF");
        printf("\n");
    }
    else {
        printf("\nCREATEFRAME\n"); // init TF
        
        generateExpressionRecursively(expNode->children[0], symtable, scope, "$t");
        printf("MOVE GF@%s TF@$t\n", resultId);

        printf("CLEARS\n\n"); // clear TF
    }
}

void generateExpressionRecursively(ASTNode *parentNode, ST_Node *symtable, IntBuffer scope, char *resultId) {
    printf("DEFVAR TF@%s\n", resultId);
    
    char *leftChild = newString(strlen(resultId) + 1);
    strcpy(leftChild, resultId);
    leftChild[strlen(resultId)] = 'l';

    char *rightChild = newString(strlen(resultId) + 1);
    strcpy(rightChild, resultId);
    rightChild[strlen(resultId)] = 'r';

    if (isNodeOperator(parentNode->children[0]->type)) {
        generateExpressionRecursively(parentNode->children[0], symtable, scope, leftChild);
    }

    if (isNodeOperator(parentNode->children[1]->type)) {
        generateExpressionRecursively(parentNode->children[1], symtable, scope, rightChild);
    }
    
    switch (parentNode->type) {
        case NODE_Add:
            if (ST_CheckTermType(parentNode->children[0], &symtable, TAG_String, scope)) {
                printf("CONCAT TF@%s ", resultId);
            }
            else {
                printf("ADD TF@%s ", resultId);
            }
            printOperands(parentNode, leftChild, rightChild);
            break;
        case NODE_Sub:
            printf("SUB TF@%s ", resultId);
            printOperands(parentNode, leftChild, rightChild);
            break;
        case NODE_Mul:
            printf("MUL TF@%s ", resultId);
            printOperands(parentNode, leftChild, rightChild);
            break;
        case NODE_Div:
            if (ST_CheckTermType(parentNode->children[0], &symtable, TAG_Int, scope)) {
                printf("IDIV TF@%s ", resultId);
            }
            else {
                printf("DIV TF@%s ", resultId);
            }
            printOperands(parentNode, leftChild, rightChild);
            break;
        case NODE_Equal:
            printf("EQ TF@%s ", resultId);
            printOperands(parentNode, leftChild, rightChild);
            break;
        case NODE_Not_Equal:
            printf("DEFVAR TF@%s1\n", resultId);
            printf("EQ TF@%s1 ", resultId);
            printOperands(parentNode, leftChild, rightChild);
            printf("NOT TF@%s TF@%s1\n", resultId, resultId);
            break;
        case NODE_Less_Then:
            printf("LT TF@%s ", resultId);
            printOperands(parentNode, leftChild, rightChild);
            break;
        case NODE_More_Then:
            printf("GT TF@%s ", resultId);
            printOperands(parentNode, leftChild, rightChild);
            break;
        case NODE_Less_Equal_Then:
            printf("DEFVAR TF@%s1\n", resultId);
            printf("DEFVAR TF@%s2\n", resultId);
            printf("EQ TF@%s1 ", resultId);
            printOperands(parentNode, leftChild, rightChild);
            printf("LT TF@%s2 ", resultId);
            printOperands(parentNode, leftChild, rightChild);
            printf("OR TF@%s TF@%s1 TF@%s2\n", resultId, resultId, resultId);
            break;
        case NODE_More_Equal_Then:
            printf("DEFVAR TF@%s1\n", resultId);
            printf("DEFVAR TF@%s2\n", resultId);
            printf("EQ TF@%s1 ", resultId);
            printOperands(parentNode, leftChild, rightChild);
            printf("GT TF@%s2 ", resultId);
            printOperands(parentNode, leftChild, rightChild);
            printf("OR TF@%s TF@%s1 TF@%s2\n", resultId, resultId, resultId);
            break;
    }
}

void printOperands(ASTNode *parentNode, char *leftChild, char *rightChild) {
    if (!isNodeOperator(parentNode->children[0]->type)) {
        printTerm(parentNode->children[0]->type, parentNode->children[0]->content, "GF");
    }
    else {
        printf("TF@%s", leftChild);
    }

    printf(" ");

    if (!isNodeOperator(parentNode->children[1]->type)) {
        printTerm(parentNode->children[1]->type, parentNode->children[1]->content, "GF");
    }
    else {
        printf("TF@%s", rightChild);
    }

    printf("\n");
}

void printTerm(typeTag type, typeUnion content, char *frame) {
    switch (type) {
        case NODE_Literal_Int:
            printf("int@%ld", content.i);
            break;
        case NODE_Literal_Float:
            printf("float@%a", content.f);
            break;
        case NODE_Literal_String:
            printf("string@%s", content.str);
            break;
        case NODE_Identifier:
            printf("%s@%s", frame, content.str);
            break;
    }
}

void printFormatedString(char *string) {
    int size = strlen(string) * 7 + 8; // max length
    char result[size];
    memcpy(result, "string@", 8);
    char tmp[7];
    for (int i = 0; i < (int)strlen(string); i++) {

        if (0 <= string[i] && 9 >= string[i]) {
            sprintf(tmp, "\\00%d", string[i]);
        }
        else if ((10 <= string[i] && 32 >= string[i]) || (string[i] == 35) || (string[i] == 92)) {
            snprintf(tmp, 7, "\\0%d", string[i]);
        }
        else {
            snprintf(tmp, 2, "%s", (char[2]){string[i], '\0'});
        }
        strcat(result, tmp);
    }

    printf("%s", result);
    return;
}