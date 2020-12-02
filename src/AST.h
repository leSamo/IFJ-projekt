/* ============= AST.h =============
 * Project: IFJ 2020/21 project
 * Team: 067, variant I
 * Author: Samuel Olekšák (xoleks00), Michal Findra (xfindr00)
 * Date: November 2020
 * Description: N-ary abstract syntax tree for semantic analysis
 * ================================= */

/* type of structure AST node represents */
typedef enum ASTNodeType {
    NODE_Empty,
    NODE_Prog,
    NODE_Block,
    NODE_Func_Def,
    NODE_Func_Def_Param_List,
    NODE_Func_Def_Param,
    NODE_Func_Def_Params_Variable,
    NODE_Func_Def_Return,
    NODE_Stat,
    NODE_If_Else,
    NODE_For,
    NODE_Func_Call,
    NODE_Return,
    NODE_Assign,
    NODE_Define,
    NODE_Exp, // do not handle with recursive descent, pass it the expression parser
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

/* primitive type of node - int, float64, string, unknown (for identifiers), any (for special _ variable) */
typedef enum typeTag {
    TAG_None,
    TAG_Unknown,
    TAG_Int,
    TAG_Float,
    TAG_String,
    TAG_Unused,
    TAG_Any
} typeTag;

/* union to hold attribute data of node */
typedef union typeUnion {
    int64_t i;
    double f;
    char *str;
} typeUnion;

/* TODO: Rework to inflatable buffer */
#define AST_NODE_CHILDREN 50

typedef struct ASTNode {
    int id; // used only in block nodes to identify scope
    ASTNodeType type;

    struct ASTNode *parent;
    struct ASTNode *children[AST_NODE_CHILDREN];
    int childrenCount;

    bool isOperatorResult; // true if node was created 
    typeTag valueType;

    typeTag contentType;
    typeUnion content;
} ASTNode;

/* Create a new AST node and attach it to supplied parent node */
ASTNode* AST_CreateNodeGeneral(ASTNode *parent, ASTNodeType type, typeTag contentType, typeUnion content);

/* Attaches child into first free child slot of the parent node */
void AST_AttachNode(ASTNode *parent, ASTNode* child);

/* Finds first direct child node of desired type */
ASTNode* AST_GetChildOfType(ASTNode *parent, ASTNodeType type);

/* Finds first (direct or indirect) child node of desired type */
ASTNode* AST_GetDescendantOfType(ASTNode *parent, ASTNodeType type);

/* Finds closest (direct or indirect) parent node of desired type */
ASTNode* AST_GetParentOfType(ASTNode *child, ASTNodeType type);

/* [DEBUG] Recursively prints out AST tree, level should be 0 in initial call */
void AST_PrettyPrint(ASTNode *nodePtr, int level);

/* [DEBUG] Converts ASTNodeType to string for printing */
char* AST_GetNodeName(ASTNodeType type);

/* Recursively delete tree and dispose of its memory */
void AST_Delete(ASTNode *nodePtr);
