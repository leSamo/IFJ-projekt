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
    
    ASTRoot = AST_CreateNode(NULL, NODE_Prog); // is root, has no parent

    ret = NT_Prolog() && NT_Func_Def_List(ASTRoot);

    AST_Print(ASTRoot); // print whole tree

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

    if (getToken().type == TOK_Identifier) {
        if (getToken().type == TOK_L_Paren) {
            ASTNode *funcNode = AST_CreateNode(parentNode, NODE_Func_Def);
            ASTNode *blockNode = AST_CreateNode(funcNode, NODE_Block);
            ret = NT_Param_List(funcNode) && NT_Return_Types(funcNode) && NT_Stat(blockNode);
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

    if (nextToken.type == TOK_Int_Keyword) {
        AST_CreateNode(parentNode, NODE_Type_Int);
        ret = true;
    }
    else if (nextToken.type == TOK_Float_Keyword) {
        AST_CreateNode(parentNode, NODE_Type_Float);
        ret = true;
    }
    else if (nextToken.type == TOK_String_Keyword) {
        AST_CreateNode(parentNode, NODE_Type_String);
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
        ret = NT_Var(parentNode, nextToken) && NT_Stat(parentNode);
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
        ASTNode *blockNode = AST_CreateNode(node, NODE_Block);
        ret = NT_For_Def(node) && NT_For_Exp(node) && NT_For_Assign(node) && NT_Stat(blockNode) && NT_Stat(parentNode);
    }
    else if (nextToken.type == TOK_Return_Keyword) {
        ASTNode *node = AST_CreateNode(parentNode, NODE_Return);
        ret = NT_Exps(node) && NT_Stat(parentNode);
    }

    return ret;
}

bool NT_Var(ASTNode *parentNode, Token identifierToken) {
    bool ret = false;

    Token nextToken = getToken();

    if (nextToken.type == TOK_Assign) {
        ASTNode *assignNode = AST_CreateNode(parentNode, NODE_Assign);
        ASTNode *idNode = AST_CreateNode(assignNode, NODE_Identifier);
        ret = NT_Exps(assignNode);
    }
    else if (nextToken.type == TOK_Define) {
        ASTNode *defineNode = AST_CreateNode(parentNode, NODE_Define);
        ASTNode *idNode = AST_CreateNode(defineNode, NODE_Identifier);
        ret = NT_Exp(defineNode, EMPTY_TOKEN);
    }
    else if (nextToken.type == TOK_Comma) {
        Token secondToken = getToken();
        if (secondToken.type == TOK_Identifier) {
            ret = NT_Var_N(parentNode, secondToken);
        }
    }
    else if (nextToken.type == TOK_L_Paren) {
        ASTNode *node = AST_CreateNode(parentNode, NODE_Func_Call);
        ret = NT_Func_Args(node);
    }

    return ret;
}

bool NT_Var_N(ASTNode *parentNode, Token identifierToken) {
    bool ret = false;

    Token nextToken = getToken();

    if (nextToken.type == TOK_Assign) {
        ASTNode *assignNode = AST_CreateNode(parentNode, NODE_Assign);
        ASTNode *idNode = AST_CreateNode(parentNode, NODE_Identifier);
        ret = NT_Exps(assignNode);
    }
    else if (nextToken.type == TOK_Comma) {
        Token secondToken = getToken();
        if (secondToken.type == TOK_Identifier) {
            ret = NT_Var_N(parentNode, secondToken);
        }
    }
    else if (nextToken.type == TOK_L_Paren) {
        ASTNode *node = AST_CreateNode(parentNode, NODE_Func_Call);
        ret = NT_Func_Args(node);
    }

    return ret;
}

bool NT_Exps(ASTNode *parentNode) {
    bool ret = false;

    Token firstToken = getToken_NL_optional();

    if (isExpFirst(firstToken.type)) {
        Token secondToken = getToken();

        if (firstToken.type == TOK_Identifier && secondToken.type == TOK_L_Paren) {
            ret = NT_Func_Args(parentNode);
        }
        else {
            overlapToken = secondToken;
            ret = NT_Exp(parentNode, firstToken) && NT_Exps_N(parentNode);
        }
    }

    return ret;
}

bool NT_Func_Args(ASTNode *parentNode) {
    bool ret = false;

    Token nextToken = getToken();

    if (nextToken.type == TOK_R_Paren) {
        ret = true;
    }
    else {
        overlapToken = nextToken;
        ret = NT_Term(parentNode) && NT_Func_Args_N(parentNode);
    }

    return ret;
}

bool NT_Func_Args_N(ASTNode *parentNode) {
    bool ret = false;

    Token nextToken = getToken();

    if (nextToken.type == TOK_R_Paren) {
        ret = true;
    }
    else if (nextToken.type == TOK_Comma) {
        ret = NT_Term(parentNode) && NT_Func_Args_N(parentNode);
    }

    return ret;
}

bool NT_Exps_N(ASTNode *parentNode) {
    bool ret = false;

    Token nextToken = getToken();

    if (nextToken.type == TOK_Comma) {
        ret = NT_Exp(parentNode, EMPTY_TOKEN) && NT_Exps_N(parentNode);
    }
    else if (nextToken.type == TOK_Newline) {
        ret = true;
    }

    return ret;
}

bool NT_Assign_N(ASTNode *parentNode) {
    bool ret = false;

    Token nextToken = getToken();

    if (nextToken.type == TOK_Assign) {
        ret = true;
    }
    else if (nextToken.type == TOK_Comma) {
        if (getToken().type == TOK_Identifier) {
            ret = NT_Assign_N(parentNode) && NT_Exp(parentNode, EMPTY_TOKEN) && getToken().type == TOK_Comma;
        }
    }

    return ret;
}

bool NT_Exp(ASTNode *parentNode, Token overlapTokenIn) {
    bool ret = false;

    ASTNode *node = AST_CreateNode(parentNode, NODE_Exp);

    // pass it to expression parser
    Token* overlapTokenOut = malloc(sizeof(Token));
    ret = handleExpression(overlapTokenIn, overlapTokenOut);

    overlapToken = *overlapTokenOut;
    free(overlapTokenOut);

    return ret;
}

bool NT_If_Else(ASTNode *parentNode) {
    bool ret = false;

    if (NT_Exp(parentNode, EMPTY_TOKEN)) {
        if (getToken().type == TOK_L_Brace) {
            ASTNode *positiveNode = AST_CreateNode(parentNode, NODE_Block);
            if (NT_Stat(positiveNode)) {
                if (getToken().type == TOK_Else_Keyword) {
                    if (getToken().type == TOK_L_Brace) {
                        ASTNode *negativeNode = AST_CreateNode(parentNode, NODE_Block);
                        ret = NT_Stat(negativeNode);
                    }
                }
            }
        }
    }

    return ret;
}

bool NT_For_Def(ASTNode *parentNode) {
    bool ret = false;

    Token nextToken = getToken();

    if (nextToken.type == TOK_Semicolon) {
        ret = true;
    }
    else if (nextToken.type == TOK_Identifier) {
        ret = NT_For_Def_Var(parentNode);
    }

    return ret;
}

bool NT_For_Def_Var(ASTNode *parentNode) {
    bool ret = false;

    Token nextToken = getToken();

    if (nextToken.type == TOK_Comma) {
        if (getToken().type == TOK_Identifier) {
            ret = NT_For_Def_Var(parentNode);
        }
    }
    else if (nextToken.type == TOK_Define) {
        ret = NT_Exp(parentNode, EMPTY_TOKEN) && NT_For_Def_Exp_N(parentNode);
    }

    return ret;
}


bool NT_For_Def_Exp_N(ASTNode *parentNode) {
    bool ret = false;

    Token nextToken = getToken();

    if (nextToken.type == TOK_Semicolon) {
        ret = true;
    }
    else if (nextToken.type == TOK_Comma) {
        ret = NT_Exp(parentNode, EMPTY_TOKEN) && NT_For_Def_Exp_N(parentNode);
    }

    return ret;
}

bool NT_For_Exp(ASTNode *parentNode) {
    bool ret = false;

    Token nextToken = getToken_NL_optional();

    if (isExpFirst(nextToken.type)) {
        ret = NT_Exp(parentNode, nextToken) && getToken().type == TOK_Semicolon;
    }

    return ret;
}

bool NT_For_Assign(ASTNode *parentNode) {
    bool ret = false;

    Token nextToken = getToken();

    if (nextToken.type == TOK_L_Brace) {
        ret = true;
    }
    else if (nextToken.type == TOK_Identifier) {
        ret = NT_For_Assign_Var(parentNode);
    }

    return ret;
}

bool NT_For_Assign_Var(ASTNode *parentNode) {
    bool ret = false;

    Token nextToken = getToken();

    if (nextToken.type == TOK_Comma) {
        if (getToken().type == TOK_Identifier) {
            ret = NT_For_Assign_Var(parentNode);
        }
    }
    else if (nextToken.type == TOK_Assign) {
        ret = NT_Exp(parentNode, EMPTY_TOKEN) && NT_For_Assign_Exp_N(parentNode);
    }

    return ret;
}

bool NT_For_Assign_Exp_N(ASTNode *parentNode) {
    bool ret = false;

    Token nextToken = getToken();

    if (nextToken.type == TOK_L_Brace) {
        ret = true;
    }
    else if (nextToken.type == TOK_Comma) {
        ret = NT_Exp(parentNode, EMPTY_TOKEN) && NT_For_Assign_Exp_N(parentNode);
    }

    return ret;
}

bool NT_Term(ASTNode *parentNode) {
    bool ret = false;

    Token nextToken = getToken_NL_optional();

    if (nextToken.type == TOK_Identifier) {
        AST_CreateNode(parentNode, NODE_Identifier);
        ret = true;
    }
    else if (nextToken.type == TOK_Int_Literal) {
        AST_CreateNode(parentNode, NODE_Literal_Int);
        ret = true;
    }
    else if (nextToken.type == TOK_Float_Literal) {
        AST_CreateNode(parentNode, NODE_Literal_Float);
        ret = true;
    }
    else if (nextToken.type == TOK_String_Literal) {
        AST_CreateNode(parentNode, NODE_Literal_String);
        ret = true;
    }

    return ret;
}
