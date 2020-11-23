/* ========== leParser.c ===========
 * Project: IFJ 2020/21 project
 * Team: 067, variant I
 * Author: Emma Krompaščíková (xkromp00), Samuel Olekšák (xoleks00), Michal Findra (xfindr00)
 * Date: November 2020
 * Description: Syntactic analysis using recursive descent
 * ================================= */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#include "leScanner.c"
#include "leParser.h"
#include "expressions.c"

typedef enum ASTNodeType {
    NODE_Prog,
    NODE_Func_Def,
    NODE_Func_Def_Params,
    NODE_Func_Def_Return,
    NODE_Stat,
    NODE_If_Else,
    NODE_For,
    NODE_Func_Call,
    NODE_Return,
    NODE_Assign,
    NODE_Define,
    NODE_Exp, // do not go further, thats the job of expressions.c
    NODE_Type
} ASTNodeType;

char* ASTNode_GetName(ASTNodeType type) {
    switch (type) {
        case NODE_Prog:
            return "Prog";
        case NODE_Func_Def:
            return "Func def";
        case NODE_Func_Def_Params:
            return "Func def params";
        case NODE_Func_Def_Return:
            return "Func def return types";
        case NODE_Stat:
            return "Statement";
        case NODE_If_Else:
            return "If-else";
        case NODE_For:
            return "For loop";
        case NODE_Func_Call:
            return "Func call";
        case NODE_Return:
            return "Return";
        case NODE_Assign:
            return "Assignment";
        case NODE_Define:
            return "Definition";
        case NODE_Exp:
            return "Expression";
        case NODE_Type:
            return "Type";
        default:
            return "Unknown AST node type";
    }
}

#define AST_NODE_CHILDREN 10

typedef struct ASTNode {
    int id;
    ASTNodeType type;
    struct ASTNode *parent;
    struct ASTNode *children[AST_NODE_CHILDREN];
    int childrenCount;
} ASTNode;

int AST_consecutiveId = 0;

ASTNode* AST_CreateNode(ASTNode *parent, ASTNodeType type) {
    ASTNode *node = malloc(sizeof(ASTNode));

    if (parent != NULL) {
        parent->children[parent->childrenCount++] = node;
    }

    node->id = AST_consecutiveId;
    node->parent = parent;
    node->type = type;
    node->childrenCount = 0;
    
    for (int i = 0; i < AST_NODE_CHILDREN; i++) {
        node->children[i] = NULL;
    }

    AST_consecutiveId++;

    return node;
}

void AST_Print(ASTNode *nodePtr) {
    printf("========================\n");
    printf("%s [%d]\n", ASTNode_GetName(nodePtr->type), nodePtr->id);

    if (nodePtr->parent != NULL) {
        printf("Parent: [%d]\n", nodePtr->parent->id);
    }

    // print children count
    printf("%d children ", nodePtr->childrenCount);

    for (int i = 0; i < nodePtr->childrenCount; i++) {
        printf("[%d] ", nodePtr->children[i]->id);
    }
    printf("\n");

    // recursivelly check children
    for (int i = 0; i < nodePtr->childrenCount; i++) {
        AST_Print(nodePtr->children[i]);
    }
}

int main(int argc, char *argv[]) {
    setvbuf(stdout, NULL, _IONBF, 0); // for debug, remove before submitting

    // setup necessary data structures
    scannerBuffer = charBufferCreate();
    stringBuffer = StringBufferCreate();

    // LEXICAL ANALYSIS
    /*
    Token currentToken;
    while((currentToken = getToken()).type != TOK_EOF) {
        printToken(currentToken);
    }
    */

    // recursive descent start
    if (NT_Prog()) {
        printError("\n~~~~~~~~~~~~~~~~~~~~ \nSyntactic analysis: All OK\n~~~~~~~~~~~~~~~~~~~~\n\n");
    }
    else {
        printError("Syntactic error\n");
        deallocateAll();
        return SYNTAX_ERROR;
    }

    deallocateAll();
    return 0;
}

