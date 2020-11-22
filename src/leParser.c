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

typedef struct {
    char *name;
    int current;
    int parent;
    int child[10];
} node;

node a[15];
int NT_cnt = 0;
int T_cnt = 0;
int Active_Parent = 0;

void PrintNode(node nodeptr) {
    printf("========================\n");
    printf("name: %s id: %d\n", nodeptr.name, nodeptr.current);
    if (nodeptr.parent != -1)
        printf("parent: %d\n", nodeptr.parent);
    for (int i = 0; i < 10; i++) {
        if (nodeptr.child[i] != -1) {
            printf("child: %d\n", nodeptr.child[i]);
        }
        else {
            if (i == 0) {
                printf("Node has no Children\n");
                break;
            }
        }
    }
}

void add_node(char *name) {
    int i = 0;
    NT_cnt++;
    a[NT_cnt].current = NT_cnt;
    a[NT_cnt].parent = Active_Parent;
    a[NT_cnt].name = name;
    while (a[a[NT_cnt].parent].child[i] != -1) {
        i++;
    }
    a[a[NT_cnt].parent].child[i] = a[NT_cnt].current;
}

int find_node(char *id) {

    //rework to work from selected node up, so it takes cloesest name !!!!!!!!!!

    for (int i = 0; i < 50; i++) {
        if (strcmp(id, a[i].name) == 0)
            return a[i].current;
    }
}

int main(int argc, char *argv[]) {

    for (int i = 0; i < 15; i++) {
        a->parent = -1;
        for (int j = 0; j < 10; j++) {
            a[i].child[j] = -1;
        }
    }

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
    for (int i = 0; i < 15; i++) {
        PrintNode(a[i]);
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
            add_node("TOK_Identifier");
            return true;
        case TOK_Float_Literal:
            add_node("TOK_Float_Literal");
            return true;
        case TOK_Int_Literal:
            add_node("Int_Literal");
            return true;
        case TOK_String_Literal:
            add_node("TOK_String_Literal");
            return true;
        case TOK_L_Paren:
            add_node("L_Paren");
            return true;

        default:
            return false;
    }
}

// initial non-terminal
bool NT_Prog() {
    bool ret = false;
    a[NT_cnt].name = "Nt_Prog";
    Active_Parent = a[NT_cnt].current;
    ret = NT_Prolog() && NT_Func_Def_List();

    return ret;
}

bool NT_Func_Def_List() {

    bool ret = false;

    Token nextToken = getToken_NL_optional();

    if (nextToken.type == TOK_EOF) {
        add_node("TOK_EOF");
        ret = true;
    }
    else if (nextToken.type == TOK_Func_Keyword) {
        add_node("Tok_Func_Keyword");
        Active_Parent = find_node("Tok_Func_Keyword");
        ret = NT_Func_Def() && NT_Func_Def_List();
    }

    return ret;
}

bool NT_Func_Def() {
    bool ret = false;

    if (getToken().type == TOK_Identifier) {
        add_node("Tok_Identifier");
        if (getToken().type == TOK_L_Paren) {
            add_node("Tok_L_Paren");
            ret = NT_Param_List() && NT_Return_Types() && NT_Stat();
        }
    }

    return ret;
}

bool NT_Param_List() {
    bool ret = false;

    Token nextToken = getToken();

    if (nextToken.type == TOK_R_Paren) {
        add_node("Tok_R_Paren");
        ret = true;
    }
    else if (nextToken.type == TOK_Identifier) {
        add_node("TOK_Identifier");
        ret = NT_Type() && NT_Param_List_N();
    }

    return ret;
}

bool NT_Param_List_N() {
    bool ret = false;

    Token nextToken = getToken();

    if (nextToken.type == TOK_R_Paren) {
        add_node("TOK_R_Paren");
        ret = true;
    }
    else if (nextToken.type == TOK_Comma) {
        add_node("TOK_Comma");
        if (getToken_NL_optional().type == TOK_Identifier)  {
            add_node("TOK_Identifier");
            ret = NT_Type() && NT_Param_List_N();
        }
    }

    return ret;
}

bool NT_Type() {
    bool ret = false;

    Token nextToken = getToken();

    if (nextToken.type == TOK_Int_Keyword) {
        add_node("Tok_Int_Keyword");
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
        add_node("Tok_L_Brace");
        ret = true;
    }
    else if (nextToken.type == TOK_L_Paren) {
        add_node("Tok_L_Paren");
        ret = NT_Type() && NT_Return_Types_N() && getToken().type == TOK_L_Brace;
    }

    return ret;
}

bool NT_Return_Types_N() {
    bool ret = false;

    Token nextToken = getToken();

    if (nextToken.type == TOK_R_Paren) {
        add_node("TOK_R_Paren");
        ret = true;
    }
    else if (nextToken.type == TOK_Comma) {
        add_node("TOK_Comma");
        ret = NT_Type() && NT_Return_Types_N();
    }

    return ret;
}

