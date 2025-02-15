/* ====== semanticAnalysis.c =======
 * Project: IFJ 2020/21 project
 * Team: 067, variant I
 * Author: Samuel Olekšák (xoleks00), Michal Findra (xfindr00)
 * Date: November, December 2020
 * Description: Two-pass semantic analysis
 * ================================= */

#include <stdlib.h>
#include <stdbool.h>

#include "AST.c"
#include "symtable.c"
#include "semanticAnalysis.h"
#include "constants.h"

ASTNode *mainFuncDef = NULL;

void AST_FirstPass(ASTNode *astNode, ST_Node **symTableRoot) {
    // recursively traverse tree
    AST_FirstPassTraversal(astNode, symTableRoot);

    // check if main func is present and has no params and no return type
    if (mainFuncDef != NULL) {
        ASTNode *funcParams = AST_GetChildOfType(mainFuncDef, NODE_Func_Def_Param_List);
        ASTNode *funcReturnTypes = AST_GetChildOfType(mainFuncDef, NODE_Func_Def_Return);

        if (funcParams->childrenCount > 0 || funcReturnTypes->childrenCount > 0) {
            throwError(ARGUMENT_ERROR, "Invalid main function\n", false);
        }
    }
    else {
        throwError(DEFINITION_ERROR, "Missing main function\n", false);
    }
}

void AST_FirstPassTraversal(ASTNode *astNode, ST_Node **symTableRoot) {
    if (astNode != NULL) {
        // when you find function node, add it into symtable
        if (astNode->type == NODE_Func_Def) {
            ST_AddFunction(astNode, symTableRoot);
        }

        // recursively call this on node's children
        for (int i = 0; i < astNode->childrenCount; i++) {
            AST_FirstPassTraversal(astNode->children[i], symTableRoot);
        }
    }
}

void ST_AddFunction(ASTNode *astNode, ST_Node **symTableRoot) {
    // detect if function is defined inside another function
    if (astNode->parent != ASTRoot) {
        throwError(OTHER_SEMANTIC_ERROR, "Nested function definition error\n", false);
    }

    // when you find main function node, save it into special variable
    if (strcmp(astNode->content.str, "main") == 0) {
        mainFuncDef = astNode;
    }

    // insert symbol into symtable
    ST_Insert(symTableRoot, astNode->content.str, SYM_Func, astNode, GLOBAL_SCOPE);
}

void AST_SecondPass(ASTNode *astNode, ST_Node **symTableRoot) {
    // buffer to save current scope during traversal
    IntBuffer *scopes = IntBufferCreate();

    AST_SecondPassTraversal(astNode, symTableRoot, *scopes);

    IntBufferDispose(&scopes);
}