Token getToken_NL_required() {
    Token nextToken = getToken();

    if (nextToken.type != TOK_Newline) {
        printError("Expected newline, found none");
        deallocateAll();
        exit(SYNTAX_ERROR);
    }

    do {
        nextToken = getToken();
    } while (nextToken.type == TOK_Newline);

    return nextToken;
}

Token getToken_NL_optional() {
    Token nextToken;

    do {
        nextToken = getToken();
    } while (nextToken.type == TOK_Newline);

    return nextToken;
}

bool isExpFirst(tokenType type) {
    switch (type) {
        case TOK_Identifier:
            return true;
        case TOK_Float_Literal:
            return true;
        case TOK_Int_Literal:
            return true;
        case TOK_String_Literal:
            return true;
        case TOK_L_Paren:
            return true;

        default:
            return false;
    }
}

// initial non-terminal
bool NT_Prog() {
    bool ret = false;
    
    ASTNode *node = AST_CreateNode(NULL, NODE_Prog); // is root, has no parent

    ret = NT_Prolog() && NT_Func_Def_List(node);

    AST_Print(node); // print whole tree

    return ret;
}

bool NT_Func_Def_List(ASTNode *parentNode) {
    bool ret = false;

    Token nextToken = getToken_NL_optional();

    if (nextToken.type == TOK_EOF) {
        ret = true;
    }
    else if (nextToken.type == TOK_Func_Keyword) {

        ret = NT_Func_Def(parentNode) && NT_Func_Def_List(parentNode);
    }

    return ret;
}

bool NT_Func_Def(ASTNode *parentNode) {
    bool ret = false;

    ASTNode *node = AST_CreateNode(parentNode, NODE_Func_Def);

    if (getToken().type == TOK_Identifier) {
        if (getToken().type == TOK_L_Paren) {
            ret = NT_Param_List(node) && NT_Return_Types(node) && NT_Stat(node);
        }
    }

    return ret;
}

bool NT_Param_List(ASTNode *parentNode) {
    bool ret = false;

    Token nextToken = getToken();

    ASTNode *node = AST_CreateNode(parentNode, NODE_Func_Def_Params);

    if (nextToken.type == TOK_R_Paren) {
        ret = true;
    }
    else if (nextToken.type == TOK_Identifier) {
        ret = NT_Type(node) && NT_Param_List_N(node);
    }

    return ret;
}

bool NT_Param_List_N(ASTNode *parentNode) {
    bool ret = false;

    Token nextToken = getToken();

    if (nextToken.type == TOK_R_Paren) {
        ret = true;
    }
    else if (nextToken.type == TOK_Comma) {
        if (getToken_NL_optional().type == TOK_Identifier)  {
            ret = NT_Type(parentNode) && NT_Param_List_N(parentNode);
        }
    }

    return ret;
}

bool NT_Type(ASTNode *parentNode) {
    bool ret = false;

    Token nextToken = getToken();

    ASTNode *node = AST_CreateNode(parentNode, NODE_Type);

    if (nextToken.type == TOK_Int_Keyword) {
        ret = true;
    }
    else if (nextToken.type == TOK_Float_Keyword) {
        ret = true;
    }
    else if (nextToken.type == TOK_String_Keyword) {
        ret = true;
    }

    return ret;
}

bool NT_Return_Types(ASTNode *parentNode) {
    bool ret = false;

    Token nextToken = getToken();

    ASTNode *node = AST_CreateNode(parentNode, NODE_Func_Def_Return);

    if (nextToken.type == TOK_L_Brace) {
        ret = true;
    }
    else if (nextToken.type == TOK_L_Paren) {
        ret = NT_Type(node) && NT_Return_Types_N(node) && getToken().type == TOK_L_Brace;
    }

    return ret;
}

