/* ====== semanticAnalysis.c =======
 * Project: IFJ 2020/21 project
 * Team: 067, variant I
 * Author: Samuel Olekšák (xoleks00), Michal Findra (xfindr00)
 * Date: November 2020
 * ================================= */

#include <stdlib.h>
#include <stdbool.h>

#include "AST.c"
#include "binTree.c"
#include "semanticAnalysis.h"

ASTNode *mainFuncDef = NULL;

void AST_FirstPass(ASTNode *astNode, ST_Node **symTableRoot) {
    AST_FirstPassTraversal(astNode, symTableRoot);

    // check if main func is present and has no params and no return type
    if (mainFuncDef != NULL) {
        ASTNode *funcParams = AST_GetChildOfType(mainFuncDef, NODE_Func_Def_Params);
        ASTNode *funcReturnTypes = AST_GetChildOfType(mainFuncDef, NODE_Func_Def_Return);

        if (funcParams->childrenCount > 0 || funcReturnTypes->childrenCount > 0) {
            printError(ARGUMENT_ERROR, "Invalid main function\n");
        }
    }
    else {
        printError(DEFINITION_ERROR, "Missing main function\n");
    }
}

void AST_FirstPassTraversal(ASTNode *astNode, ST_Node **symTableRoot) {
    if (astNode != NULL) {
        if (astNode->type == NODE_Func_Def) {
            ST_AddFunction(astNode, symTableRoot);
        }

        for (int i = 0; i < astNode->childrenCount; i++) {
            AST_FirstPassTraversal(astNode->children[i], symTableRoot);
        }
    }
}

void ST_AddFunction(ASTNode *astNode, ST_Node **symTableRoot) {
    if (ST_Search(*symTableRoot, astNode->content.str) != NULL) {
        printError(DEFINITION_ERROR, "Function identifier redefinition error\n");
    }

    if (astNode->parent != ASTRoot) { // syntactic analyzer should cover this
        printError(OTHER_SEMANTIC_ERROR, "Nested function definition error\n");
    }

    if (strcmp(astNode->content.str, "main") == 0) {
        mainFuncDef = astNode;
    }

    // check if parent is prog, else error
    ST_Insert(symTableRoot, astNode->content.str, SYM_FUNC);
}

void AST_SecondPass(ASTNode *astNode, ST_Node **symTableRoot) {
    AST_SecondPassTraversal(astNode, symTableRoot);
}

void AST_SecondPassTraversal(ASTNode *astNode, ST_Node **symTableRoot) {
    if (astNode != NULL) {
        if (astNode->type == NODE_Define) {
            ST_AddVariable(astNode, symTableRoot);
        }

        for (int i = 0; i < astNode->childrenCount; i++) {
            AST_SecondPassTraversal(astNode->children[i], symTableRoot);
        }
    }
}

void ST_AddVariable(ASTNode *astNode, ST_Node **symTableRoot) {
    ASTNode *variableNode = astNode->children[0];
    ASTNode *expressionNode = astNode->children[1];

    typeTag type = ST_DeriveExpressionType(expressionNode, symTableRoot);

    ST_Insert(symTableRoot, variableNode->content.str, type);
}

typeTag ST_DeriveExpressionType(ASTNode *expNode, ST_Node **symTableRoot) {
    // go left until you hit a leaf node
    ASTNode *seedNode = expNode;

    do {
        seedNode = seedNode->children[0];
    } while (seedNode->children[0] != NULL);

    typeTag expType = seedNode->valueType;

    // we don't know type - it's an identifier - look into symbol table
    if (expType == TAG_Unknown) {
        ST_Node *symbol = ST_Search(*symTableRoot, seedNode->content.str);

        if (symbol != NULL) {
            expType = symbol->type;
        }
        else {
            printError(DEFINITION_ERROR, "Undefined variable\n");
        }
    }
    
    if (ST_CheckExpressionType(expNode, symTableRoot, expType)) {
        printf("Compatible, adding to symbol table\n");
        return expType;
    }
    else {
        printError(INCOMPATIBLE_TYPE_ERROR, "Incompatible types inside expression error\n");
    }
}

bool ST_CheckExpressionType(ASTNode *partialExpNode, ST_Node **symTableRoot, typeTag type) {
    // TODO: Check if string literals are using only NODE_Add
    if (partialExpNode->childrenCount == 0) { // this node is leaf
        typeTag expType = partialExpNode->valueType;

        // we don't know type - it's an identifier - look into symbol table
        if (expType == TAG_Unknown) {
            ST_Node *symbol = ST_Search(*symTableRoot, partialExpNode->content.str);

            if (symbol != NULL) {
                expType = symbol->type;
            }
            else {
                printError(DEFINITION_ERROR, "Undefined variable\n");
            }
        }
        return expType == type;
    }
    else if (partialExpNode->childrenCount == 1) {
        return ST_CheckExpressionType(partialExpNode->children[0], symTableRoot, type);
    }
    else {
        return ST_CheckExpressionType(partialExpNode->children[0], symTableRoot, type) &&
               ST_CheckExpressionType(partialExpNode->children[1], symTableRoot, type);
    }
}