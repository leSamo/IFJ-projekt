/* ========== symtable.c ===========
 * Project: IFJ 2020/21 project
 * Team: 067, variant I
 * Author: Samuel Olekšák (xoleks00), Michal Findra (xfindr00)
 * Date: November 2020
 * Note: Part of this file was copied from xfindr00's IAL 2 project
 * ================================= */

#ifndef SYMTABLE
#define SYMTABLE

#include <stdbool.h>
#include <string.h>

#include "constants.h"
#include "symtable.h"
#include "leParser.h"
#include "intBuffer.c"

IntBuffer GLOBAL_SCOPE = {0,0,NULL};

void ST_Init(ST_Node **RootPtr) {
    *RootPtr = NULL;
    ST_SetupBuiltIn(RootPtr);
}

void ST_SetupBuiltIn(ST_Node **RootPtr) {

    ASTBuiltIn = AST_CreateNode(NULL, NODE_Empty);

    // writeonly _ variable
    ST_Insert(RootPtr, "_", TAG_Any, NULL, GLOBAL_SCOPE);

    // TODO: Create function node factory

    // func inputs() (string,int)
    ASTNode *inputs = AST_CreateStringNode(ASTBuiltIn, NODE_Func_Def, "inputs");
    AST_CreateNode(inputs, NODE_Func_Def_Param_List);
    ASTNode *inputsReturn = AST_CreateNode(inputs, NODE_Func_Def_Return);
    AST_CreateTaggedNode(inputsReturn, NODE_Type_String, TAG_String);
    AST_CreateTaggedNode(inputsReturn, NODE_Type_Int, TAG_Int);
    ST_Insert(RootPtr, "inputs", SYM_Func, inputs, GLOBAL_SCOPE);

    // func inputi() (int,int)
    ASTNode *inputi = AST_CreateStringNode(ASTBuiltIn, NODE_Func_Def, "inputi");
    AST_CreateNode(inputi, NODE_Func_Def_Param_List);
    ASTNode *inputiReturn = AST_CreateNode(inputi, NODE_Func_Def_Return);
    AST_CreateTaggedNode(inputiReturn, NODE_Type_Int, TAG_Int);
    AST_CreateTaggedNode(inputiReturn, NODE_Type_Int, TAG_Int);
    ST_Insert(RootPtr, "inputi", SYM_Func, inputi, GLOBAL_SCOPE);

    // func inputf() (float64,int)
    ASTNode *inputf = AST_CreateStringNode(ASTBuiltIn, NODE_Func_Def, "inputf");
    AST_CreateNode(inputf, NODE_Func_Def_Param_List);
    ASTNode *inputfReturn = AST_CreateNode(inputf, NODE_Func_Def_Return);
    AST_CreateTaggedNode(inputfReturn, NODE_Type_Float, TAG_Float);
    AST_CreateTaggedNode(inputfReturn, NODE_Type_Int, TAG_Int);
    ST_Insert(RootPtr, "inputf", SYM_Func, inputf, GLOBAL_SCOPE);

    // func print(term1 , term2 , … , termn)
    ASTNode *print = AST_CreateStringNode(ASTBuiltIn, NODE_Func_Def, "print");
    AST_CreateNode(print, NODE_Func_Def_Params_Variable);
    ASTNode *printReturn = AST_CreateNode(print, NODE_Func_Def_Return);
    ST_Insert(RootPtr, "print", SYM_Func, print, GLOBAL_SCOPE);

    // func int2float(i int) (float64)
    ASTNode *int2float = AST_CreateStringNode(ASTBuiltIn, NODE_Func_Def, "int2float");
    ASTNode *int2floatParams = AST_CreateNode(int2float, NODE_Func_Def_Param_List);
    AST_CreateTaggedNode(int2floatParams, NODE_Type_Int, TAG_Int);
    ASTNode *int2floatReturn = AST_CreateNode(int2float, NODE_Func_Def_Return);
    AST_CreateTaggedNode(int2floatReturn, NODE_Type_Float, TAG_Float);
    ST_Insert(RootPtr, "int2float", SYM_Func, int2float, GLOBAL_SCOPE);

    // func float2int(f float64) (int)
    ASTNode *float2int = AST_CreateStringNode(ASTBuiltIn, NODE_Func_Def, "float2int");
    ASTNode *float2intParams = AST_CreateNode(float2int, NODE_Func_Def_Param_List);
    AST_CreateTaggedNode(float2intParams, NODE_Type_Float, TAG_Float);
    ASTNode *float2intReturn = AST_CreateNode(float2int, NODE_Func_Def_Return);
    AST_CreateTaggedNode(float2intReturn, NODE_Type_Int, TAG_Int);
    ST_Insert(RootPtr, "float2int", SYM_Func, float2int, GLOBAL_SCOPE);

    // func len(s string) (int)
    ASTNode *len = AST_CreateStringNode(ASTBuiltIn, NODE_Func_Def, "len");
    ASTNode *lenParams = AST_CreateNode(len, NODE_Func_Def_Param_List);
    AST_CreateTaggedNode(lenParams, NODE_Type_String, TAG_String);
    ASTNode *lenReturn = AST_CreateNode(len, NODE_Func_Def_Return);
    AST_CreateTaggedNode(lenReturn, NODE_Type_Int, TAG_Int);
    ST_Insert(RootPtr, "len", SYM_Func, len, GLOBAL_SCOPE);

    // func substr(s string, i int, n int) (string, int)
    ASTNode *substr = AST_CreateStringNode(ASTBuiltIn, NODE_Func_Def, "substr");
    ASTNode *substrParams = AST_CreateNode(substr, NODE_Func_Def_Param_List);
    AST_CreateTaggedNode(substrParams, NODE_Type_String, TAG_String);
    AST_CreateTaggedNode(substrParams, NODE_Type_Int, TAG_Int);
    AST_CreateTaggedNode(substrParams, NODE_Type_Int, TAG_Int);
    ASTNode *substrReturn = AST_CreateNode(substr, NODE_Func_Def_Return);
    AST_CreateTaggedNode(substrReturn, NODE_Type_String, TAG_String);
    AST_CreateTaggedNode(substrReturn, NODE_Type_Int, TAG_Int);
    ST_Insert(RootPtr, "substr", SYM_Func, substr, GLOBAL_SCOPE);

    // func ord(s string, i int) (int, int)
    ASTNode *ord = AST_CreateStringNode(ASTBuiltIn, NODE_Func_Def, "ord");
    ASTNode *ordParams = AST_CreateNode(ord, NODE_Func_Def_Param_List);
    AST_CreateTaggedNode(ordParams, NODE_Type_String, TAG_String);
    AST_CreateTaggedNode(ordParams, NODE_Type_Int, TAG_Int);
    ASTNode *ordReturn = AST_CreateNode(ord, NODE_Func_Def_Return);
    AST_CreateTaggedNode(ordReturn, NODE_Type_Int, TAG_Int);
    AST_CreateTaggedNode(ordReturn, NODE_Type_Int, TAG_Int);
    ST_Insert(RootPtr, "ord", SYM_Func, ord, GLOBAL_SCOPE);

    // func chr(i int) (string, int)
    ASTNode *chr = AST_CreateStringNode(ASTBuiltIn, NODE_Func_Def, "chr");
    ASTNode *chrParams = AST_CreateNode(chr, NODE_Func_Def_Param_List);
    AST_CreateTaggedNode(chrParams, NODE_Type_Int, TAG_Int);
    ASTNode *chrReturn = AST_CreateNode(chr, NODE_Func_Def_Return);
    AST_CreateTaggedNode(chrReturn, NODE_Type_String, TAG_String);
    AST_CreateTaggedNode(chrReturn, NODE_Type_Int, TAG_Int);
    ST_Insert(RootPtr, "chr", SYM_Func, chr, GLOBAL_SCOPE);
}

