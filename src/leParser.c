/* ========= parserMock.c ==========
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

int main(int argc, char* argv[]) {
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
        printError("All OK\n");
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
        case TOK_Float_Literal:
        case TOK_Int_Literal:
        case TOK_String_Literal:
        case TOK_L_Paren:
            return true;
        
        default:
            return false;
    }
}

// initial non-terminal
bool NT_Prog() {
    bool ret = false;

    ret = NT_Prolog() && NT_Func_Def_List();

    return ret;
}

bool NT_Func_Def_List() {
    bool ret = false;

    Token nextToken = getToken_NL_optional();

    if (nextToken.type == TOK_EOF) {
        ret = true;
    }
    else if (nextToken.type == TOK_Func_Keyword) {
        ret = NT_Func_Def() && NT_Func_Def_List();
    }

    return ret;
}

bool NT_Func_Def() {
    bool ret = false;

    if (getToken().type == TOK_Identifier) {
        if (getToken().type == TOK_L_Paren) {
            ret = NT_Param_List() && NT_Return_Types() && NT_Stat();
        }
    }

    return ret;
}

bool NT_Param_List() {
    bool ret = false;

    Token nextToken = getToken();

    if (nextToken.type == TOK_R_Paren) {
        ret = true;
    }
    else if (nextToken.type == TOK_Identifier) {
        ret = NT_Type() && NT_Param_List_N();
    }

    return ret;
}

bool NT_Param_List_N() {
    bool ret = false;

    Token nextToken = getToken();

    if (nextToken.type == TOK_R_Paren) {
        ret = true;
    }
    else if (nextToken.type == TOK_Comma) {
        if (getToken().type == TOK_Identifier)  {
            ret = NT_Type() && NT_Param_List_N();
        }
    }

    return ret;
}

bool NT_Type() {
    bool ret = false;

    Token nextToken = getToken();

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

bool NT_Return_Types() {
    bool ret = false;

    Token nextToken = getToken();

    if (nextToken.type == TOK_L_Brace) {
        ret = true;
    }
    else if (nextToken.type == TOK_L_Paren) {
        ret = NT_Type() && NT_Return_Types_N() && getToken().type == TOK_L_Brace;
    }

    return ret;
}

bool NT_Return_Types_N() {
    bool ret = false;

    Token nextToken = getToken();

    if (nextToken.type == TOK_R_Paren) {
        ret = true;
    }
    else if (nextToken.type == TOK_Comma) {
        ret = NT_Type() && NT_Return_Types_N();
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

bool NT_Stat() {
    bool ret = false;

    Token nextToken = getToken_NL_optional();

    if (nextToken.type == TOK_Identifier) {
        ret = NT_Var() && NT_Stat();
    }
    else if (nextToken.type == TOK_R_Brace) {
        ret = true;
    }
    else if (nextToken.type == TOK_If_Keyword) {
        ret = NT_If_Else() && NT_Stat();
    }
    else if (nextToken.type == TOK_For_Keyword) {
        ret = NT_For_Def() && NT_For_Exp() && NT_For_Assign() && NT_Stat() && NT_Stat();
    }
    else if (nextToken.type == TOK_Return_Keyword) {
        ret = NT_Exps() && NT_Stat();
    }

    return ret;
}

bool NT_Var() {
    bool ret = false;

    Token nextToken = getToken();

    if (nextToken.type == TOK_Assign) {
        ret = NT_Exps();
    }
    else if (nextToken.type == TOK_Define) {
        ret = NT_Exps();
    }
    else if (nextToken.type == TOK_Comma) {
        if (getToken().type == TOK_Identifier) {
            ret = NT_Var();
        }
    }
    else if (nextToken.type == TOK_L_Paren) {
        ret = NT_Func_Args();
    }

    return ret;
}

bool NT_Exps() {
    bool ret = false;

    Token firstToken = getToken();

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

bool NT_If_Else() {
    bool ret = false;

    if (NT_Exp(EMPTY_TOKEN)) {
        if (getToken().type == TOK_L_Brace) {
            if (NT_Stat()) {
                if (getToken().type == TOK_Else_Keyword) {
                    if (getToken().type == TOK_L_Brace) {
                        ret = NT_Stat();
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

    Token nextToken = getToken();

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

    Token nextToken = getToken();

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
