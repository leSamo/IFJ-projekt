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

ASTNode* AST_CreateIntNode(ASTNode *parent, ASTNodeType type, int i) {
    typeUnion tu = { .i = i };

    return AST_CreateNodeGeneral(parent, type, TAG_Int, tu);
}

ASTNode* AST_CreateFloatNode(ASTNode *parent, ASTNodeType type, double f) {
    typeUnion tu = { .f = f };

    return AST_CreateNodeGeneral(parent, type, TAG_Float, tu);
}

ASTNode* AST_CreateStringNode(ASTNode *parent, ASTNodeType type, char* str) {
    typeUnion tu = { .str = str };

    return AST_CreateNodeGeneral(parent, type, TAG_String, tu);
}

ASTNode* AST_CreateNode(ASTNode *parent, ASTNodeType type) {
    typeUnion tu;

    return AST_CreateNodeGeneral(parent, type, TAG_None, tu);
}

ASTNode* AST_CreateTaggedNode(ASTNode *parent, ASTNodeType type, typeTag tag) {
    typeUnion tu;

    return AST_CreateNodeGeneral(parent, type, tag, tu);
}

ASTNode* AST_CreateNodeGeneral(ASTNode *parent, ASTNodeType type, typeTag contentType, typeUnion content) {
    ASTNode *node = malloc(sizeof(ASTNode));

    if (parent != NULL) {
        parent->children[parent->childrenCount++] = node;
    }

    node->id = AST_consecutiveId++;
    node->parent = parent;
    node->childrenCount = 0;
    
    for (int i = 0; i < AST_NODE_CHILDREN; i++) {
        node->children[i] = NULL;
    }

    node->type = type;
    node->isOperatorResult = false;
    node->contentType = contentType;
    node->content = content;

    return node;
}

void AST_AttachNode(ASTNode *parent, ASTNode* child) {
    parent->children[parent->childrenCount++] = child;
    child->parent = parent;
}

ASTNode* AST_GetChildOfType(ASTNode *parent, ASTNodeType type) {
    for (int i = 0; i < parent->childrenCount; i++) {
        if (parent->children[i]->type == type) {
            return parent->children[i];
        }
    }

    return NULL; // not found
}

ASTNode* AST_GetParentOfType(ASTNode *child, ASTNodeType type) {
    while (child->parent != NULL) {
        if (child->parent->type == type) {
            return child->parent;
        }

        child = child->parent;
    }

    return NULL; // not found
}

void AST_PrettyPrint(ASTNode *nodePtr, int level) {
    for (int i = 0; i < level; i++) {
        printf("  ");
    }

    printf("%s", AST_GetNodeName(nodePtr->type));

    switch (nodePtr->contentType) {
        case TAG_Int:
            printf(" - %d\n", nodePtr->content.i);
            break;
        case TAG_Float:
            printf(" - %f\n", nodePtr->content.f);
            break;
        case TAG_String:
            printf(" - %s\n", nodePtr->content.str);
            break;
        default:
            printf("\n");
    }

    for (int i = 0; i < nodePtr->childrenCount; i++) {
        AST_PrettyPrint(nodePtr->children[i], level + 1);
    }
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

char *AST_GetTagName(typeTag type) {
    switch (type) {
    case TAG_Float:
        return "Float";
    case TAG_Int:
        return "Int";
    case TAG_String:
        return "String";
    default:
        return "none yet";
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
        case NODE_Func_Def_Param_List:
            return "Func def param list";
        case NODE_Func_Def_Param:
            return "Func def param";
        case NODE_Func_Def_Params_Variable:
            return "Func def variable params";
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
        case NODE_Add:
            return "Addition";
        case NODE_Sub:
            return "Subtraction";
        case NODE_Mul:
            return "Multiplication";
        case NODE_Div:
            return "Division";
        case NODE_Less_Then:
            return "Less then";
        case NODE_More_Then:
            return "More then";
        case NODE_Less_Equal_Then:
            return "Less or equal then";
        case NODE_More_Equal_Then:
            return "More or equal then";
        case NODE_Equal:
            return "Equal";
        case NODE_Not_Equal:
            return "Not equal";
        case NODE_Multi_L_Value:
            return "Multi L-Value";
        default:
            return "Unknown AST node type";
        }
}

#endif
