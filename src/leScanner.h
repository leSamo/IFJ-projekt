/* ========== leScanner.h ==========
 * Project: IFJ 2020/21 project
 * Team: 067, variant I
 * Author: Samuel Olekšák (xoleks00), Emma Krompaščíková (xkromp00)
 * Date: November, December 2020
 * ================================= */

#ifndef SCANNER
#define SCANNER

/* list of all posible automaton states */
typedef enum {
    AS_Default,
    AS_Word,
    AS_Leading_Zero,
    AS_Int,
    AS_Float_Start,
    AS_Float,
    AS_Float_Scientific_Start,
    AS_Float_Scientific_Start_2,
    AS_Float_Scientific,
    AS_String,
    AS_String_Escape,
    AS_String_Escape_Hex_1,
    AS_String_Escape_Hex_2,
    AS_Comm_Start,
    AS_BlockComm,
    AS_BlockComm_End,
    AS_LineComm,
    AS_Colon,
    AS_Equal,
    AS_Less_Then,
    AS_More_Then,
    AS_Exclamation
} state;

/* reads input from stdin and sends a token */
Token getToken();

/* print out an error message to stderr */
void throwError(int errorCode, char* msg, bool printLine);

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
Token newIntToken(int64_t content);

/* creates a new float token and inserts it into tokens array */
Token newFloatToken(float content);

/* decides whether content is keyword or identifier, creates a new token and inserts it into tokens array */
Token newWordToken(char* content);

/* creates a new string token and inserts it into tokens array */
Token newStringToken(char* content);

/* creates a new attributeless token with passed type and inserts it into tokens array */
Token newHalfToken(tokenType type);

/* [DEBUG] prints a token in readable form */
void printToken(Token token);

/* [DEBUG] convert token type enum int to string for printing */
char* getTokenName(tokenType type);

#endif
