/* ========= parserMock.h ==========
 * Project: IFJ 2020/21 project
 * Team: 067, variant I
 * Author: Emma Krompaščíková (xkromp00), Samuel Olekšák (xoleks00), Michal Findra (xfindr00)
 * Date: November 2020
 * Description: Syntactic analysis using recursive descent
 * ================================= */

Token *getToken_NL_required();

Token *getToken_NL_optional();

bool NT_Prog();

bool NT_Prolog();

bool NT_Main();

bool NT_Stat();

bool NT_Variable();

bool NT_Assign_N();

bool NT_Define();

bool NT_If_Else();

bool NT_Exp();

bool NT_For_Decl();

bool NT_For_Exp();

bool NT_For_Assign();