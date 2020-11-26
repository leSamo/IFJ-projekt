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

void AST_FirstPass(ASTNode *astNode, tBSTNodePtr *symTableRoot) {
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

void AST_FirstPassTraversal(ASTNode *astNode, tBSTNodePtr *symTableRoot) {
    if (astNode != NULL) {
        if (astNode->type == NODE_Func_Def) {
            ST_AddFunction(astNode, symTableRoot);
        }

        for (int i = 0; i < astNode->childrenCount; i++) {
            AST_FirstPassTraversal(astNode->children[i], symTableRoot);
        }
    }
}

void ST_AddFunction(ASTNode *astNode, tBSTNodePtr *symTableRoot) {
    if (BSTSearch(*symTableRoot, astNode->content.str) != NULL) {
        printError(DEFINITION_ERROR, "Function identifier redefinition error\n");
    }

    if (astNode->parent != ASTRoot) { // syntactic analyzer should cover this
        printError(OTHER_SEMANTIC_ERROR, "Nested function definition error\n");
    }

    if (strcmp(astNode->content.str, "main") == 0) {
        mainFuncDef = astNode;
    }

    // check if parent is prog, else error
    BSTInsert(symTableRoot, astNode->content.str, SYM_FUNC);
}
