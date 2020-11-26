/* ====== semanticAnalysis.c =======
 * Project: IFJ 2020/21 project
 * Team: 067, variant I
 * Author: Samuel Olekšák (xoleks00), Michal Findra (xfindr00)
 * Date: November 2020
 * ================================= */

#include "AST.c"
#include "binTree.c"

/* In the first pass fill symbol table with function identifiers */
void AST_FirstPass(ASTNode *astNode, tBSTNodePtr *symTableRoot);

/* Recursively traverse tree and look for function definition */
void AST_FirstPassTraversal(ASTNode *astNode, tBSTNodePtr *symTableRoot);

/* Add function to symbol table */
void ST_AddFunction(ASTNode *astNode, tBSTNodePtr *symTableRoot);
