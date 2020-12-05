/* ========= expressions.h =========
 * Project: IFJ 2020/21 project
 * Team: 067, variant I
 * Author: Emma Krompaščíková (xkromp00), Samuel Olekšák (xoleks00)
 * Date: November 2020
 * Description: Expression syntactic analysis with postfix conversion using shunting-yard algorithm
 * ================================= */

/* AST Node array used to link expression into AS tree */
typedef struct {
    ASTNode** nodes;
    int count;
} NodeBuffer;

/* From tokens array in infix notation creates postfix notation */
bool handleExpression(ASTNode *expRoot, Token overlapTokenIn);

/* Verify if output queue syntax is correct */
ASTNode* verifyOutput(TokenBuffer *outputQueue);

/* If possible collapse 3 nodes inside buffer on pos, pos-1 and pos-2 into one node with 2 children */
bool NodeBufferCollapse(NodeBuffer *buffer, int pos);

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
