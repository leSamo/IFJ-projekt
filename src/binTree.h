/* ========= binTree.h ==========
 * Project: IFJ 2020/21 project
 * Team: 067, variant I
 * Author: Samuel Olekšák (xoleks00), Michal Findra (xfindr00)
 * Date: November 2020
 * Note: Part of this file was copied from xfindr00's IAL 2 project
 * ================================= */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "binTree.c"

void BSTInit(tBSTNodePtr *);

tBSTNodePtr BSTSearch(tBSTNodePtr RootPtr, char *searchedId);

void BSTInsert(tBSTNodePtr *RootPtr, char *id, symType type);

void BSTDispose(tBSTNodePtr *);

void BSTPrettyPrint(tBSTNodePtr nodePtr, int level);
