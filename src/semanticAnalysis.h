/* ====== semanticAnalysis.c =======
 * Project: IFJ 2020/21 project
 * Team: 067, variant I
 * Author: Samuel Olekšák (xoleks00), Michal Findra (xfindr00)
 * Date: November 2020
 * ================================= */

#include "AST.c"
#include "symtable.c"

/* In the first pass fill symbol table with function identifiers */
void AST_FirstPass(ASTNode *astNode, ST_Node **symTableRoot);

/* Recursively traverse tree and look for function definitions */
void AST_FirstPassTraversal(ASTNode *astNode, ST_Node **symTableRoot);

/* Add function to symbol table */
void ST_AddFunction(ASTNode *astNode, ST_Node **symTableRoot);

/* In the second pass fill sybol table with variable identifiers and derive their type */
void AST_SecondPass(ASTNode *astNode, ST_Node **symTableRoot);

/* Recursively traverse tree and look for variable definitions */
void AST_SecondPassTraversal(ASTNode *astNode, ST_Node **symTableRoot, IntBuffer scopes);

/* Add variable to symbol table with its type and scope */
void ST_VariableDefinition(ASTNode *astNode, ST_Node **symTableRoot, IntBuffer scopes);

/* Check if assignment is valid */
void ST_VariableAssignment(ASTNode *astNode, ST_Node **symTableRoot, IntBuffer scopes);

/* Check if return statement is returning correct types according to function definition */
void ST_Return(ASTNode *astNode, ST_Node **symTableRoot, IntBuffer scopes);

/* Check if function call arguments are correct according to function definition */
void ST_CheckFuncCallArgs(ASTNode *funcCallNode, char *funcName, ST_Node **symTableRoot, IntBuffer scopes);

/* Work out what is the type of expression pointed to by first argument */
typeTag ST_DeriveExpressionType(ASTNode *expNode, ST_Node **symTableRoot, IntBuffer scopes);

/* Recursively check if all terms inside expression are of the supplied type */
bool ST_CheckExpressionType(ASTNode *partialExpNode, ST_Node **symTableRoot, typeTag type, IntBuffer scopes);

/* Check if term is of the supplied type */
bool ST_CheckTermType(ASTNode *termNode, ST_Node **symTableRoot, typeTag type, IntBuffer scopes);

/* Fetch variable by id from symbol table and return its type */
typeTag ST_GetVariableType(char *id, ST_Node **symTableRoot, IntBuffer scopes);

/* Fetch functino by id from symbol table and return its node */
ASTNode *ST_GetFuncNode(char *id, ST_Node **symTableRoot);
