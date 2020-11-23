/* ========== leParser.h ===========
 * Project: IFJ 2020/21 project
 * Team: 067, variant I
 * Author: Emma Krompaščíková (xkromp00), Samuel Olekšák (xoleks00), Michal Findra (xfindr00)
 * Date: November 2020
 * Description: Syntactic analysis using recursive descent
 * ================================= */

#include "AST.c"

/* Returns new non-newline token, but requires at least one newline token prior */
Token getToken_NL_required();

/* Returns new non-newline token, ignores newline tokens prior */
Token getToken_NL_optional();

/* Returns true if supplied tokenType could be first token of expression */
bool isExpFirst(tokenType type);

/* NON-TERMINALS */
bool NT_Prog();

bool NT_Func_Def_List(ASTNode *parentNode);

bool NT_Func_Def(ASTNode *parentNode);

bool NT_Param_List(ASTNode *parentNode);

bool NT_Param_List_N(ASTNode *parentNode);

bool NT_Type(ASTNode *parentNode);

bool NT_Return_Types(ASTNode *parentNode);

bool NT_Return_Types_N(ASTNode *parentNode);

bool NT_Prolog(); // no semantic value

bool NT_Stat(ASTNode *parentNode);

bool NT_Var(ASTNode *parentNode, Token identifierToken);

bool NT_Var_N(ASTNode *parentNode, Token identifierToken);

bool NT_Exps(ASTNode *parentNode);

bool NT_Func_Args(ASTNode *parentNode);

bool NT_Func_Args_N(ASTNode *parentNode);

bool NT_Exps_N(ASTNode *parentNode);

bool NT_Assign_N(ASTNode *parentNode);

bool NT_If_Else(ASTNode *parentNode);

bool NT_Exp(ASTNode *parentNode, Token overlapTokenIn);

bool NT_For_Def(ASTNode *parentNode);

bool NT_For_Def_Var(ASTNode *parentNode);

bool NT_For_Def_Exp_N(ASTNode *parentNode);

bool NT_For_Exp(ASTNode *parentNode);

bool NT_For_Assign(ASTNode *parentNode);

bool NT_For_Assign_Var(ASTNode *parentNode);

bool NT_For_Assign_Exp_N(ASTNode *parentNode);

bool NT_Term(ASTNode *parentNode);
