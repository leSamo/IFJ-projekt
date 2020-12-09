/* ======== codeGenerator.c ========
 * Project: IFJ 2020/21 project
 * Team: 067, variant I
 * Author: Samuel Olekšák (xoleks00)
 * Date: November, December 2020
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
    printf("LABEL *main\n");
    printf("CREATEFRAME\n");
    printf("PUSHFRAME # func main LF\n\n");

    ASTNode *mainBlockNode = AST_GetChildOfType(mainNode, NODE_Block);

    IntBuffer *mainScope = IntBufferCreate();
    IntBufferPush(mainScope, mainBlockNode->id);

    for (int i = 0; i < mainBlockNode->childrenCount; i++) {
        generateStructure(mainBlockNode->children[i], symtable, *mainScope);
    }

    printf("\nPOPFRAME # func main LF\n");
    printf("EXIT int@0\n");

    // generate other functions
    for (int i = 0; i < astRoot->childrenCount; i++) {
        ASTNode *funcDefNode = astRoot->children[i];

        if (strcmp(funcDefNode->content.str, "main") != 0) {
            ASTNode *funcDefBlockNode = AST_GetChildOfType(funcDefNode, NODE_Block);
            ASTNode *funcDefParamsNode = AST_GetChildOfType(funcDefNode, NODE_Func_Def_Param_List);

            IntBuffer *blockScope = IntBufferCreate();

            printf("LABEL *%s\n", funcDefNode->content.str);
            printf("PUSHFRAME\n");

            for (int i = 0; i < funcDefParamsNode->childrenCount; i++) {
                printf("DEFVAR LF@%s\n", funcDefParamsNode->children[i]->children[0]->content.str);
                printf("MOVE LF@%s LF@!arg%d\n", funcDefParamsNode->children[i]->children[0]->content.str, i);
            }

            generateBlock(funcDefNode, symtable, *blockScope);

            printf("POPFRAME\n");
            printf("RETURN\n");
        }
    }

    IntBufferDispose(&mainScope);
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
        case NODE_For:
            generateFor(node, symtable, scope);
            break;
        case NODE_Block:
            IntBufferPush(&scope, node->id);
            generateBlock(node, symtable, scope);
            break;
        case NODE_Return:
            generateReturn(node, symtable, scope);
            break;
    }
}

void generateReturn(ASTNode *node, ST_Node *symtable, IntBuffer scope) {
    ASTNode *parentFunctionNode = AST_GetParentOfType(node, NODE_Func_Def);
    ASTNode *returnParamsNode = AST_GetChildOfType(parentFunctionNode, NODE_Func_Def_Return);

    if (strcmp(parentFunctionNode->content.str, "main") == 0) {
        printf("EXIT int@0\n");
    }
    else {
        for (int i = 0; i < node->childrenCount; i++) {
            char expId[16];
            sprintf(expId, "!ret%d", i);

            printf("DEFVAR LF@%s\n", expId);
            generateExpression(node->children[i], symtable, scope, expId, true);
        }

        printf("POPFRAME\n");
        printf("RETURN\n");
    }
}

void generateBlock(ASTNode *node, ST_Node *symtable, IntBuffer scope) {
    for (int i = 0; i < node->childrenCount; i++) {
        generateStructure(node->children[i], symtable, scope);
    }
}

void generateFor(ASTNode *node, ST_Node *symtable, IntBuffer scope) {
    ASTNode *blockNode = AST_GetChildOfType(node, NODE_Block);
    ASTNode *defineNode = AST_GetChildOfType(node, NODE_Define);
    ASTNode *expNode = AST_GetChildOfType(node, NODE_Exp);
    ASTNode *assignNode = AST_GetChildOfType(node, NODE_Assign); // there may be multiple assignments

    int localId = consecutiveLabelId++;
    
    IntBufferPush(&scope, node->id); // for header scope

    char expId[14];
    sprintf(expId, "!forexp_%d", localId);

    if (defineNode != NULL) {
        generateDefinition(defineNode, symtable, scope); // for definition
    }

    printf("DEFVAR LF@"); // expression result temp
    IntBufferPrefix(&scope);
    printf("%s\n", expId); // expression result temp

    printf("\nLABEL !for_start_%d\n", localId);

    generateExpression(expNode, symtable, scope, expId, false);
    printf("JUMPIFNEQ !for_end_%d LF@", localId);
    IntBufferPrefix(&scope);
    printf("!forexp_%d bool@true\n", localId);

    IntBufferPush(&scope, blockNode->id);
    generateBlock(blockNode, symtable, scope); // TODO: Update scope

    scope.count--;
    if (assignNode != NULL) {
        generateAssignment(assignNode, symtable, scope); // for assignment
    }

    printf("JUMP !for_start_%d\n", localId);
    printf("\nLABEL !for_end_%d\n", localId);
}

void generateIfElse(ASTNode *node, ST_Node *symtable, IntBuffer scope) {
    ASTNode *expNode = AST_GetChildOfType(node, NODE_Exp);
    ASTNode *ifBlock = node->children[1];
    ASTNode *elseBlock = node->children[2];

    int localId = consecutiveLabelId++;

    char expId[14];
    sprintf(expId, "!exp_%d", localId);

    printf("DEFVAR LF@"); // expression result temp
    IntBufferPrefix(&scope);
    printf("%s\n", expId); // expression result temp

    generateExpression(expNode, symtable, scope, expId, false);
    printf("JUMPIFNEQ !neg_%d LF@", localId);
    IntBufferPrefix(&scope);
    printf("!exp_%d bool@true\n", localId);

    printf("LABEL !pos_%d\n", localId);
    generateStructure(ifBlock, symtable, scope);
    printf("JUMP !end_%d\n", localId);

    printf("LABEL !neg_%d\n", localId);
    generateStructure(elseBlock, symtable, scope);

    printf("LABEL !end_%d\n", localId);
}

void generateDefinition(ASTNode *defineNode, ST_Node *symtable, IntBuffer scope) {
    ASTNode *idNode = AST_GetChildOfType(defineNode, NODE_Identifier);
    ASTNode *expNode = defineNode->children[1];
    
    printf("DEFVAR LF@");
    IntBufferPrefix(&scope);
    printf("%s\n", idNode->content.str);

    generateExpression(expNode, symtable, scope, idNode->content.str, false);
}

void generateAssignment(ASTNode *assignNode, ST_Node *symtable, IntBuffer scope) {
    ASTNode *idNode = AST_GetChildOfType(assignNode, NODE_Identifier);
    ASTNode *leftSideNode = assignNode->children[0];
    ASTNode *rightSideNode = assignNode->children[1];
    
    if (rightSideNode->type == NODE_Exp) {
        generateExpression(rightSideNode, symtable, scope, idNode->content.str, false);
    }
    else if (rightSideNode->type == NODE_Func_Call) {
        if (strcmp(rightSideNode->content.str, "inputs") == 0) {
            ASTNode *outputStringNode = leftSideNode->children[0];
            ASTNode *outputErrNode = leftSideNode->children[1];

            // no need to check type, string takes anything
            printf("READ LF@%s string\n", outputStringNode->content.str);
        }
        else if (strcmp(rightSideNode->content.str, "inputi") == 0) {
            ASTNode *outputIntNode = leftSideNode->children[0];
            ASTNode *outputErrNode = leftSideNode->children[1];

            printf("\nCREATEFRAME\n");
            printf("DEFVAR TF@res%d\n", consecutiveLabelId);
            printf("DEFVAR TF@type%d\n", consecutiveLabelId);

            printf("READ TF@res%d int\n", consecutiveLabelId);
            printf("TYPE TF@type%d TF@res%d\n", consecutiveLabelId, consecutiveLabelId);

            printf("JUMPIFNEQ !error%d string@int TF@type%d\n", consecutiveLabelId, consecutiveLabelId);
            printf("MOVE LF@%s TF@res%d\n", outputIntNode->content.str, consecutiveLabelId);

            printf("JUMP !end%d\n", consecutiveLabelId);
            printf("LABEL !error%d\n", consecutiveLabelId);

            if (strcmp(outputErrNode->content.str, "_") != 0) {
                printf("MOVE LF@%s int@1\n", outputErrNode->content.str);
            }

            printf("LABEL !end%d\n", consecutiveLabelId++);
            printf("CLEARS\n\n");
        }
        else if (strcmp(rightSideNode->content.str, "inputf") == 0) {
            ASTNode *outputFloatNode = leftSideNode->children[0];
            ASTNode *outputErrNode = leftSideNode->children[1];

            printf("\nCREATEFRAME\n");
            printf("DEFVAR TF@res%d\n", consecutiveLabelId);
            printf("DEFVAR TF@type%d\n", consecutiveLabelId);

            printf("READ TF@res%d float\n", consecutiveLabelId);
            printf("TYPE TF@type%d TF@res%d\n", consecutiveLabelId, consecutiveLabelId);

            printf("JUMPIFNEQ !error%d string@float TF@type%d\n", consecutiveLabelId, consecutiveLabelId);
            printf("MOVE LF@%s TF@res%d\n", outputFloatNode->content.str, consecutiveLabelId);

            printf("JUMP !end%d\n", consecutiveLabelId);
            printf("LABEL !error%d\n", consecutiveLabelId);

            if (strcmp(outputErrNode->content.str, "_") != 0) {
                printf("MOVE LF@%s int@1\n", outputErrNode->content.str);
            }

            printf("LABEL !end%d\n", consecutiveLabelId++);
            printf("CLEARS\n\n");
        }
        else if (strcmp(rightSideNode->content.str, "len") == 0) {
            ASTNode *outputIntNode = leftSideNode;
            ASTNode *inputStringNode = rightSideNode->children[0];

            printf("STRLEN LF@%s LF@%s\n", outputIntNode->content.str, inputStringNode->content.str);
        }
        else if (strcmp(rightSideNode->content.str, "ord") == 0) {
            ASTNode *inputStringNode = rightSideNode->children[0];
            ASTNode *inputPositionNode = rightSideNode->children[1];
    
            ASTNode *outputAsciiNode = leftSideNode->children[0];
            ASTNode *outputErrorNode = leftSideNode->children[1];

            // get input string length - 1
            printf("DEFVAR LF@!len%d\n", consecutiveLabelId);
            printf("STRLEN LF@!len%d ", consecutiveLabelId);
            printTerm(inputStringNode, symtable, scope, "LF");
            printf("\n");
            printf("SUB LF@!len%d LF@!len%d int@1\n", consecutiveLabelId, consecutiveLabelId);

            // check if pos isn't less than 0
            printf("DEFVAR LF@!isErr%d\n", consecutiveLabelId);
            printf("GT LF@!isErr%d int@0 ", consecutiveLabelId);
            printTerm(inputPositionNode, symtable, scope, "LF");
            printf("\n");
            printf("JUMPIFEQ !err%d LF@!isErr%d bool@true\n", consecutiveLabelId, consecutiveLabelId);

            // check if pos isn't more than strlen - 1
            printf("LT LF@!isErr%d LF@!len%d ", consecutiveLabelId, consecutiveLabelId);
            printTerm(inputPositionNode, symtable, scope, "LF");
            printf("\n");
            printf("JUMPIFEQ !err%d LF@!isErr%d bool@true\n", consecutiveLabelId, consecutiveLabelId);

            // no error branch
            printf("MOVE LF@%s int@0\n", outputErrorNode->content.str);

            printf("STRI2INT LF@%s ", outputAsciiNode->content.str);
            printTerm(inputStringNode, symtable, scope, "LF");
            printf(" ");
            printTerm(inputPositionNode, symtable, scope, "LF");
            printf("\n");

            printf("JUMP !end%d\n", consecutiveLabelId);
            printf("LABEL !err%d\n", consecutiveLabelId);

            // error branch
            printf("MOVE LF@%s int@1\n", outputErrorNode->content.str);

            printf("LABEL !end%d\n", consecutiveLabelId);

            consecutiveLabelId++;
        }
        else if (strcmp(rightSideNode->content.str, "chr") == 0) {
            ASTNode *inputAsciiNode = rightSideNode->children[0];
    
            ASTNode *outputCharNode = leftSideNode->children[0];
            ASTNode *outputErrorNode = leftSideNode->children[1];

            // check if ascii isn't less than 0
            printf("DEFVAR LF@!isErr%d\n", consecutiveLabelId);
            printf("GT LF@!isErr%d int@0 ", consecutiveLabelId);
            printTerm(inputAsciiNode, symtable, scope, "LF");
            printf("\n");
            printf("JUMPIFEQ !err%d LF@!isErr%d bool@true\n", consecutiveLabelId, consecutiveLabelId);

            // check if ascii isn't more than 255
            printf("LT LF@!isErr%d int@255 ", consecutiveLabelId, consecutiveLabelId);
            printTerm(inputAsciiNode, symtable, scope, "LF");
            printf("\n");
            printf("JUMPIFEQ !err%d LF@!isErr%d bool@true\n", consecutiveLabelId, consecutiveLabelId);

            // no error branch
            printf("MOVE LF@%s int@0\n", outputErrorNode->content.str);

            printf("INT2Char LF@%s ", outputCharNode->content.str);
            printTerm(inputAsciiNode, symtable, scope, "LF");
            printf("\n");

            printf("JUMP !end%d\n", consecutiveLabelId);
            printf("LABEL !err%d\n", consecutiveLabelId);

            // error branch
            printf("MOVE LF@%s int@1\n", outputErrorNode->content.str);

            printf("LABEL !end%d\n", consecutiveLabelId);

            consecutiveLabelId++;
        }
        else if (strcmp(rightSideNode->content.str, "substr") == 0) {
            ASTNode *inputStringNode = rightSideNode->children[0];
            ASTNode *inputStartIndexNode = rightSideNode->children[1];
            ASTNode *inputLengthNode = rightSideNode->children[2];
    
            ASTNode *outputStringNode = leftSideNode->children[0];
            ASTNode *outputErrorNode = leftSideNode->children[1];

            // get input string length - 1
            printf("DEFVAR LF@!len%d\n", consecutiveLabelId);
            printf("STRLEN LF@!len%d ", consecutiveLabelId);
            printTerm(inputStringNode, symtable, scope, "LF");
            printf("\n");
            printf("SUB LF@!len%d LF@!len%d int@1\n", consecutiveLabelId, consecutiveLabelId);

            // check if substring length isn't less than 0
            printf("DEFVAR LF@!isErr%d\n", consecutiveLabelId);
            printf("GT LF@!isErr%d int@0 ", consecutiveLabelId);
            printTerm(inputLengthNode, symtable, scope, "LF");
            printf("\n");
            printf("JUMPIFEQ !err%d LF@!isErr%d bool@true\n", consecutiveLabelId, consecutiveLabelId);

            // check if start index isn't less than 0
            printf("GT LF@!isErr%d int@0 ", consecutiveLabelId);
            printTerm(inputStartIndexNode, symtable, scope, "LF");
            printf("\n");
            printf("JUMPIFEQ !err%d LF@!isErr%d bool@true\n", consecutiveLabelId, consecutiveLabelId);

            // check if start index isn't more than strlen - 1
            printf("LT LF@!isErr%d LF@!len%d ", consecutiveLabelId, consecutiveLabelId);
            printTerm(inputStartIndexNode, symtable, scope, "LF");
            printf("\n");
            printf("JUMPIFEQ !err%d LF@!isErr%d bool@true\n", consecutiveLabelId, consecutiveLabelId);

            // temp result string variable
            printf("DEFVAR LF@string%d\n", consecutiveLabelId);
            printf("MOVE LF@string%d string@\n", consecutiveLabelId);

            // save start index to temp variable
            printf("DEFVAR LF@startIndex%d\n", consecutiveLabelId);
            printf("MOVE LF@startIndex%d ", consecutiveLabelId);
            printTerm(inputStartIndexNode, symtable, scope, "LF");
            printf("\n");

            // no error branch
            if (strcmp(outputErrorNode->content.str, "_") != 0) {
                printf("MOVE LF@%s int@0\n", outputErrorNode->content.str);
            }

            // calculate end index
            printf("DEFVAR LF@!endIndex%d\n", consecutiveLabelId);
            printf("ADD LF@!endIndex%d ", consecutiveLabelId);
            printTerm(inputStartIndexNode, symtable, scope, "LF");
            printf(" ");
            printTerm(inputLengthNode, symtable, scope, "LF");
            printf("\n");

            // temp variable to store currently moved char
            printf("DEFVAR LF@!currChar%d\n", consecutiveLabelId);

            // loop which gets one char at the time and appends it to the result
            printf("LABEL !loop%d\n", consecutiveLabelId);

            // get char store it in temp
            printf("GETCHAR LF@!currChar%d ", consecutiveLabelId);
            printTerm(inputStringNode, symtable, scope, "LF");
            printf(" LF@startIndex%d\n", consecutiveLabelId);

            // append temp char to result
            printf("CONCAT LF@string%d LF@string%d LF@!currChar%d\n", consecutiveLabelId, consecutiveLabelId, consecutiveLabelId);

            // increment start index by one
            printf("ADD LF@startIndex%d LF@startIndex%d int@1\n", consecutiveLabelId, consecutiveLabelId);

            // if start index != end index do another iteration
            printf("JUMPIFNEQ !loop%d LF@startIndex%d LF@!endIndex%d\n", consecutiveLabelId, consecutiveLabelId, consecutiveLabelId);

            // move substring from temp to result
            if (strcmp(outputStringNode->content.str, "_") != 0) {
                printf("MOVE LF@%s LF@string%d\n", outputStringNode->content.str, consecutiveLabelId);
            }

            printf("JUMP !end%d\n", consecutiveLabelId);
            printf("LABEL !err%d\n", consecutiveLabelId);

            // error branch
            if (strcmp(outputErrorNode->content.str, "_") != 0) {
                printf("MOVE LF@%s int@1\n", outputErrorNode->content.str);
            }

            printf("LABEL !end%d\n", consecutiveLabelId);

            consecutiveLabelId++;
        }
        else if (strcmp(rightSideNode->content.str, "int2float") == 0) {
            ASTNode *outputFloatNode = leftSideNode;
            ASTNode *inputIntNode = rightSideNode->children[0];

            printf("INT2FLOAT LF@%s LF@%s\n", outputFloatNode->content.str, inputIntNode->content.str);
        }
        else if (strcmp(rightSideNode->content.str, "float2int") == 0) {
            ASTNode *outputIntNode = leftSideNode;
            ASTNode *inputFloatNode = rightSideNode->children[0];

            printf("FLOAT2INT LF@%s LF@%s\n", outputIntNode->content.str, inputFloatNode->content.str);
        }
        else {
            if (leftSideNode->type == NODE_Identifier) { // one return value
                generateFuncCall(rightSideNode, symtable, scope);

                if (strcmp(leftSideNode->content.str, "_") != 0) {
                    printf("MOVE ");
                    printVar(leftSideNode->content.str, symtable, scope, "LF");
                    printf(" TF@!ret0\n");
                }

                printf("CLEARS\n");
            }
            else if (leftSideNode->type == NODE_Multi_L_Value) { // more return values
                generateFuncCall(rightSideNode, symtable, scope);

                for (int i = 0; i < leftSideNode->childrenCount; i++) {
                    if (strcmp(leftSideNode->children[i]->content.str, "_") == 0) {
                        continue;
                    }

                    printf("MOVE ");
                    printVar(leftSideNode->children[i]->content.str, symtable, scope, "LF");
                    printf(" TF@!ret%d\n", i);
                }

                printf("CLEARS\n");
            }
        }
    }
}

void generateFuncCall(ASTNode *funcCallNode, ST_Node *symtable, IntBuffer scope) {
    if (strcmp(funcCallNode->content.str, "print") == 0) {
        for (int i = 0; i < funcCallNode->childrenCount; i++) {
            printf("WRITE ");
            printTerm(funcCallNode->children[i], symtable, scope, "LF");
            printf("\n");
        }
    }
    else {
        printf("CREATEFRAME\n");

        for (int i = 0; i < funcCallNode->childrenCount; i++) {
            printf("DEFVAR TF@!arg%d\n", i);

            printf("MOVE TF@!arg%d ", i);
            printTerm(funcCallNode->children[i], symtable, scope, "LF");
            printf("\n");
        }

        printf("CALL *%s\n", funcCallNode->content.str);
    }
}

void generateExpression(ASTNode *expNode, ST_Node *symtable, IntBuffer scope, char *resultId, bool tempFrame) {
    if (strcmp(resultId, "_") == 0) {
        return;
    }

    if (!isNodeOperator(expNode->children[0]->type)) {
        printf("MOVE ");
        if (tempFrame) {
            printf("LF@%s", resultId);
        }
        else {
            printVar(resultId, symtable, scope, "LF");
        }

        printf(" ");
        printTerm(expNode->children[0], symtable, scope, "LF");
        printf("\n");
    }
    else {
        printf("\nCREATEFRAME\n"); // init TF
        
        generateExpressionRecursively(expNode->children[0], symtable, scope, "$t");

        printf("MOVE ");
        if (tempFrame) {
            printf("LF@%s", resultId);
        }
        else {
            printVar(resultId, symtable, scope, "LF");
        }
        printf(" TF@$t\n");

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
            if (ST_CheckExpressionType(parentNode->children[0], &symtable, TAG_String, scope)) {
                printf("CONCAT TF@%s ", resultId);
            }
            else {
                printf("ADD TF@%s ", resultId);
            }
            printOperands(parentNode, symtable, scope, leftChild, rightChild);
            break;
        case NODE_Sub:
            printf("SUB TF@%s ", resultId);
            printOperands(parentNode, symtable, scope, leftChild, rightChild);
            break;
        case NODE_Mul:
            printf("MUL TF@%s ", resultId);
            printOperands(parentNode, symtable, scope, leftChild, rightChild);
            break;
        case NODE_Div:
            if (ST_CheckExpressionType(parentNode->children[0], &symtable, TAG_Int, scope)) {
                printf("IDIV TF@%s ", resultId);
            }
            else {
                printf("DIV TF@%s ", resultId);
            }
            printOperands(parentNode, symtable, scope, leftChild, rightChild);
            break;
        case NODE_Equal:
            printf("EQ TF@%s ", resultId);
            printOperands(parentNode, symtable, scope, leftChild, rightChild);
            break;
        case NODE_Not_Equal:
            printf("DEFVAR TF@%s1\n", resultId);
            printf("EQ TF@%s1 ", resultId);
            printOperands(parentNode, symtable, scope, leftChild, rightChild);
            printf("NOT TF@%s TF@%s1\n", resultId, resultId);
            break;
        case NODE_Less_Than:
            printf("LT TF@%s ", resultId);
            printOperands(parentNode, symtable, scope, leftChild, rightChild);
            break;
        case NODE_Greater_Than:
            printf("GT TF@%s ", resultId);
            printOperands(parentNode, symtable, scope, leftChild, rightChild);
            break;
        case NODE_Less_Equal_Than:
            printf("DEFVAR TF@%s1\n", resultId);
            printf("DEFVAR TF@%s2\n", resultId);
            printf("EQ TF@%s1 ", resultId);
            printOperands(parentNode, symtable, scope, leftChild, rightChild);
            printf("LT TF@%s2 ", resultId);
            printOperands(parentNode, symtable, scope, leftChild, rightChild);
            printf("OR TF@%s TF@%s1 TF@%s2\n", resultId, resultId, resultId);
            break;
        case NODE_Greater_Equal_Than:
            printf("DEFVAR TF@%s1\n", resultId);
            printf("DEFVAR TF@%s2\n", resultId);
            printf("EQ TF@%s1 ", resultId);
            printOperands(parentNode, symtable, scope, leftChild, rightChild);
            printf("GT TF@%s2 ", resultId);
            printOperands(parentNode, symtable, scope, leftChild, rightChild);
            printf("OR TF@%s TF@%s1 TF@%s2\n", resultId, resultId, resultId);
            break;
    }
}

void printOperands(ASTNode *parentNode, ST_Node *symtable, IntBuffer scope, char *leftChild, char *rightChild) {
    if (!isNodeOperator(parentNode->children[0]->type)) {
        printTerm(parentNode->children[0], symtable, scope, "LF");
    }
    else {
        printf("TF@%s", leftChild);
    }

    printf(" ");

    if (!isNodeOperator(parentNode->children[1]->type)) {
        printTerm(parentNode->children[1], symtable, scope, "LF");
    }
    else {
        printf("TF@%s", rightChild);
    }

    printf("\n");
}

void printVar(char *id, ST_Node *symtable, IntBuffer scopes, char *frame) {
    printf("%s@", frame);

    ST_Node *symbol = ST_Search(symtable, symtable, id, scopes);

    if (symbol != NULL) {
        IntBufferPrefix(symbol->scopes);
    }
    else {
        IntBufferPrefix(&scopes);
    }

    printf("%s", id);
}

void printTerm(ASTNode *node, ST_Node *symtable, IntBuffer scopes, char *frame) {
    switch (node->type) {
        case NODE_Literal_Int:
            printf("int@%ld", node->content.i);
            break;
        case NODE_Literal_Float:
            printf("float@%a", node->content.f);
            break;
        case NODE_Literal_String:
            printFormatedString(node->content.str);
            break;
        case NODE_Identifier:
            printf("%s@", frame);

            ST_Node *symbol = ST_Search(symtable, symtable, node->content.str, scopes);
            if (symbol != NULL) {
                IntBufferPrefix(symbol->scopes);
            }
            else {
                IntBufferPrefix(&scopes);
            }

            printf("%s", node->content.str);
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