ST_Node* ST_Search(ST_Node *RootPtr, ST_Node *CurrentPtr, char *searchedId, IntBuffer scopes) {
    if (CurrentPtr == NULL) {
        if (scopes.count > 0) {
            scopes.count--;
            return ST_Search(RootPtr, RootPtr, searchedId, scopes);
        }
        else {
            return NULL;
        }
    }

    if (strcmp(searchedId, CurrentPtr->id) == 0) {
        int scopeComparison = IntBufferCompare(*CurrentPtr->scopes, scopes);

        if (scopeComparison < 0) {
            return ST_Search(RootPtr, CurrentPtr->RPtr, searchedId, scopes);
        }
        else if (scopeComparison > 0) {
            return ST_Search(RootPtr, CurrentPtr->LPtr, searchedId, scopes);
        }
        else {
            return CurrentPtr;
        }
    }

    if (strcmp(searchedId, CurrentPtr->id) < 0) {
        return ST_Search(RootPtr, CurrentPtr->RPtr, searchedId, scopes);
    }
    else {
        return ST_Search(RootPtr, CurrentPtr->LPtr, searchedId, scopes);
    }
}

void ST_Insert(ST_Node **RootPtr, char *id, symType type, ASTNode *node, IntBuffer scopes) {
    if (*RootPtr == NULL) {        
        ST_Node *new_leaf = malloc(sizeof(ST_Node));
        new_leaf->id = newString(strlen(id));
        
        // TODO: Check if malloc was successful

        strcpy(new_leaf->id, id);
        new_leaf->scopes = IntBufferCreate();
        new_leaf->scopes->capacity = scopes.capacity;
        new_leaf->scopes->count = scopes.count;

        for (int i = 0; i < scopes.count; i++) {
            new_leaf->scopes->content[i] = scopes.content[i];
        }

        new_leaf->type = type;
        new_leaf->node = node;

        new_leaf->LPtr = NULL;
        new_leaf->RPtr = NULL;

        *RootPtr = new_leaf;
    }
    else if (strcmp(id, (*RootPtr)->id) == 0) { // tree already has node with this id
        int scopeComparison = IntBufferCompare(*(*RootPtr)->scopes, scopes);

        if (scopeComparison < 0) {
            ST_Insert(&(*RootPtr)->RPtr, id, type, node, scopes);
        }
        else if (scopeComparison > 0) {
            ST_Insert(&(*RootPtr)->LPtr, id, type, node, scopes);
        }
        else {
            throwError(DEFINITION_TYPE_ERROR, "Variable redefinition in the same scope error\n", false);
        }
    }
    else if (strcmp(id, (*RootPtr)->id) < 0) {
        ST_Insert(&(*RootPtr)->RPtr, id, type, node, scopes);
    }
    else if (strcmp(id, (*RootPtr)->id) > 0) {
        ST_Insert(&(*RootPtr)->LPtr, id, type, node, scopes);
    }
}

void ST_Dispose(ST_Node **RootPtr) {
    if (*RootPtr != NULL) {
        ST_Dispose(&(*RootPtr)->RPtr);
        ST_Dispose(&(*RootPtr)->LPtr);
        
        // IntBufferDispose(&(*RootPtr)->scopes);
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
