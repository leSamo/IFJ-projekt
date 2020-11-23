/* ============= AST.c =============
 * Project: IFJ 2020/21 project
 * Team: 067, variant I
 * Author: Samuel Olekšák (xoleks00), Michal Findra (xfindr00)
 * Date: November 2020
 * Description: N-ary abstract syntax tree for semantic analysis
 * ================================= */

#ifndef AST
#define AST

#include <stdlib.h>

#include "AST.h"

int AST_consecutiveId = 0;

ASTNode* AST_CreateNode(ASTNode *parent, ASTNodeType type) {
    ASTNode *node = malloc(sizeof(ASTNode));

    if (parent != NULL) {
        parent->children[parent->childrenCount++] = node;
    }

    node->id = AST_consecutiveId++;
    node->parent = parent;
    node->type = type;
    node->childrenCount = 0;
    
    for (int i = 0; i < AST_NODE_CHILDREN; i++) {
        node->children[i] = NULL;
    }

    return node;
}

void AST_Print(ASTNode *nodePtr) {
    printf("========================\n");
    printf("%s [%d]\n", AST_GetNodeName(nodePtr->type), nodePtr->id);

    if (nodePtr->parent != NULL) {
        printf("Parent: [%d]\n", nodePtr->parent->id);
    }

    // print children count
    printf("%d children ", nodePtr->childrenCount);

    for (int i = 0; i < nodePtr->childrenCount; i++) {
        printf("[%d] ", nodePtr->children[i]->id);
    }
    printf("\n");

    // recursivelly check children
    for (int i = 0; i < nodePtr->childrenCount; i++) {
        AST_Print(nodePtr->children[i]);
    }
}

void AST_Delete(ASTNode *nodePtr) {
    if (nodePtr != NULL) {
        for (int i = 0; i < nodePtr->childrenCount; i++) {
            AST_Delete(nodePtr->children[i]);
        }

        free(nodePtr);
    }
}

char* AST_GetNodeName(ASTNodeType type) {
    switch (type) {
        case NODE_Prog:
            return "Prog";
        case NODE_Block:
            return "Block";
        case NODE_Func_Def:
            return "Func def";
        case NODE_Func_Def_Params:
            return "Func def params";
        case NODE_Func_Def_Return:
            return "Func def return types";
        case NODE_Stat:
            return "Statement";
        case NODE_If_Else:
            return "If-else";
        case NODE_For:
            return "For loop";
        case NODE_Func_Call:
            return "Func call";
        case NODE_Return:
            return "Return";
        case NODE_Assign:
            return "Assignment";
        case NODE_Define:
            return "Definition";
        case NODE_Exp:
            return "Expression";
        case NODE_Type_Int:
            return "Type int";
        case NODE_Type_Float:
            return "Type float";
        case NODE_Type_String:
            return "Type string";
        case NODE_Identifier:
            return "Identifier";
        case NODE_Literal_Int:
            return "Int literal";
        case NODE_Literal_Float:
            return "Float literal";
        case NODE_Literal_String:
            return "String literal";
        default:
            return "Unknown AST node type";
    }
}

#endif