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

void generateBlock(ASTNode *node, ST_Node *symtable, IntBuffer scope);

void generateFor(ASTNode *node, ST_Node *symtable, IntBuffer scope);

void generateIfElse(ASTNode *node, ST_Node *symtable, IntBuffer scope);

void generateDefinition(ASTNode *defineNode, ST_Node *symtable, IntBuffer scope);

void generateAssignment(ASTNode *assignNode, ST_Node *symtable, IntBuffer scope);

void generateFuncCall(ASTNode *funcCallNode, ST_Node *symtable, IntBuffer scope);

void generateExpression(ASTNode *expNode, ST_Node *symtable, IntBuffer scope, char *resultId);

void generateExpressionRecursively(ASTNode *expNode, ST_Node *symtable, IntBuffer scope, char *resultId);

void printOperands(ASTNode *parentNode, char *leftChild, char *rightChild);

void printTerm(typeTag type, typeUnion content, char *frame);

char* escapeString(char* string);

//print to standard output formated string in IFJcode20 "string@<content>"
void printFormatedString(char *string);
