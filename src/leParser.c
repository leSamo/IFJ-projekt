/* ========== leParser.c ===========
 * Project: IFJ 2020/21 project
 * Team: 067, variant I
 * Author: Emma Krompaščíková (xkromp00), Samuel Olekšák (xoleks00), Michal Findra (xfindr00)
 * Date: November, December 2020
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
#include "semanticAnalysis.c"
#include "codeGenerator.c"

int main(int argc, char *argv[]) {
    setvbuf(stdout, NULL, _IONBF, 0); // for debug, remove before submitting

    // setup necessary data structures
    scannerBuffer = charBufferCreate();
    stringBuffer = StringBufferCreate();
    tokenBuffer = TokenBufferCreate();

    // [DEBUG] Print all tokens
    /*
    Token currentToken;
    while((currentToken = getToken()).type != TOK_EOF) {
        printToken(currentToken);
    }
    */

    /* Syntactic analysis */
    bool semanticAnalysisSucess = NT_Prog();

    if (!semanticAnalysisSucess) {
        throwError(SYNTAX_ERROR, "Syntactic error\n", true);
    }

    //AST_PrettyPrint(ASTRoot, 0); // print whole AST

    /* Semantic analysis */
    ST_Init(&SymTableTree);

    AST_FirstPass(ASTRoot, &SymTableTree);
    AST_SecondPass(ASTRoot, &SymTableTree);
    
    //ST_PrettyPrint(SymTableTree, 0); // print symtable

    /* Final code generation */
    generateCode(ASTRoot, mainFuncDef, SymTableTree);
    ST_Dispose(&SymTableTree);

    // successfuly compiled input program
    deallocateAll();
    return 0;
}

Token getToken_NL_required() {
    Token nextToken = getToken();

    if (nextToken.type != TOK_Newline) {
        throwError(SYNTAX_ERROR, "Expected newline, found none\n", true);
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
            // semantic action: create function definition node
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
        // semantic action: create function parameter node
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
            // semantic action: create function parameter node
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
        ret = NT_Return_Types_Start(node);
    }

    return ret;
}

bool NT_Return_Types_Start(ASTNode *parentNode) {
    bool ret = false;

    Token nextToken = getToken();

    if (nextToken.type == TOK_R_Paren) {
        ret = getToken().type == TOK_L_Brace;
    }
    else {
        overlapToken = nextToken; // return one token back to the scanner
        ret = NT_Type(parentNode) && NT_Return_Types_N(parentNode) && getToken().type == TOK_L_Brace;
    }
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
    Token nextToken = getToken_NL_required();

    // if there was a multi-assignment check if amount of L-values and R-values was the same
    if (!TokenBufferEmpty(tokenBuffer)) {
        fprintf(stderr, "Unbalanced construction error\n");
        return false;
    }

    if (nextToken.type == TOK_Identifier) {
        // push token to buffer, in case this will be multi-assignment
        TokenBufferPush(tokenBuffer, nextToken);
        
        ret = NT_Var(parentNode) && NT_Stat(parentNode);
    }
    else if (nextToken.type == TOK_R_Brace) {
        ret = true;
    }
    else if (nextToken.type == TOK_If_Keyword) {
        // semantic action: create if-else parameter node
        ASTNode *node = AST_CreateNode(parentNode, NODE_If_Else);

        ret = NT_If_Else(node) && NT_Stat(parentNode);
    }
    else if (nextToken.type == TOK_For_Keyword) {
        // semantic action: create for node
        ASTNode *node = AST_CreateNode(parentNode, NODE_For);
        ASTNode *blockNode = AST_CreateNode(node, NODE_Block);

        ret = NT_For_Def(node) && NT_For_Exp(node) && NT_For_Assign(node) && NT_Stat(blockNode) && NT_Stat(parentNode);
    }
    else if (nextToken.type == TOK_Return_Keyword) {
        Token secondToken = getToken();

        if (isExpFirst(secondToken.type)) {
            overlapToken = secondToken; // return one token back to the scanner

            ASTNode *node = AST_CreateNode(parentNode, NODE_Return);

            ret = NT_Exps(node, false) && NT_Stat(parentNode);
        }
        else if (secondToken.type == TOK_Newline) {
            overlapToken = secondToken; // return one token back to the scanner

            ASTNode *node = AST_CreateNode(parentNode, NODE_Return);

            ret = NT_Stat(parentNode);
        }

    }

    return ret;
}

bool NT_Var(ASTNode *parentNode) {
    bool ret = false;

    Token nextToken = getToken();

    if (nextToken.type == TOK_Assign) {
         // semantic action: create assignment node
        ASTNode *assignNode = AST_CreateNode(parentNode, NODE_Assign);
        ASTNode *idNode = AST_CreateStringNode(assignNode, NODE_Identifier, TokenBufferPopFront(&tokenBuffer).str);
        
        ret = NT_Exp_Or_Fn(assignNode);
    }
    else if (nextToken.type == TOK_Define) {
         // semantic action: create definition node
        ASTNode *defineNode = AST_CreateNode(parentNode, NODE_Define);
        ASTNode *idNode = AST_CreateStringNode(defineNode, NODE_Identifier, TokenBufferPopFront(&tokenBuffer).str);
        
        ret = NT_Exp(defineNode, EMPTY_TOKEN, false);
    }
    else if (nextToken.type == TOK_Comma) {
        Token idToken = getToken();

        if (idToken.type == TOK_Identifier) {
            // reading multi-assignment, push to token buffer to be later converted into multiple assign nodes
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
            // reading multi-assignment, push to token buffer to be later converted into multiple assign nodes
            TokenBufferPush(tokenBuffer, idToken);
            ret = NT_Var_N(parentNode);
        }
    }

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

                // reading multi-assignment, pop identificators and pair them with expression to create assignment nodes
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
            overlapToken = secondToken; // return one token back to the scanner

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
            overlapToken = secondToken; // return one token back to the scanner
            
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
        overlapToken = nextToken; // return one token back to the scanner
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
        // there was a multi-assignment check if amount of L-values and R-values was the same
        if (!TokenBufferEmpty(tokenBuffer)) {
            fprintf(stderr, "Unbalanced construction error\n");
            return false;
        }
        
        overlapToken = nextToken; // return one token back to the scanner
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

    if (createAssignment) { // semantic action: create assignment node
        ASTNode *assignNode = AST_CreateNode(parentNode, NODE_Assign);
        AST_CreateStringNode(assignNode, NODE_Identifier, TokenBufferPopFront(&tokenBuffer).str);
        
        node = AST_CreateNode(assignNode, NODE_Exp);
    }
    else {
        node = AST_CreateNode(parentNode, NODE_Exp);
    }

    // provide expression parser pointer to return token it might have read and should not have
    Token *overlapTokenOut = malloc(sizeof(Token));

    // pass it to expression parser
    ret = handleExpression(node, overlapTokenIn, overlapTokenOut);

    overlapToken = *overlapTokenOut; // return one token back to the scanner
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
        // push token to buffer, in case this will be multi-assignment
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
            // push token to buffer, in case this will be multi-assignment
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