bool NT_Prolog() {
    bool ret = false;

    if (getToken_NL_optional().type == TOK_Package_Keyword) {
        add_node("TOK_Package_Keyword");
        Token nextToken = getToken();       
        if (nextToken.type == TOK_Identifier && (strcmp(nextToken.str, "main") == 0)) {
            add_node("Tok_main");
            ret = true;
        }
    }

    return ret;
}

bool NT_Stat() {
    bool ret = false;
    add_node("NT_Stat");
    Active_Parent = find_node("NT_Stat");
    Token nextToken = getToken_NL_optional();

    if (nextToken.type == TOK_Identifier) {
        add_node("TOK_Identifier");
        ret = NT_Var() && NT_Stat();
    }
    else if (nextToken.type == TOK_R_Brace) {
        add_node("TOK_R_Brace");
        ret = true;
    }
    else if (nextToken.type == TOK_If_Keyword) {
        add_node("TOK_If_Keyword");
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
        add_node("TOK_Assign");
        ret = NT_Exps();
    }
    else if (nextToken.type == TOK_Define) {
        add_node("TOK_Define");
        ret = NT_Exps();
    }
    else if (nextToken.type == TOK_Comma) {
        add_node("TOK_Comma");
        if (getToken().type == TOK_Identifier) {
            add_node("TOK_Identifier");
            ret = NT_Var();
        }
    }
    else if (nextToken.type == TOK_L_Paren) {
        add_node("TOK_L_Paren");
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
            add_node("TOK_Identifier");
            add_node("TOK_L_Paren");
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
        add_node("TOK_R_Paren");
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
        add_node("TOK_R_Paren");
        ret = true;
    }
    else if (nextToken.type == TOK_Comma) {
        add_node("TOK_Comma");
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
        add_node("TOK_Assign");
        ret = true;
    }
    else if (nextToken.type == TOK_Comma) {
        add_node("TOK_Comma");
        if (getToken().type == TOK_Identifier) {
            add_node("TOK_Identifier");
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
            add_node("TOK_L_Brace");
            if (NT_Stat()) {
                if (getToken().type == TOK_Else_Keyword) {
                    add_node("TOK_Else_Keyword");
                    if (getToken().type == TOK_L_Brace) {
                        add_node("TOK_L_Brace");
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
        add_node("TOK_Semicolon");
        ret = true;
    }
    else if (nextToken.type == TOK_Identifier) {
        add_node("TOK_Identifier");
        ret = NT_For_Def_Var();
    }

    return ret;
}

bool NT_For_Def_Var() {
    bool ret = false;

    Token nextToken = getToken();

    if (nextToken.type == TOK_Comma) {
        add_node("TOK_Comma");
        if (getToken().type == TOK_Identifier) {
            add_node("TOK_Identifier");
            ret = NT_For_Def_Var();
        }
    }
    else if (nextToken.type == TOK_Define) {
        add_node("TOK_Define");
        ret = NT_Exp(EMPTY_TOKEN) && NT_For_Def_Exp_N();
    }

    return ret;
}


bool NT_For_Def_Exp_N() {
    bool ret = false;

    Token nextToken = getToken();

    if (nextToken.type == TOK_Semicolon) {
        add_node("TOK_Semicolon");
        ret = true;
    }
    else if (nextToken.type == TOK_Comma) {
        add_node("TOK_Comma");
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
        add_node("TOK_L_Brace");
        ret = true;
    }
    else if (nextToken.type == TOK_Identifier) {
        add_node("TOK_Identifier");
        ret = NT_For_Assign_Var();
    }

    return ret;
}

bool NT_For_Assign_Var() {
    bool ret = false;

    Token nextToken = getToken();

    if (nextToken.type == TOK_Comma) {
        add_node("TOK_Comma");
        if (getToken().type == TOK_Identifier) {
            add_node("TOK_Identifier");
            ret = NT_For_Assign_Var();
        }
    }
    else if (nextToken.type == TOK_Assign) {
        add_node("TOK_Assign");
        ret = NT_Exp(EMPTY_TOKEN) && NT_For_Assign_Exp_N();
    }

    return ret;
}

bool NT_For_Assign_Exp_N() {
    bool ret = false;

    Token nextToken = getToken();

    if (nextToken.type == TOK_L_Brace) {
        add_node("TOK_L_Brace");
        ret = true;
    }
    else if (nextToken.type == TOK_Comma) {
        add_node("TOK_Comma");
        ret = NT_Exp(EMPTY_TOKEN) && NT_For_Assign_Exp_N();
    }

    return ret;
}

bool NT_Term() {
    bool ret = false;

    Token nextToken = getToken_NL_optional();

    if (nextToken.type == TOK_Identifier) {
        add_node("TOK_Identifier");
        ret = true;
    }
    else if (nextToken.type == TOK_Int_Literal) {
        add_node("TOK_Int_Literal");
        ret = true;
    }
    else if (nextToken.type == TOK_Float_Literal) {
        add_node("TOK_Float_Literal");
        ret = true;
    }
    else if (nextToken.type == TOK_String_Literal) {
        add_node("TOK_String_Literal");
        ret = true;
    }

    return ret;
}
