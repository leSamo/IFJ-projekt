/* ============= SymTable.c =============
 * Project: IFJ 2020/21 project
 * Team: 067, variant I
 * Author: 
 * Date: November 2020
 * Description: Maria skare
 * ================================= */

#ifndef SYMTABLE
#define SYMTABLE

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "leParser.h"
#include "binTree.h"
/*
struct ST_Node SymTable[20];
char *blocks[20][20];
int BlockCnt = 0;
int ST_Cnt_firstPass = 0;
int ST_Cnt_secondPass = 0;
int ST_Block = 0;
bool block_start = false;
int SemanticError = 0;
//struct ST_Node Decalred[30];

// return false if id in block is already in tree
bool FindByNameInBlock(int block, char *name) {
    for (int i = 0; i < ST_Cnt_firstPass; i++) {
        char buf[20];
        snprintf(buf, 20, "%d_prefix@%s", block, name);
        if (strcmp(buf, SymTable[i].id) == 0) {
            return false;
        }
    }
    return true;
}

// if arithmetic operands are same, retrun type off operands
typeTag CheckChildrenType(ASTNode *nodePtr) {
    if (nodePtr->children[0]->contentType == nodePtr->children[1]->contentType) {
        if (nodePtr->children[0]->type == NODE_Literal_Float)
            return TAG_Float;
        else if (nodePtr->children[0]->type == NODE_Literal_Int)
            return TAG_Int;
        else if (nodePtr->children[0]->type == NODE_Literal_String)
            return TAG_String;
    }
}

bool CheckOperands(ASTNode *nodePtr) {
    if (nodePtr->childrenCount == 2) {
        if (nodePtr->children[0]->type == NODE_Mul || nodePtr->children[0]->type == NODE_Div || nodePtr->children[0]->type == NODE_Add || nodePtr->children[0]->type == NODE_Sub) {
            CheckOperands(nodePtr->children[0]);
        }
        else if (nodePtr->children[1]->type == NODE_Mul || nodePtr->children[1]->type == NODE_Div || nodePtr->children[1]->type == NODE_Add || nodePtr->children[1]->type == NODE_Sub) {
            CheckOperands(nodePtr->children[1]);
        }
        // check if operation with strings is only concatination
        if (nodePtr->children[0]->contentType == nodePtr->children[1]->contentType) {
            if (nodePtr->children[0]->type == NODE_Literal_String) {
                if (nodePtr->type == NODE_Add) {
                    return true;
                }
                else {
                    return false;
                }
            }
            return true;
        }
    }
    return false;
}

void CheckOperations(ASTNode *nodePtr, int level) {
    if (nodePtr->type == NODE_Mul || nodePtr->type == NODE_Div || nodePtr->type == NODE_Add || nodePtr->type == NODE_Sub) {
        //printf("%s", AST_GetNodeName(nodePtr->type));
        if (CheckOperands(nodePtr) == false) {
            printf("Semantic ERROR - %s operands type not same\n", AST_GetNodeName(nodePtr->type));
            SemanticError = 1;
        };
    }
    for (int i = 0; i < nodePtr->childrenCount; i++) {
        CheckOperations(nodePtr->children[i], level + 1);
    }
}

// function return typ of declaerd id
typeTag GetDeclaredType(ASTNode *nodePtr) {
    if (nodePtr->parent->type == NODE_Define) {
        switch (nodePtr->parent->children[1]->children[0]->type) {
        case NODE_Literal_Int:
            return TAG_Int;
        case NODE_Literal_Float:
            return TAG_Float;
        case NODE_Literal_String:
            return TAG_String;
        default:
            return TAG_None;
        }
    }
}

// set prefixes of all parental blocks, and amount of them
void SetPrefixes(ST_Node *node, ASTNode *n) {
    ASTNode tmp = *n;
    int i = 0;
    while (tmp.parent->type != NODE_Prog) {
        if (tmp.type == NODE_Block) {
            //printf("%d", tmp.id);
            node->prefix[i] = tmp.id;
            i++;
        }
        tmp = *tmp.parent;
    }
    node->PrefixCount = i;
}

// find in binary tree with every prefix and check if at least one is declared
bool IsDeclared(ST_Node *node, char *name) {
    for (int i = 0; i < node->PrefixCount; i++) {
        char buf[20];
        snprintf(buf, 20, "%s@%d", name, node->prefix[i]);
        //printf("%s\n", buf);
        if (ST_Search(SymTableTree, buf)) {
            //printf("%s, declared", node->id);
            return true;
        }
    }
    //printf("%s, not declared", node->id);
    return false;
}

int ClosestBlock(ASTNode *n) {
    ASTNode tmp = *n;
    while (tmp.parent->type != NODE_Prog) {
        if (tmp.type == NODE_Block)
            return (tmp.id);

        tmp = *tmp.parent;
    }
}

void PrintPrefix(ST_Node *node) {
    for (int i = 0; i < 5; i++) {
        if (node->prefix[i] != 0)
            printf("%d, ", node->prefix[i]);
    }
    printf("\n");
}

void FillTable_SecondPass(ASTNode *nodePtr, int level) {
    if (nodePtr->contentType == TAG_String) {
        if (nodePtr->type == NODE_Identifier) {
            char tmp[20];
            snprintf(tmp, 20, "%s@%d", nodePtr->content.str, ClosestBlock(nodePtr));
            //printf("%s\n", tmp);

            if (IsDeclared(&SymTable[ST_Cnt_secondPass], nodePtr->content.str) == false) {
                printf("veriable %s, not declared \n", tmp);
            };
        }
        ST_Cnt_secondPass++;
    }

    for (int i = 0; i < nodePtr->childrenCount; i++) {
        FillTable_SecondPass(nodePtr->children[i], level + 1);
    }
}

// function fill SymNode and put it to bin tree
void FillTable_FirstPass(ASTNode *nodePtr) {
    if (nodePtr->type == NODE_Identifier) {
        if (FindByNameInBlock(ClosestBlock(nodePtr), nodePtr->content.str)) {
            SymTable[ST_Cnt_firstPass].Type = GetDeclaredType(nodePtr); // fill type of declard node to ST

            for (int i = 0; i < 5; i++) {
                SymTable[ST_Cnt_firstPass].prefix[i] = 0;
            }

            SetPrefixes(&SymTable[ST_Cnt_firstPass], nodePtr);

            //PrintPrefix(&SymTable[ST_Cnt_firstPass]);
            // printf("%d", SymTable[ST_Cnt_firstPass].prefix[0]);
            char tmp[100];
            snprintf(tmp, 20, "%s@%d", nodePtr->content.str, ClosestBlock(nodePtr));
            strcpy(SymTable[ST_Cnt_firstPass].id, tmp);

            // wont work vvv
            ST_Insert(&SymTableTree, tmp, SymTable[ST_Cnt_firstPass].Type);

            ST_Cnt_firstPass++;
        }
    }

    for (int i = 0; i < nodePtr->childrenCount; i++) {
        FillTable_FirstPass(nodePtr->children[i]);
    }
}
*/
#endif
