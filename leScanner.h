/* ========== leScanner.h ==========
 * Project: IFJ 2020/21 project
 * Team: 067, variant I
 * Author: Samuel Olekšák (xoleks00)
 * Date: November 2020
 * ================================= */

/* reads input from stdin and sends a token */
Token *getToken();

/* print out an error message to stderr */
void throwError(char* msg);

/* returns its argument char back to the input stream, reverts getChar() function */
void ungetChar(char c);

/* returns whether char is digit (0-9) */
bool isDigit(char c);

/* returns whether char is hexadecimal digit (0-9, a-f, A-F) */
bool isHexDigit(char c);

/* returns whether char is letter (a-z, A-Z) */
bool isAlpha(char c);

/* returns whether char is whitespace (space, \n, \t, \v, \f, \r) */
bool isWhiteSpace(char c);

/* converts hex number (represented as two chars) to int corresponding char in ASCII table */
char hexToChar(char a, char b);

/* creates a new int token and inserts it into tokens array */
Token *newIntToken(int content);

/* creates a new float token and inserts it into tokens array */
Token *newFloatToken(float content);

/* decides whether content is keyword or identifier, creates a new token and inserts it into tokens array */
Token *newWordToken(char* content);

/* creates a new token with passed type and value and inserts it into tokens array */
Token *newToken(tokenType type, char* content);

/* creates a new attributeless token with passed type and inserts it into tokens array */
Token *newHalfToken(tokenType type);

/* Creates a new error token signifying lexical error */
Token *newErrorToken();

/* prints all tokens from tokens array */
void printToken(Token *token);

/* debug function to convert token type enum int to string for printing */
char* getTokenName(tokenType type);

/* pushes character at the end of buffer */
void charBufferPush(char* buffer, int *bufferPos, char character);

/* clears buffer and bufferPos and returns its content */
char* charBufferPop(char* buffer, int *bufferPos);

/* debug function to print content of buffer */
void charBufferPrint(char* buffer, int *bufferPos);