bool NT_Return_Types_N(ASTNode *parentNode) {
    bool ret = false;

    Token nextToken = getToken();

    if (nextToken.type == TOK_R_Paren) {
        ret = true;
    }
    else if (nextToken.type == TOK_Comma) {
        ret = NT_Type(parentNode) && NT_Return_Types_N(parentNode);
    }

    return ret;
}

bool NT_Prolog() {
    bool ret = false;

    if (getToken_NL_optional().type == TOK_Package_Keyword) {
        Token nextToken = getToken();       
        if (nextToken.type == TOK_Identifier && (strcmp(nextToken.str, "main") == 0)) {
            ret = true;
        }
    }

    return ret;
}

bool NT_Stat(ASTNode *parentNode) {
    bool ret = false;
    Token nextToken = getToken_NL_optional();

    if (nextToken.type == TOK_Identifier) {
        ret = NT_Var(parentNode) && NT_Stat(parentNode);
    }
    else if (nextToken.type == TOK_R_Brace) {
        ret = true;
    }
    else if (nextToken.type == TOK_If_Keyword) {
        ASTNode *node = AST_CreateNode(parentNode, NODE_If_Else);
        ret = NT_If_Else(node) && NT_Stat(parentNode);
    }
    else if (nextToken.type == TOK_For_Keyword) {
        ASTNode *node = AST_CreateNode(parentNode, NODE_For);
        ret = NT_For_Def() && NT_For_Exp() && NT_For_Assign() && NT_Stat(parentNode) && NT_Stat(parentNode);
    }
    else if (nextToken.type == TOK_Return_Keyword) {
        ASTNode *node = AST_CreateNode(parentNode, NODE_Return);
        ret = NT_Exps() && NT_Stat(parentNode);
    }

    return ret;
}

bool NT_Var(ASTNode *parentNode) {
    bool ret = false;

    Token nextToken = getToken();

    if (nextToken.type == TOK_Assign) {
        ASTNode *node = AST_CreateNode(parentNode, NODE_Assign);
        ret = NT_Exps();
    }
    else if (nextToken.type == TOK_Define) {
        ASTNode *node = AST_CreateNode(parentNode, NODE_Define);
        ret = NT_Exps();
    }
    else if (nextToken.type == TOK_Comma) {
        if (getToken().type == TOK_Identifier) {
            ret = NT_Var(parentNode);
        }
    }
    else if (nextToken.type == TOK_L_Paren) {
        ret = NT_Func_Args();
    }

    return ret;
}

bool NT_Exps() {
    bool ret = false;

    Token firstToken = getToken_NL_optional();

    if (isExpFirst(firstToken.type)) {
        Token secondToken = getToken();

        if (firstToken.type == TOK_Identifier && secondToken.type == TOK_L_Paren) {
            ret = NT_Func_Args();
        }
        else {
            overlapToken = secondToken;
            ret = NT_Exp(firstToken) && NT_Exps_N();
        }
    }

    return ret;
}

bool NT_Func_Args() {
    bool ret = false;

    Token nextToken = getToken();

    if (nextToken.type == TOK_R_Paren) {
        ret = true;
    }
    else {
        overlapToken = nextToken;
        ret = NT_Term() && NT_Func_Args_N();
    }

    return ret;
}

bool NT_Func_Args_N() {
    bool ret = false;

    Token nextToken = getToken();

    if (nextToken.type == TOK_R_Paren) {
        ret = true;
    }
    else if (nextToken.type == TOK_Comma) {
        ret = NT_Term() && NT_Func_Args_N();
    }

    return ret;
}

bool NT_Exps_N() {
    bool ret = false;

    Token nextToken = getToken();

    if (nextToken.type == TOK_Comma) {
        ret = NT_Exp(EMPTY_TOKEN) && NT_Exps_N();
    }
    else if (nextToken.type == TOK_Newline) {
        ret = true;
    }

    return ret;
}

