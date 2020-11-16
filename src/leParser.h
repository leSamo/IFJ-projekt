/* ========= parserMock.h ==========
 * Project: IFJ 2020/21 project
 * Team: 067, variant I
 * Author: Emma Krompaščíková (xkromp00), Samuel Olekšák (xoleks00), Michal Findra (xfindr00)
 * Date: November 2020
 * Description: Syntactic analysis using recursive descent
 * ================================= */

/* Returns new non-newline token, but requires at least one newline token prior */
Token getToken_NL_required();

/* Returns new non-newline token, ignores newline tokens prior */
Token getToken_NL_optional();

/* Returns true if supplied tokenType could be first token of expression */
bool isExpFirst(tokenType type);

/* NON-TERMINALS */
bool NT_Prog();

bool NT_Prolog();

bool NT_Main();

bool NT_Stat();

bool NT_Variable();

bool NT_Assign_N();

bool NT_Define();

bool NT_If_Else();

bool NT_Exp(Token overlapTokenIn);

bool NT_For_Decl();

bool NT_For_Exp();

bool NT_For_Assign();