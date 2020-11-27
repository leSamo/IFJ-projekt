/* ========= binTree.c ==========
 * Project: IFJ 2020/21 project
 * Team: 067, variant I
 * Author: Samuel Olekšák (xoleks00), Michal Findra (xfindr00)
 * Date: November 2020
 * Note: Part of this file was copied from xfindr00's IAL 2 project
 * ================================= */

#ifndef BINTREE
#define BINTREE

#include <stdbool.h>
#include <string.h>

#include "leParser.h"

void ST_Init(ST_Node **RootPtr) {
    *RootPtr = NULL;
    ST_SetupBuiltIn(RootPtr);
}

void ST_SetupBuiltIn(ST_Node **RootPtr) {
    // readonly _ variable
    ST_Insert(RootPtr, "_", SYM_Readonly, NULL);

    // func inputs() (string,int)
    ASTNode *inputs = AST_CreateStringNode(NULL, NODE_Func_Def, "inputs");
    AST_CreateNode(inputs, NODE_Func_Def_Params);
    ASTNode *inputsReturn = AST_CreateNode(inputs, NODE_Func_Def_Return);
    AST_CreateTaggedNode(inputsReturn, NODE_Type_String, TAG_String);
    AST_CreateTaggedNode(inputsReturn, NODE_Type_Int, TAG_Int);
    ST_Insert(RootPtr, "inputs", SYM_Func, inputs);

    // func inputi() (int,int)
    ASTNode *inputi = AST_CreateStringNode(NULL, NODE_Func_Def, "inputi");
    AST_CreateNode(inputi, NODE_Func_Def_Params);
    ASTNode *inputiReturn = AST_CreateNode(inputi, NODE_Func_Def_Return);
    AST_CreateTaggedNode(inputiReturn, NODE_Type_Int, TAG_Int);
    AST_CreateTaggedNode(inputiReturn, NODE_Type_Int, TAG_Int);
    ST_Insert(RootPtr, "inputi", SYM_Func, inputi);

    // func inputf() (float64,int)
    ASTNode *inputf = AST_CreateStringNode(NULL, NODE_Func_Def, "inputf");
    AST_CreateNode(inputf, NODE_Func_Def_Params);
    ASTNode *inputfReturn = AST_CreateNode(inputf, NODE_Func_Def_Return);
    AST_CreateTaggedNode(inputfReturn, NODE_Type_Float, TAG_Float);
    AST_CreateTaggedNode(inputfReturn, NODE_Type_Int, TAG_Int);
    ST_Insert(RootPtr, "inputf", SYM_Func, inputf);

    // func int2float(i int) (float64)
    ASTNode *int2float = AST_CreateStringNode(NULL, NODE_Func_Def, "int2float");
    ASTNode *int2floatParams = AST_CreateNode(int2float, NODE_Func_Def_Params);
    AST_CreateTaggedNode(int2floatParams, NODE_Type_Int, TAG_Int);
    ASTNode *int2floatReturn = AST_CreateNode(int2float, NODE_Func_Def_Return);
    AST_CreateTaggedNode(int2floatReturn, NODE_Type_Float, TAG_Float);
    ST_Insert(RootPtr, "int2float", SYM_Func, int2float);

    // func float2int(f float64) (int)
    ASTNode *float2int = AST_CreateStringNode(NULL, NODE_Func_Def, "float2int");
    ASTNode *float2intParams = AST_CreateNode(float2int, NODE_Func_Def_Params);
    AST_CreateTaggedNode(float2intParams, NODE_Type_Float, TAG_Float);
    ASTNode *float2intReturn = AST_CreateNode(float2int, NODE_Func_Def_Return);
    AST_CreateTaggedNode(float2intReturn, NODE_Type_Int, TAG_Int);
    ST_Insert(RootPtr, "float2int", SYM_Func, float2int);
}

ST_Node* ST_Search(ST_Node *RootPtr, char *searchedId) {
    if (RootPtr == NULL) {
        return NULL;
    }

    if (strcmp(searchedId, RootPtr->id) == 0) {
        return RootPtr;
    }

    if (strcmp(searchedId, RootPtr->id) < 0) {
        return ST_Search(RootPtr->RPtr, searchedId);
    }
    else {
        return ST_Search(RootPtr->LPtr, searchedId);
    }
}

void ST_Insert(ST_Node **RootPtr, char *id, symType type, ASTNode *node) {
    if (*RootPtr == NULL) {        
        ST_Node *new_leaf = malloc(sizeof(ST_Node));
        new_leaf->id = newString(strlen(id));
        
        // TODO: Check if malloc was successful

        strcpy(new_leaf->id, id);
        new_leaf->type = type;
        new_leaf->node = node;

        new_leaf->LPtr = NULL;
        new_leaf->RPtr = NULL;

        *RootPtr = new_leaf;
    }
    else if (strcmp(id, (*RootPtr)->id) == 0) { // tree already has node with key K
        printError(DEFINITION_TYPE_ERROR, "Variable redefinition error\n");
    }
    else if (strcmp(id, (*RootPtr)->id) < 0) {
        ST_Insert(&(*RootPtr)->RPtr, id, type, node);
    }
    else if (strcmp(id, (*RootPtr)->id) > 0) {
        ST_Insert(&(*RootPtr)->LPtr, id, type, node);
    }
}

void ST_Dispose(ST_Node **RootPtr) {
    if (*RootPtr != NULL) {
        ST_Dispose(&(*RootPtr)->RPtr);
        ST_Dispose(&(*RootPtr)->LPtr);
        free(*RootPtr);
    }
    
    *RootPtr = NULL;
}

void ST_PrettyPrint(ST_Node *nodePtr, int level) {
    if (nodePtr != NULL) {
        for (int i = 0; i < level; i++) {
            printf("  ");
        }

        printf("%s\n", nodePtr->id);

        ST_PrettyPrint(nodePtr->LPtr, level + 1);
        ST_PrettyPrint(nodePtr->RPtr, level + 1);
    }
}

#endif