void AST_SecondPassTraversal(ASTNode *astNode, ST_Node **symTableRoot, IntBuffer scopes) {
    if (astNode != NULL) {
        if (astNode->type == NODE_Block) {
            // we are entering block add new scope id to the scope buffer
            IntBufferPush(&scopes, astNode->id);
        }
        else if (astNode->type == NODE_For) {
            // for header is a new scope, add it to the scope buffer
            IntBufferPush(&scopes, astNode->id);

            ASTNode *forExpNode = AST_GetChildOfType(astNode, NODE_Exp);
            ASTNode *forDefNode = AST_GetChildOfType(astNode, NODE_Define);

            // check if expression evaluates to bool (has relational operator at its root)
            ASTNodeType expRootType = forExpNode->children[0]->type;

            if (expRootType != NODE_Equal && expRootType != NODE_Not_Equal && expRootType != NODE_Greater_Than
             && expRootType != NODE_Greater_Equal_Than && expRootType != NODE_Less_Than && expRootType != NODE_Less_Equal_Than) {
                throwError(INCOMPATIBLE_TYPE_ERROR, "For expression doesn't evaluate to bool error\n", false);
            }

            if (forDefNode != NULL) {
                // check if define part is valid
                ST_VariableDefinition(forDefNode, symTableRoot, scopes);
            }

            // check if expression part is valid
            ST_DeriveExpressionType(forExpNode, symTableRoot, scopes);
        }
        else if (astNode->type == NODE_If_Else) {
            ASTNode *ifExpNode = AST_GetChildOfType(astNode, NODE_Exp);

            // check if expression evaluates to bool (has relational operator at its root)
            ASTNodeType expRootType = ifExpNode->children[0]->type;

            if (expRootType != NODE_Equal && expRootType != NODE_Not_Equal && expRootType != NODE_Greater_Than
             && expRootType != NODE_Greater_Equal_Than && expRootType != NODE_Less_Than && expRootType != NODE_Less_Equal_Than) {
                throwError(INCOMPATIBLE_TYPE_ERROR, "If expression doesn't evaluate to bool error\n", false);
            }

            // check if expression part is valid
            ST_DeriveExpressionType(ifExpNode, symTableRoot, scopes);
        }
        else if (astNode->type == NODE_Define && astNode->parent->type != NODE_For) {
            ST_VariableDefinition(astNode, symTableRoot, scopes);
        }
        else if (astNode->type == NODE_Assign) {
            ST_VariableAssignment(astNode, symTableRoot, scopes);
        }
        else if (astNode->type == NODE_Func_Call && astNode->parent->type != NODE_Assign) {
            // func call without return types
            ASTNode *funcNode = ST_GetFuncNode(astNode->content.str, symTableRoot, scopes);
            ST_CheckFuncCallArgs(astNode, funcNode->content.str, symTableRoot, scopes);
        }
        else if (astNode->type == NODE_Return) {
            ST_Return(astNode, symTableRoot, scopes);
        }
        else if (astNode->type == NODE_Func_Def) {
            ST_FuncDefinition(astNode, symTableRoot, scopes);
        }

        // recursively call this function on each nodes children
        for (int i = 0; i < astNode->childrenCount; i++) {
            AST_SecondPassTraversal(astNode->children[i], symTableRoot, scopes);
        }
    }
}

void ST_FuncDefinition(ASTNode *astNode, ST_Node **symTableRoot, IntBuffer scopes) {
    ASTNode *paramListNode = AST_GetChildOfType(astNode, NODE_Func_Def_Param_List);
    ASTNode *blockNode = AST_GetChildOfType(astNode, NODE_Block);

    // outer function block has only one scope (block id)
    IntBuffer blockScope;

    blockScope.count = 1;
    blockScope.capacity = 1;
    blockScope.content = &blockNode->id;

    // insert each param into symtable
    for (int i = 0; i < paramListNode->childrenCount; i++) {
        ASTNode *paramNode = paramListNode->children[i];
        ASTNode *idNode = AST_GetChildOfType(paramNode, NODE_Identifier);
        ASTNode *typeNode = paramNode->children[1];

        ST_Insert(symTableRoot, idNode->content.str, typeNode->contentType, idNode, blockScope);
    }

    // check if all return statement within function are compatible with definition
    ASTNode *returnListNode = AST_GetChildOfType(astNode, NODE_Func_Def_Return);

    if (returnListNode->childrenCount > 0) {
        if (AST_GetDescendantOfType(blockNode, NODE_Return) == NULL) {
            throwError(ARGUMENT_ERROR, "Missing return statement in function definition\n", false);
        }
    }
}

void ST_VariableDefinition(ASTNode *astNode, ST_Node **symTableRoot, IntBuffer scopes) {
    ASTNode *variableNode = astNode->children[0];
    ASTNode *expressionNode = astNode->children[1];

    typeTag type = ST_DeriveExpressionType(expressionNode, symTableRoot, scopes);

    ST_Insert(symTableRoot, variableNode->content.str, type, variableNode, scopes);
}

