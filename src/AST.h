/* ============= AST.h =============
 * Project: IFJ 2020/21 project
 * Team: 067, variant I
 * Author: Samuel Olekšák (xoleks00), Michal Findra (xfindr00)
 * Date: November 2020
 * Description: N-ary abstract syntax tree for semantic analysis
 * ================================= */

typedef enum ASTNodeType {
    NODE_Prog,
    NODE_Block,
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
    NODE_Type_Int,
    NODE_Type_Float,
    NODE_Type_String,
    NODE_Identifier,
    NODE_Literal_Int,
    NODE_Literal_Float,
    NODE_Literal_String
} ASTNodeType;

#define AST_NODE_CHILDREN 10

typedef struct ASTNode {
    int id;
    ASTNodeType type;
    struct ASTNode *parent;
    struct ASTNode *children[AST_NODE_CHILDREN];
    int childrenCount;
} ASTNode;

ASTNode* AST_CreateNode(ASTNode *parent, ASTNodeType type);

void AST_Print(ASTNode *nodePtr);

char* AST_GetNodeName(ASTNodeType type);

void AST_Delete(ASTNode *nodePtr);
