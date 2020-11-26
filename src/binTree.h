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

void ST_Init(ST_Node **);

ST_Node* ST_Search(ST_Node *RootPtr, char *searchedId);

void ST_Insert(ST_Node **RootPtr, char *id, symType type);

void ST_Dispose(ST_Node **);

void ST_PrettyPrint(ST_Node *nodePtr, int level);
