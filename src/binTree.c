/* ========= binTree.c ==========
 * Project: IFJ 2020/21 project
 * Team: 067, variant I
 * Author: Samuel Olekšák (xoleks00), Michal Findra (xfindr00)
 * Date: November 2020
 * ================================= */

#include "binTree.h"

void BSTInit(tBSTNodePtr *RootPtr)
{
	*RootPtr = NULL;
}

int BSTSearch(tBSTNodePtr RootPtr, char K, int *Content)
{
	if (RootPtr == NULL) // chceck if tree was initialized
	{
		return FALSE;
	}

	if (K == RootPtr->Key) // key was found
	{
		*Content = RootPtr->BSTNodeCont;
		return TRUE;
	}
	if (K > RootPtr->Key) // key not foud, move to right/left child
	{
		return BSTSearch(RootPtr->RPtr, K, Content);
	}
	else
	{
		return BSTSearch(RootPtr->LPtr, K, Content);
	}
}

void BSTInsert(tBSTNodePtr *RootPtr, char K, int Content)
{
	if (*RootPtr == NULL) // empty tree
	{
		tBSTNodePtr new_leaf = malloc(sizeof(struct tBSTNode));
		if (new_leaf == NULL)
		{
			return;
		}
		new_leaf->Key = K;
		new_leaf->BSTNodeCont = Content;
		new_leaf->LPtr = NULL;
		new_leaf->RPtr = NULL;
		*RootPtr = new_leaf;
		return;
	}
	else if ((*RootPtr)->Key == K) // tree alreade has node with key K
	{
		(*RootPtr)->BSTNodeCont = Content;
		return;
	}
	else if ((*RootPtr)->Key < K) // key has smaller/greater value than K, call recursively
	{
		BSTInsert(&(*RootPtr)->RPtr, K, Content);
	}
	else if ((*RootPtr)->Key > K)
	{
		BSTInsert(&(*RootPtr)->LPtr, K, Content);
	}
	return;
}

void ReplaceByRightmost(tBSTNodePtr PtrReplaced, tBSTNodePtr *RootPtr)
{
	if (*RootPtr == NULL) // chceck if tree is empty
	{
		return;
	}
	else if ((*RootPtr)->RPtr != NULL) // chcek if node has right child
	{
		ReplaceByRightmost(PtrReplaced, &(*RootPtr)->RPtr);
	}
	else // replace node and free memory
	{
		PtrReplaced->Key = (*RootPtr)->Key;
		PtrReplaced->BSTNodeCont = (*RootPtr)->BSTNodeCont;
		tBSTNodePtr previous = (*RootPtr);
		(*RootPtr) = (*RootPtr)->LPtr;
		free(previous);
		return;
	}
}

void BSTDelete(tBSTNodePtr *RootPtr, char K)
{
	if (*RootPtr == NULL) // chceck if tree is empty
	{
		return;
	}

	tBSTNodePtr tmp;

	if ((*RootPtr)->Key != K) // recursively find node with key = K
	{
		if ((*RootPtr)->Key < K)
		{
			BSTDelete(&(*RootPtr)->RPtr, K);
		}
		else if ((*RootPtr)->Key > K)
		{
			BSTDelete(&(*RootPtr)->LPtr, K);
		}
	}
	else if ((*RootPtr)->Key == K) // node with key = K found
	{
		if (((*RootPtr)->LPtr == NULL) && ((*RootPtr)->RPtr == NULL)) // node is leaf
		{
			free((*RootPtr));
			(*RootPtr) = NULL;
			return;
		}
		else if (((*RootPtr)->LPtr != NULL) && ((*RootPtr)->RPtr != NULL)) // node has both children
		{
			ReplaceByRightmost(*RootPtr, &(*RootPtr)->LPtr);
			return;
		}
		else if ((*RootPtr)->LPtr != NULL) // node has only left child
		{
			tmp = (*RootPtr)->LPtr;
			free(*RootPtr);
			*RootPtr = tmp;
			return;
		}
		else // node has only right child
		{
			tmp = (*RootPtr)->RPtr;
			free(*RootPtr);
			*RootPtr = tmp;
			return;
		}
	}
}

void BSTDispose(tBSTNodePtr *RootPtr)
{
	if (*RootPtr != NULL)
	{
		BSTDispose(&(*RootPtr)->RPtr);
		BSTDispose(&(*RootPtr)->LPtr);
		free(*RootPtr);
	}
	*RootPtr = NULL;
}