bool NT_Assign_N() {
    bool ret = false;

    Token nextToken = getToken();

    if (nextToken.type == TOK_Assign) {
        ret = true;
    }
    else if (nextToken.type == TOK_Comma) {
        if (getToken().type == TOK_Identifier) {
            ret = NT_Assign_N() && NT_Exp(EMPTY_TOKEN) && getToken().type == TOK_Comma;
        }
    }

    return ret;
}

bool NT_Exp(Token overlapTokenIn) {
    bool ret = false;

    // pass it to expression parser
    Token* overlapTokenOut = malloc(sizeof(Token));
    ret = handleExpression(overlapTokenIn, overlapTokenOut);

    overlapToken = *overlapTokenOut;
    free(overlapTokenOut);

    return ret;
}

bool NT_If_Else(ASTNode *parentNode) {
    bool ret = false;

    if (NT_Exp(EMPTY_TOKEN)) {
        if (getToken().type == TOK_L_Brace) {
            if (NT_Stat(parentNode)) {
                if (getToken().type == TOK_Else_Keyword) {
                    if (getToken().type == TOK_L_Brace) {
                        ret = NT_Stat(parentNode);
                    }
                }
            }
        }
    }

    return ret;
}

bool NT_For_Def() {
    bool ret = false;

    Token nextToken = getToken();

    if (nextToken.type == TOK_Semicolon) {
        ret = true;
    }
    else if (nextToken.type == TOK_Identifier) {
        ret = NT_For_Def_Var();
    }

    return ret;
}

bool NT_For_Def_Var() {
    bool ret = false;

    Token nextToken = getToken();

    if (nextToken.type == TOK_Comma) {
        if (getToken().type == TOK_Identifier) {
            ret = NT_For_Def_Var();
        }
    }
    else if (nextToken.type == TOK_Define) {
        ret = NT_Exp(EMPTY_TOKEN) && NT_For_Def_Exp_N();
    }

    return ret;
}


bool NT_For_Def_Exp_N() {
    bool ret = false;

    Token nextToken = getToken();

    if (nextToken.type == TOK_Semicolon) {
        ret = true;
    }
    else if (nextToken.type == TOK_Comma) {
        ret = NT_Exp(EMPTY_TOKEN) && NT_For_Def_Exp_N();
    }

    return ret;
}

bool NT_For_Exp() {
    bool ret = false;

    Token nextToken = getToken_NL_optional();

    if (isExpFirst(nextToken.type)) {
        ret = NT_Exp(nextToken) && getToken().type == TOK_Semicolon;
    }

    return ret;
}

bool NT_For_Assign() {
    bool ret = false;

    Token nextToken = getToken();

    if (nextToken.type == TOK_L_Brace) {
        ret = true;
    }
    else if (nextToken.type == TOK_Identifier) {
        ret = NT_For_Assign_Var();
    }

    return ret;
}

bool NT_For_Assign_Var() {
    bool ret = false;

    Token nextToken = getToken();

    if (nextToken.type == TOK_Comma) {
        if (getToken().type == TOK_Identifier) {
            ret = NT_For_Assign_Var();
        }
    }
    else if (nextToken.type == TOK_Assign) {
        ret = NT_Exp(EMPTY_TOKEN) && NT_For_Assign_Exp_N();
    }

    return ret;
}

bool NT_For_Assign_Exp_N() {
    bool ret = false;

    Token nextToken = getToken();

    if (nextToken.type == TOK_L_Brace) {
        ret = true;
    }
    else if (nextToken.type == TOK_Comma) {
        ret = NT_Exp(EMPTY_TOKEN) && NT_For_Assign_Exp_N();
    }

    return ret;
}

bool NT_Term() {
    bool ret = false;

    Token nextToken = getToken_NL_optional();

    if (nextToken.type == TOK_Identifier) {
        ret = true;
    }
    else if (nextToken.type == TOK_Int_Literal) {
        ret = true;
    }
    else if (nextToken.type == TOK_Float_Literal) {
        ret = true;
    }
    else if (nextToken.type == TOK_String_Literal) {
        ret = true;
    }

    return ret;
}
