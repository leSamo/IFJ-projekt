/* ============= SymTable.c =============
 * Project: IFJ 2020/21 project
 * Team: 067, variant I
 * Author: 
 * Date: November 2020
 * Description: Maria skare
 * ================================= */

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "leParser.h"
#include "binTree.h"

tBSTNodePtr SymTableTree;

struct tBSTNode SymTable[20];
int ST_Cnt = 0;
int ST_Block = 0;
bool block_start = false;

// retrun false if variable is defined in block
bool FindByNameInBlock(int block, char *name) {
    for (int i = 0; i < ST_Cnt; i++) {
        char buf[20];
        snprintf(buf, 20, "%d_prefix@%s", block, name);
        if (strcmp(buf, SymTable[i].id) == 0) {
            return false;
        }
    }
    return true;
}

// function fill SymNode and put it to bin tree
void GetIds(ASTNode *nodePtr, int level) {
    if (nodePtr->type == NODE_Block) {
        ST_Block++;
    }

    if (nodePtr->contentType == TAG_String) {
        if (nodePtr->type == NODE_Identifier) {
            if (FindByNameInBlock(ST_Block, nodePtr->content.str)) {
                SymTable[ST_Cnt].Type = TAG_None;
                SymTable[ST_Cnt].content = nodePtr->content;
                char tmp[50];
                snprintf(tmp, 20, "%d_prefix@%s", ST_Block, nodePtr->content.str);
                //printf("%s\n", SymTable[ST_Cnt].id);
                strcpy(SymTable[ST_Cnt].id, tmp);
                ST_Cnt++;
                BSTInsert(&SymTableTree, tmp, SymTable[ST_Cnt].content);
            }
        }
    }

    for (int i = 0; i < nodePtr->childrenCount; i++) {
        GetIds(nodePtr->children[i], level + 1);
    }
}
