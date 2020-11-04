/* Reads input from stdin and sends a token */
Token *getToken();

/* print out an error message to stderr */
void throwError(char* msg);

/* Returns its argument char back to the input stream, reverts getChar() function */
void ungetChar(char c);

/* returns whether char is digit (0-9) */
bool isDigit(char c);

/* returns whether char is letter (a-z, A-Z) */
bool isAlpha(char c);

/* returns whether char is whitespace (space, \n, \t, \v, \f, \r) */
bool isWhiteSpace(char c);

/* creates a new int token and inserts it into tokens array */
Token *newIntToken(int content);

/* creates a new float token and inserts it into tokens array */
Token *newFloatToken(float content);

/* creates a new token with passed type and value and inserts it into tokens array */
Token *newToken(tokenType type, char* content);

/* creates a new attributeless token with passed type and inserts it into tokens array */
Token *newHalfToken(tokenType type);

/* prints all tokens from tokens array */
void printToken(Token *token);

/* Debug function to convert token type enum int to string for printing */
char* getTokenName(tokenType type);

/* Pushes character at the end of buffer */
void charBufferPush(char* buffer, int *bufferPos, char character);

/* Clears buffer and bufferPos and returns its content */
char* charBufferClear(char* buffer, int *bufferPos);

/* Debug function to print content of buffer */
void charBufferPrint(char* buffer, int *bufferPos);
