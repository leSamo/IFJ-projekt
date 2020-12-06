/* ========= expressions.h =========
 * Project: IFJ 2020/21 project
 * Team: 067, variant I
 * Author: Emma Krompaščíková (xkromp00), Samuel Olekšák (xoleks00), Michal Findra (xfindr00)
 * Date: November 2020
 * Description: Expression syntactic analysis with postfix conversion using shunting-yard algorithm
 * ================================= */

/* AST Node array used to link expression into AS tree */
typedef struct {
    ASTNode** nodes;
    int count;
} NodeBuffer;

/* Elements of operator precedence table */
typedef enum PAT_Element {
    SHIFTT = 1, // <
    REDUCE = 2, // >
    EQUALL = 3, // =
    ERRORR = -1
} PAT_Element;

/* Elements of operator precedence table header */
typedef enum PAT_Header {
    Plus = 0,
    Minus = 0,
    Multiply = 1,
    Divide = 1,
    LeftBracket = 2,
    RightBracket = 3,
    Identifier = 4,
    GreaterThan = 5,
    LessThan = 5,
    GreaterEqualThan = 5,
    LessEqualThan = 5,
    Equal = 5,
    NotEqual = 5,
    EOLT = 6
} PAT_Header;

PAT_Element PA_Table[7][7] = {
    /* +-      * /      (       )      id      rel      $  */
    {REDUCE, SHIFTT, SHIFTT, REDUCE, SHIFTT, REDUCE, REDUCE},  // +-
    {REDUCE, REDUCE, SHIFTT, REDUCE, SHIFTT, REDUCE, REDUCE},  // /*
    {SHIFTT, SHIFTT, SHIFTT, EQUALL, SHIFTT, SHIFTT, ERRORR},  // (
    {REDUCE, REDUCE, ERRORR, REDUCE, ERRORR, REDUCE, REDUCE},  // )
    {REDUCE, REDUCE, ERRORR, REDUCE, ERRORR, REDUCE, REDUCE},  // id
    {SHIFTT, SHIFTT, SHIFTT, REDUCE, SHIFTT, ERRORR, REDUCE},  // rel
    {SHIFTT, SHIFTT, SHIFTT, ERRORR, SHIFTT, SHIFTT, ERRORR}}; // $

/* Retruns PAT header from given Token type */
PAT_Header PAT_GetHeaderFromToken(Token token);

/* Retruns element from PAT */
PAT_Element PAT_GetSign(PAT_Header Row, PAT_Header Column);

/* Fill imput stack with token and check operators */
Token FillInputStack(TokenBuffer *InStack, Token overlapIn);

/* From tokens array in infix notation creates postfix notation */
bool handleExpression(ASTNode *expRoot, Token overlapTokenIn, Token *overlapTokenOut);

/* Returns priority for that token (1,2,3) */
int getPriority(tokenType type);

/* Returns true if token type is valid for expression */
bool isValidExpToken(tokenType type);

/* Returnds true if AST node type is an operator (+,-,*,/,<,<=,>,>=,==,!=), similar to isOperator but with AST node type */
bool isNodeOperator(ASTNodeType type);

/* Returns true if token type is operator (+,-,*,/,<,<=,>,>=,==,!=) */
bool isOperator(tokenType type);

/* Returns true if node type is relation operator (<,<=,>,>=,==,!=) */
bool isNodeRelationalOperator(ASTNodeType type);

/* Returns true if token can be legally followed by newline symbol (+,-,*,/,(,<,<=,>,>=,==,!=) */
bool canHaveNewlineAfter(tokenType type);
