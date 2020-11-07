/* ========== constants.h ==========
/* Project: IFJ 2020/21 project
/* Team: 067, variant I
/* Author: Samuel Olekšák (xoleks00)
/* Date: November 2020
/* Description: Various structures, declarations and constants shared by multiple components
/* ================================= */

// return codes for various error scenarios
typedef enum {
    LEXICAL_ERROR = 1, // chyba v programu v rámci lexikální analýzy (chybná struktura aktuálního lexému)
    SYNTAX_ERROR = 2, // chyba v programu v rámci syntaktické analýzy (chybná syntaxe programu, neočekávané odřádkování)
    DEFINITION_ERROR = 3, // sémantická chyba v programu – nedefinovaná funkce/proměnná, pokus o redefinici funkce/proměnné, atp.
    DEFINITION_TYPE_ERROR = 4, // sémantická chyba při odvozování datového typu nově definované proměnné
    INCOMPATIBLE_TYPE_ERROR = 5, // sémantická chyba typové kompatibility v aritmetických, řetězcových a relačních výrazech
    ARGUMENT_ERROR = 6, // sémantická chyba v programu – špatný počet/typ parametrů či návratových hodnot u volání funkce či návratu z funkce
    OTHER_SEMANTIC_ERROR = 7, // ostatní sémantické chyby
    ZERO_DIVISION_ERROR = 9, // sémantická chyba dělení nulovou konstantou
    INTERNAL_ERROR = 99 // interní chyba překladače tj. neovlivněná vstupním programem (např. chyba alokace paměti, atd.)
} errorCode;

/* size of char buffer for scanner */
#define MAX_CHAR_BUFFER_SIZE 50

/* list of all posible automaton states */
typedef enum {
    AS_Default,
    AS_Word,
    AS_Int,
    AS_Float,
    AS_Float_Scientific_Start,
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

/* list of all token types */
typedef enum {
    TOK_Error, // signifies lexical error
    TOK_Identifier,

    /* assignment operators */
    TOK_Assign,
    TOK_Define,

    /* literals */
    TOK_Int_Literal,
    TOK_Float_Literal,
    TOK_String_Literal,

    /* arithmetic operators */
    TOK_Add,
    TOK_Sub,
    TOK_Mul,
    TOK_Div,

    /* relational operators */
    TOK_Less_Then,
    TOK_More_Then,
    TOK_Less_Equal_Then,
    TOK_More_Equal_Then,
    TOK_Equal,
    TOK_Not_Equal,

    /* keyword tokens */
    TOK_Else_Keyword,
    TOK_Float_Keyword,
    TOK_For_Keyword,
    TOK_Func_Keyword,
    TOK_If_Keyword,
    TOK_Int_Keyword,
    TOK_Package_Keyword,
    TOK_Return_Keyword,
    TOK_String_Keyword,

    /* brackets */
    TOK_L_Paren,
    TOK_R_Paren,
    TOK_L_Brace,
    TOK_R_Brace,

    /* separators */
    TOK_Newline,
    TOK_Comma,
    TOK_Semicolon
} tokenType;

/* represents one token each token has type and some have value of either type int, float or string */
typedef struct {
    tokenType type;
    union {
        int i;
        float f;
        char str[MAX_CHAR_BUFFER_SIZE];
    };
} Token;

typedef struct {
    char* word;
    tokenType type;
} keyword;

/* associative array of all reserved keywords which cannot be used as identifiers */
const keyword keywords[] = {
    (keyword){"else", TOK_Else_Keyword},
    (keyword){"float64", TOK_Float_Keyword},
    (keyword){"for", TOK_For_Keyword},
    (keyword){"func", TOK_Func_Keyword},
    (keyword){"if", TOK_If_Keyword},
    (keyword){"int", TOK_Int_Keyword},
    (keyword){"package", TOK_Package_Keyword},
    (keyword){"return", TOK_Return_Keyword},
    (keyword){"string", TOK_String_Keyword},
};

/* size of keywords array */
#define KEYWORDS_ARRAY_SIZE 9
