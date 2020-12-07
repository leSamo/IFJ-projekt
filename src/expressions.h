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
    Plus,
    Multiply,
    LeftBracket,
    RightBracket,
    Identifier,
    GreaterThan,
    EOLT
} PAT_Header;

/* Operator precedence table*/
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

/* Fills imput stack with token and check operators */
Token FillInputStack(TokenBuffer *InStack, Token overlapIn);

/* Returns true if token type is valid for expression */
bool isValidExpToken(tokenType type);

/* Returns true if AST node type is an operator (+,-,*,/,<,<=,>,>=,==,!=), similar to isOperator but with AST node type */
bool isNodeOperator(ASTNodeType type);

/* Returns true if token type is operator (+,-,*,/,<,<=,>,>=,==,!=) */
bool isOperator(tokenType type);

/* Creates operation node and stroe it to node arr*/
void ExpCreateOperationNode(ASTNodeType nodeType, Token firstTok, Token secondTok, ASTNode **nodes);

/* Creates leaf node and store it to node arr*/
void ExpCreateIdentifNode(ASTNodeType nodeType, Token firstTok, ASTNode **nodes);

/* Applies Reduction rule to top elements on Stack*/
bool Reduce(TokenBuffer *Stack, Token currentToken, ASTNode **nodes);

/* Inserts Shift token after top terminal in stack and insert current token to top of the stack*/
void InsertShiftAfterTopTerminal(TokenBuffer *Stack, Token currentToken);

/* Returns top terminal on stack */
Token GetTopTerminal(TokenBuffer *Stack);

/* Creates nodes from tokens array */
bool handleExpression(ASTNode *expRoot, Token overlapTokenIn, Token *overlapTokenOut);

/*Attach expression node to parent tree*/
void attachASTToRoot(ASTNode *expRoot, ASTNode **nodes);
