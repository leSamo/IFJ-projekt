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
    NODE_Literal_String,
    NODE_Add,
    NODE_Sub,
    NODE_Mul,
    NODE_Div,
    NODE_Less_Then,
    NODE_More_Then,
    NODE_Less_Equal_Then,
    NODE_More_Equal_Then,
    NODE_Equal,
    NODE_Not_Equal,
    NODE_Multi_L_Value
} ASTNodeType;

typedef enum typeTag {
    TAG_None,
    TAG_Int,
    TAG_Float,
    TAG_String
} typeTag;

typedef union typeUnion {
    int64_t i;
    double f;
    char *str;
} typeUnion;

#define AST_NODE_CHILDREN 50

typedef struct ASTNode {
    int id;
    ASTNodeType type;

    struct ASTNode *parent;
    struct ASTNode *children[AST_NODE_CHILDREN];
    int childrenCount;

    bool isOperatorResult;
    typeTag operatorType;

    typeTag contentType;
    typeUnion content;
} ASTNode;

ASTNode* AST_CreateNodeGeneral(ASTNode *parent, ASTNodeType type, typeTag contentType, typeUnion content);

void AST_AttachNode(ASTNode *parent, ASTNode* child);

ASTNode* AST_GetChildOfType(ASTNode *parent, ASTNodeType type);

void AST_PrettyPrint(ASTNode *nodePtr, int level);

void AST_Print(ASTNode *nodePtr);

char* AST_GetNodeName(ASTNodeType type);

void AST_Delete(ASTNode *nodePtr);