void ST_VariableAssignment(ASTNode *astNode, ST_Node **symTableRoot, IntBuffer scopes) {
    ASTNode *variableNode = astNode->children[0];
    ASTNode *rightSideNode = astNode->children[1];

    // has more left side identifiers, must be a function call
    if (variableNode->type == NODE_Multi_L_Value) {
        ASTNode *funcNode = ST_GetFuncNode(rightSideNode->content.str, symTableRoot, scopes);
        ASTNode *returnTypesNode = AST_GetChildOfType(funcNode, NODE_Func_Def_Return);

        if (returnTypesNode->childrenCount != variableNode->childrenCount) {
            throwError(ARGUMENT_ERROR, "Invalid return value count in func call assignment\n", false);
        }
        else {
            // for each multi L-value type find matching return type for fn
            for (int i = 0; i < returnTypesNode->childrenCount; i++) {
                typeTag variableType = ST_GetVariableType(variableNode->children[i]->content.str, symTableRoot, scopes);
                if (variableType != TAG_Any && returnTypesNode->children[i]->contentType != variableType) {
                    throwError(ARGUMENT_ERROR, "Incompatible type in func call assignment error\n", false);
                }
            }

            ST_CheckFuncCallArgs(rightSideNode, rightSideNode->content.str, symTableRoot, scopes);
        }
    }
    else {
        typeTag variableType = ST_GetVariableType(variableNode->content.str, symTableRoot, scopes);
        typeTag rightSideType;

        // check if assignment left and right side match in types 
        if (rightSideNode->type == NODE_Exp) {
            rightSideType = ST_DeriveExpressionType(rightSideNode, symTableRoot, scopes);
        }
        else if (rightSideNode->type == NODE_Func_Call) {
            ASTNode *funcNode = ST_GetFuncNode(rightSideNode->content.str, symTableRoot, scopes);
            ASTNode *returnTypesNode = AST_GetChildOfType(funcNode, NODE_Func_Def_Return);

            if (returnTypesNode->childrenCount != 1) {
                throwError(ARGUMENT_ERROR, "Invalid return value count in func call assignment\n", false);
            }
            else {
                rightSideType = returnTypesNode->children[0]->contentType;
            }

            ST_CheckFuncCallArgs(rightSideNode, rightSideNode->content.str, symTableRoot, scopes);
        }

        if (variableType != TAG_Any && variableType != rightSideType) {
            throwError(INCOMPATIBLE_TYPE_ERROR, "Incompatible type in assignment error\n", false);
        }
    }
}

void ST_Return(ASTNode *returnNode, ST_Node **symTableRoot, IntBuffer scopes) {
    ASTNode *funcDefinition = AST_GetParentOfType(returnNode, NODE_Func_Def);
    ASTNode *funcDefReturnTypes = AST_GetChildOfType(funcDefinition, NODE_Func_Def_Return);

    if (funcDefReturnTypes->childrenCount != returnNode->childrenCount) {
        throwError(ARGUMENT_ERROR, "Return statement incompatible with func definition error\n", false);
    }
    else {
        // for each func def return type find matching return statement type
        for (int i = 0; i < funcDefReturnTypes->childrenCount; i++) {
            if (!ST_CheckExpressionType(returnNode->children[i], symTableRoot, funcDefReturnTypes->children[i]->contentType, scopes)) {
                throwError(ARGUMENT_ERROR, "Return statement incompatible with func definition error\n", false);
            }
        }
    }
}

void ST_CheckFuncCallArgs(ASTNode *funcCallNode, char *funcName, ST_Node **symTableRoot, IntBuffer scopes) {
    ASTNode *funcDefinition = ST_GetFuncNode(funcName, symTableRoot, scopes);
    ASTNode *funcDefParamList = AST_GetChildOfType(funcDefinition, NODE_Func_Def_Param_List);

    if (funcDefParamList != NULL) { // fixed param count (unlike built-in print function)
        if (funcDefParamList->childrenCount != funcCallNode->childrenCount) {
            throwError(ARGUMENT_ERROR, "Incorrect argument count in function call\n", false);
        }
        else { // check type compatibility one-by-one
            for (int i = 0; i < funcDefParamList->childrenCount; i++) {
                if (funcDefParamList->children[i]->childrenCount > 1) {
                    if (!ST_CheckTermType(funcCallNode->children[i], symTableRoot, funcDefParamList->children[i]->children[1]->contentType, scopes)) {
                        throwError(ARGUMENT_ERROR, "Incorrect argument types in function call\n", false);
                    }
                }
            }
        }
    }
    // else variable param count, just check if passing valid terms
    else {
        for (int i = 0; i < funcCallNode->childrenCount; i++) {
            ST_CheckTermType(funcCallNode->children[i], symTableRoot, funcCallNode->children[i]->contentType, scopes);
        }
    }
}


