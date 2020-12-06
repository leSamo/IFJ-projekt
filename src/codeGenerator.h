/* ======== codeGenerator.h ========
 * Project: IFJ 2020/21 project
 * Team: 067, variant I
 * Author: Samuel Olekšák (xoleks00)
 * Date: November 2020
 * ================================= */

#include "AST.c"
#include "symtable.c"

void generateCode(ASTNode *astRoot, ASTNode *mainNode, ST_Node *symtable);

void generateStructure(ASTNode *node, ST_Node *symtable, IntBuffer scope);

void generateReturn(ASTNode *node, ST_Node *symtable, IntBuffer scope);

void generateBlock(ASTNode *node, ST_Node *symtable, IntBuffer scope);

void generateFor(ASTNode *node, ST_Node *symtable, IntBuffer scope);

void generateIfElse(ASTNode *node, ST_Node *symtable, IntBuffer scope);

void generateDefinition(ASTNode *defineNode, ST_Node *symtable, IntBuffer scope);

void generateAssignment(ASTNode *assignNode, ST_Node *symtable, IntBuffer scope);

void generateFuncCall(ASTNode *funcCallNode, ST_Node *symtable, IntBuffer scope);

void generateExpression(ASTNode *expNode, ST_Node *symtable, IntBuffer scope, char *resultId, bool tempFrame);

void generateExpressionRecursively(ASTNode *expNode, ST_Node *symtable, IntBuffer scope, char *resultId);

void printOperands(ASTNode *parentNode, ST_Node *symtable, IntBuffer scope, char *leftChild, char *rightChild);

void printVar(char *id, ST_Node *symtable, IntBuffer scopes, char *frame);

void printTerm(ASTNode *node, ST_Node *symtable, IntBuffer scopes, char *frame);

char* escapeString(char* string);

//print to standard output formated string in IFJcode20 "string@<content>"
void printFormatedString(char *string);
