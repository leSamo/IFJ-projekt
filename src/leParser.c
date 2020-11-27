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
#include "symtable.c"
#include "semanticAnalysis.c"

int main(int argc, char *argv[]) {
    setvbuf(stdout, NULL, _IONBF, 0); // for debug, remove before submitting

    // setup necessary data structures
    scannerBuffer = charBufferCreate();
    stringBuffer = StringBufferCreate();
    tokenBuffer = TokenBufferCreate();

    // LEXICAL ANALYSIS
    /*
    Token currentToken;
    while((currentToken = getToken()).type != TOK_EOF) {
        printToken(currentToken);
    }
    */

    // recursive descent start
    if (NT_Prog()) {
        printf("\n~~~~~~~~~~~~~~~~~~~~ \nSyntactic analysis: All OK\n~~~~~~~~~~~~~~~~~~~~\n\n");
    }
    else {
        printError(SYNTAX_ERROR, "Syntactic error\n");
        deallocateAll();
        return SYNTAX_ERROR;
    }

    deallocateAll();
    return 0;
}

Token getToken_NL_required() {
    Token nextToken = getToken();

    if (nextToken.type != TOK_Newline) {
        printError(SYNTAX_ERROR, "Expected newline, found none");
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

    AST_PrettyPrint(ASTRoot, 0); // print whole AST
    printf("=========================\n");
    
    /* Semantic analysis */
    ST_Init(&SymTableTree);

    AST_FirstPass(ASTRoot, &SymTableTree);
    AST_SecondPass(ASTRoot, &SymTableTree);

    ST_PrettyPrint(SymTableTree, 0);

    ST_Dispose(&SymTableTree);

    /*
    FillTable_FirstPass(ASTRoot);
    
    Print_tree(SymTableTree);
    
    FillTable_SecondPass(ASTRoot, 0);
    //CheckSemant(ASTRoot, 0);

    if (SemanticError == 0){
        printf("\n~~~~~~~~~~~~~~~~~~~~ \nSemantic analysis: All OK\n~~~~~~~~~~~~~~~~~~~~\n\n");
    }
    */

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

    Token identifierNode = getToken();

    if (identifierNode.type == TOK_Identifier) {
        if (getToken().type == TOK_L_Paren) {
            ASTNode *funcNode = AST_CreateStringNode(parentNode, NODE_Func_Def, identifierNode.str);
            ASTNode *blockNode = AST_CreateNode(funcNode, NODE_Block);
            ret = NT_Param_List(funcNode) && NT_Return_Types(funcNode) && NT_Stat(blockNode);
        }
    }

    return ret;
}

bool NT_Param_List(ASTNode *parentNode) {
    bool ret = false;

    Token nextToken = getToken();

    ASTNode *node = AST_CreateNode(parentNode, NODE_Func_Def_Param_List);

    if (nextToken.type == TOK_R_Paren) {
        ret = true;
    }
    else if (nextToken.type == TOK_Identifier) {
        ASTNode *paramNode = AST_CreateNode(node, NODE_Func_Def_Param);
        ASTNode *idNode = AST_CreateStringNode(paramNode, NODE_Identifier, nextToken.str);

        ret = NT_Type(paramNode) && NT_Param_List_N(node);
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
        Token idToken = getToken_NL_optional();
        if (idToken.type == TOK_Identifier)  {
            ASTNode *paramNode = AST_CreateNode(parentNode, NODE_Func_Def_Param);
            ASTNode *idNode = AST_CreateStringNode(paramNode, NODE_Identifier, idToken.str);
            
            ret = NT_Type(paramNode) && NT_Param_List_N(parentNode);
        }
    }

    return ret;
}

bool NT_Type(ASTNode *parentNode) {
    bool ret = false;

    Token nextToken = getToken();

    if (nextToken.type == TOK_Int_Keyword) {
        AST_CreateTaggedNode(parentNode, NODE_Type_Int, TAG_Int);
        ret = true;
    }
    else if (nextToken.type == TOK_Float_Keyword) {
        AST_CreateTaggedNode(parentNode, NODE_Type_Float, TAG_Float);
        ret = true;
    }
    else if (nextToken.type == TOK_String_Keyword) {
        AST_CreateTaggedNode(parentNode, NODE_Type_String, TAG_String);
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

    if (!TokenBufferEmpty(tokenBuffer)) {
        printf("Unbalanced construction error\n");
        return false;
    }

    if (nextToken.type == TOK_Identifier) {
        TokenBufferPush(tokenBuffer, nextToken);
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
        ASTNode *blockNode = AST_CreateNode(node, NODE_Block);
        ret = NT_For_Def(node) && NT_For_Exp(node) && NT_For_Assign(node) && NT_Stat(blockNode) && NT_Stat(parentNode);
    }
    else if (nextToken.type == TOK_Return_Keyword) {
        ASTNode *node = AST_CreateNode(parentNode, NODE_Return);
        ret = NT_Exps(node, false) && NT_Stat(parentNode);
    }

    return ret;
}

bool NT_Var(ASTNode *parentNode) {
    bool ret = false;

    Token nextToken = getToken();

    if (nextToken.type == TOK_Assign) {
        ASTNode *assignNode = AST_CreateNode(parentNode, NODE_Assign);
        ASTNode *idNode = AST_CreateStringNode(assignNode, NODE_Identifier, TokenBufferPopFront(&tokenBuffer).str);
        ret = NT_Exp_Or_Fn(assignNode);
    }
    else if (nextToken.type == TOK_Define) {
        ASTNode *defineNode = AST_CreateNode(parentNode, NODE_Define);
        ASTNode *idNode = AST_CreateStringNode(defineNode, NODE_Identifier, TokenBufferPopFront(&tokenBuffer).str);
        ret = NT_Exp(defineNode, EMPTY_TOKEN, false);
    }
    else if (nextToken.type == TOK_Comma) {
        Token idToken = getToken();
        if (idToken.type == TOK_Identifier) {
            TokenBufferPush(tokenBuffer, idToken);
            ret = NT_Var_N(parentNode);
        }
    }
    else if (nextToken.type == TOK_L_Paren) {
        ASTNode *node = AST_CreateStringNode(parentNode, NODE_Func_Call, TokenBufferPopFront(&tokenBuffer).str);
        ret = NT_Func_Args(node);
    }

    return ret;
}

bool NT_Var_N(ASTNode *parentNode) {
    bool ret = false;

    Token nextToken = getToken();

    if (nextToken.type == TOK_Assign) {
        ret = NT_Exps(parentNode, true);
    }
    else if (nextToken.type == TOK_Comma) {
        Token idToken = getToken();
        if (idToken.type == TOK_Identifier) {
            TokenBufferPush(tokenBuffer, idToken);
            ret = NT_Var_N(parentNode);
        }
    }
    /*
    else if (nextToken.type == TOK_L_Paren) {
        ASTNode *assignNode = AST_CreateNode(parentNode, NODE_Assign);
        ASTNode *multiNode = AST_CreateNode(assignNode, NODE_Multi_L_Value);

        while (tokenBuffer->count > 1) {
            ASTNode *value = AST_CreateStringNode(multiNode, NODE_Identifier, TokenBufferPopFront(&tokenBuffer).str);
        }

        ASTNode *funcNode = AST_CreateStringNode(parentNode, NODE_Func_Call, TokenBufferPopFront(&tokenBuffer).str);
        ret = NT_Func_Args(funcNode);
    }
    */

    return ret;
}

bool NT_Exps(ASTNode *parentNode, bool createAssignment) {
    bool ret = false;

    Token firstToken = getToken_NL_optional();

    if (isExpFirst(firstToken.type)) {
        Token secondToken = getToken();

        if (firstToken.type == TOK_Identifier && secondToken.type == TOK_L_Paren) {
            ASTNode *assignNode;
            
            if (createAssignment) {
                assignNode = AST_CreateNode(parentNode, NODE_Assign);
                ASTNode *multiNode = AST_CreateNode(assignNode, NODE_Multi_L_Value);

                while (!TokenBufferEmpty(tokenBuffer)) {
                    ASTNode *value = AST_CreateStringNode(multiNode, NODE_Identifier, TokenBufferPopFront(&tokenBuffer).str);
                }
            }
            else {
               assignNode = parentNode;
            }

            ASTNode *funcNode = AST_CreateStringNode(assignNode, NODE_Func_Call, firstToken.str);
            ret = NT_Func_Args(funcNode);
        }
        else {
            overlapToken = secondToken;
            ret = NT_Exp(parentNode, firstToken, createAssignment) && NT_Exps_N(parentNode, createAssignment);
        }
    }

    return ret;
}

bool NT_Exp_Or_Fn(ASTNode *parentNode) {
    bool ret = false;

    Token firstToken = getToken_NL_optional();

    if (isExpFirst(firstToken.type)) {
        Token secondToken = getToken();

        if (firstToken.type == TOK_Identifier && secondToken.type == TOK_L_Paren) {
            ASTNode *funcNode = AST_CreateStringNode(parentNode, NODE_Func_Call, firstToken.str);
            ret = NT_Func_Args(funcNode);
        }
        else {
            overlapToken = secondToken;
            ret = NT_Exp(parentNode, firstToken, false);
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

bool NT_Exps_N(ASTNode *parentNode, bool createAssignment) {
    bool ret = false;

    Token nextToken = getToken();

    if (nextToken.type == TOK_Comma) {
        ret = NT_Exp(parentNode, EMPTY_TOKEN, createAssignment) && NT_Exps_N(parentNode, createAssignment);
    }
    else if (nextToken.type == TOK_Newline) {
        if (!TokenBufferEmpty(tokenBuffer)) {
            printf("Unbalanced construction error\n");
            return false;
        }

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
            ret = NT_Assign_N(parentNode) && NT_Exp(parentNode, EMPTY_TOKEN, false) && getToken().type == TOK_Comma;
        }
    }

    return ret;
}

bool NT_Exp(ASTNode *parentNode, Token overlapTokenIn, bool createAssignment) {
    bool ret = false;

    ASTNode *node;

    if (createAssignment) {
        ASTNode *assignNode = AST_CreateNode(parentNode, NODE_Assign);
        ASTNode *idNode = AST_CreateStringNode(assignNode, NODE_Identifier, TokenBufferPopFront(&tokenBuffer).str);
        node = AST_CreateNode(assignNode, NODE_Exp);
    }
    else {
        node = AST_CreateNode(parentNode, NODE_Exp);
    }

    // pass it to expression parser
    Token* overlapTokenOut = malloc(sizeof(Token));
    ret = handleExpression(node, overlapTokenIn, overlapTokenOut);

    overlapToken = *overlapTokenOut;
    free(overlapTokenOut);

    return ret;
}

bool NT_If_Else(ASTNode *parentNode) {
    bool ret = false;

    if (NT_Exp(parentNode, EMPTY_TOKEN, false)) {
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
        ASTNode *assignNode = AST_CreateNode(parentNode, NODE_Define);
        ASTNode *idNode = AST_CreateStringNode(assignNode, NODE_Identifier, nextToken.str);
        ret = NT_For_Def_Var(assignNode);
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
        ret = NT_Exp(parentNode, EMPTY_TOKEN, false) && getToken().type == TOK_Semicolon;
    }

    return ret;
}

bool NT_For_Exp(ASTNode *parentNode) {
    bool ret = false;

    Token nextToken = getToken_NL_optional();

    if (isExpFirst(nextToken.type)) {
        ret = NT_Exp(parentNode, nextToken, false) && getToken().type == TOK_Semicolon;
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
        TokenBufferPush(tokenBuffer, nextToken);
        ret = NT_For_Assign_Var(parentNode);
    }

    return ret;
}

bool NT_For_Assign_Var(ASTNode *parentNode) {
    bool ret = false;

    Token nextToken = getToken();

    if (nextToken.type == TOK_Comma) {
        Token secondToken = getToken();
        if (secondToken.type == TOK_Identifier) {
            TokenBufferPush(tokenBuffer, secondToken);
            ret = NT_For_Assign_Var(parentNode);
        }
    }
    else if (nextToken.type == TOK_Assign) {
        ret = NT_Exp(parentNode, EMPTY_TOKEN, true) && NT_For_Assign_Exp_N(parentNode);
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
        ret = NT_Exp(parentNode, EMPTY_TOKEN, true) && NT_For_Assign_Exp_N(parentNode);
    }

    return ret;
}

bool NT_Term(ASTNode *parentNode) {
    bool ret = false;

    Token nextToken = getToken_NL_optional();

    if (nextToken.type == TOK_Identifier) {
        AST_CreateStringNode(parentNode, NODE_Identifier, nextToken.str);
        ret = true;
    }
    else if (nextToken.type == TOK_Int_Literal) {
        AST_CreateIntNode(parentNode, NODE_Literal_Int, nextToken.i);
        ret = true;
    }
    else if (nextToken.type == TOK_Float_Literal) {
        AST_CreateFloatNode(parentNode, NODE_Literal_Float, nextToken.f);
        ret = true;
    }
    else if (nextToken.type == TOK_String_Literal) {
        AST_CreateStringNode(parentNode, NODE_Literal_String, nextToken.str);
        ret = true;
    }

    return ret;
}