typeTag ST_DeriveExpressionType(ASTNode *expNode, ST_Node **symTableRoot, IntBuffer scopes) {
    // go left until you hit a leaf node
    ASTNode *seedNode = expNode;

    do {
        seedNode = seedNode->children[0];
    } while (seedNode->children[0] != NULL);

    // we assume the expression is of that type of that leaf node
    typeTag expType = seedNode->valueType;

    // we don't know type - it's an identifier - look into symbol table
    if (expType == TAG_Unknown) {
        expType = ST_GetVariableType(seedNode->content.str, symTableRoot, scopes);
    }

    // verify if the whole expression is of that type
    if (ST_CheckExpressionType(expNode, symTableRoot, expType, scopes)) {
        return expType;
    }
    else {
        throwError(INCOMPATIBLE_TYPE_ERROR, "Incompatible types inside expression error\n", false);
    }
}

bool ST_CheckExpressionType(ASTNode *partialExpNode, ST_Node **symTableRoot, typeTag type, IntBuffer scopes) {   
    if (partialExpNode->childrenCount == 0) { // this node is leaf
        typeTag expType = partialExpNode->valueType;

        // we don't know type - it's an identifier - look into symbol table
        if (expType == TAG_Unknown) {
            if (strcmp(partialExpNode->content.str, "_") == 0) {
                throwError(DEFINITION_ERROR, "Reading of _ variable error\n", false);
            }

            expType = ST_GetVariableType(partialExpNode->content.str, symTableRoot, scopes);
        }

        // Check if string literals are using only NODE_Add
        if (expType == TAG_String) {
            if (partialExpNode->parent->type == NODE_Sub || partialExpNode->parent->type == NODE_Mul || partialExpNode->parent->type == NODE_Div) {
                throwError(INCOMPATIBLE_TYPE_ERROR, "Invalid operation on string error\n", false);
            }
        }

        return expType == type;
    }
    // recursively call this funcion for nodes children
    else if (partialExpNode->childrenCount == 1) {
        return ST_CheckExpressionType(partialExpNode->children[0], symTableRoot, type, scopes);
    }
    else {
        return ST_CheckExpressionType(partialExpNode->children[0], symTableRoot, type, scopes) &&
               ST_CheckExpressionType(partialExpNode->children[1], symTableRoot, type, scopes);
    }
}

bool ST_CheckTermType(ASTNode *termNode, ST_Node **symTableRoot, typeTag expectedType, IntBuffer scopes) {
    typeTag termType;

    if (termNode->type == NODE_Identifier) {
        if (strcmp(termNode->content.str, "_") == 0) {
            throwError(DEFINITION_ERROR, "Write-only variable _ in function call error\n" , false);
        }

        // fetch identifier type from symtable
        termType = ST_GetVariableType(termNode->content.str, symTableRoot, scopes);
    }
    else if (termNode->type == NODE_Literal_Int)  {
        termType = TAG_Int;
    }
    else if (termNode->type == NODE_Literal_Float)  {
        termType = TAG_Float;
    }
    else if (termNode->type == NODE_Literal_String)  {
        termType = TAG_String;
    }

    return termType == expectedType;
}

typeTag ST_GetVariableType(char *id, ST_Node **symTableRoot, IntBuffer scopes) {
    // fetch variable identifier type from symtable
    ST_Node *symbol = ST_Search(*symTableRoot, *symTableRoot, id, scopes);

    if (symbol != NULL) {
        return symbol->type;
    }
    else {
        throwError(DEFINITION_ERROR, "Undefined variable error\n", false);
    }
}

ASTNode *ST_GetFuncNode(char *id, ST_Node **symTableRoot, IntBuffer scopes) {
    // fetch function identifier type from symtable
    ST_Node *symbol = ST_Search(*symTableRoot, *symTableRoot, id, scopes);

    if (symbol != NULL) {
        if (symbol->type != SYM_Func) {
            throwError(DEFINITION_ERROR, "Calling function identifier shadow by variable error\n", false);
        }

        return symbol->node;
    }
    else {
        throwError(DEFINITION_ERROR, "Undefined function identifier\n", false);
    }
}
