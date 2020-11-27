/* ====== semanticAnalysis.c =======
 * Project: IFJ 2020/21 project
 * Team: 067, variant I
 * Author: Samuel Olekšák (xoleks00), Michal Findra (xfindr00)
 * Date: November 2020
 * ================================= */

#include "AST.c"
#include "binTree.c"

/* In the first pass fill symbol table with function identifiers */
void AST_FirstPass(ASTNode *astNode, ST_Node **symTableRoot);

/* Recursively traverse tree and look for function definitions */
void AST_FirstPassTraversal(ASTNode *astNode, ST_Node **symTableRoot);

/* Add function to symbol table */
void ST_AddFunction(ASTNode *astNode, ST_Node **symTableRoot);

/* In the second pass fill sybol table with variable identifiers and derive their type */
void AST_SecondPass(ASTNode *astNode, ST_Node **symTableRoot);

/* Recursively traverse tree and look for variable definitions */
void AST_SecondPassTraversal(ASTNode *astNode, ST_Node **symTableRoot);

/* Add variable to symbol table with its type and scope */
void ST_VariableDefinition(ASTNode *astNode, ST_Node **symTableRoot);

/* Check if assignment is valid */
void ST_VariableAssignment(ASTNode *astNode, ST_Node **symTableRoot);

/* Work out what is the type of expression pointed to by first argument */
typeTag ST_DeriveExpressionType(ASTNode *astNode, ST_Node **symTableRoot);

/* Recursively check if all terms inside expression are of the supplied type */
bool ST_CheckExpressionType(ASTNode *partialExpNode, ST_Node **SymTableTree, typeTag type);

/* Fetch variable by id from symbol table and return its type */
typeTag ST_GetVariableType(char *id, ST_Node **symTableRoot);

/* Fetch functino by id from symbol table and return its node */
ASTNode *ST_GetFuncNode(char *id, ST_Node **symTableRoot);
