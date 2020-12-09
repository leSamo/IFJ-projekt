/* ======== codeGenerator.h ========
 * Project: IFJ 2020/21 project
 * Team: 067, variant I
 * Author: Samuel Olekšák (xoleks00)
 * Date: November, December 2020
 * ================================= */

#include "AST.c"
#include "symtable.c"

/* Generate code of the whole program using AST tree and symtable */
void generateCode(ASTNode *astRoot, ASTNode *mainNode, ST_Node *symtable);

/* Depending on supplied AST node generates its realization in IFJcode20 */
void generateStructure(ASTNode *node, ST_Node *symtable, IntBuffer scope);

/* Generates return statement and ends current function */
void generateReturn(ASTNode *node, ST_Node *symtable, IntBuffer scope);

/* Generates all statements inside of supplied block */
void generateBlock(ASTNode *node, ST_Node *symtable, IntBuffer scope);

/* Generates for loop header and body */
void generateFor(ASTNode *node, ST_Node *symtable, IntBuffer scope);

/* Generates if-else constructions with its correspoding blocks */
void generateIfElse(ASTNode *node, ST_Node *symtable, IntBuffer scope);

/* Generates definition */
void generateDefinition(ASTNode *defineNode, ST_Node *symtable, IntBuffer scope);

/* Generates assignment of expression(s) or function call, also generates some built-in function */
void generateAssignment(ASTNode *assignNode, ST_Node *symtable, IntBuffer scope);

/* Generates function call of a function without return types */
void generateFuncCall(ASTNode *funcCallNode, ST_Node *symtable, IntBuffer scope);

/* Generates expression from supplied AST node of type expression */
void generateExpression(ASTNode *expNode, ST_Node *symtable, IntBuffer scope, char *resultId, bool tempFrame);

/* Helper function for generateExpression */
void generateExpressionRecursively(ASTNode *expNode, ST_Node *symtable, IntBuffer scope, char *resultId);

/* Used during expression generating, print both operants like term or like a temp variable */
void printOperands(ASTNode *parentNode, ST_Node *symtable, IntBuffer scope, char *leftChild, char *rightChild);

/* Prints variable in form IFJcode20 can understand it, with frame; and prefixes it with scope */
void printVar(char *id, ST_Node *symtable, IntBuffer scopes, char *frame);

/* Prints a term in form IFJcode20 can understand it, variables prefix with frame 
and literals prefix with int@, float@ or string@ */
void printTerm(ASTNode *node, ST_Node *symtable, IntBuffer scopes, char *frame);

/* print to standard output formated string in IFJcode20 "string@<content>" */
void printFormattedString(char *string);
