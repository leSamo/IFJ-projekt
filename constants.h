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
    INTERANAL_ERROR = 99 // interní chyba překladače tj. neovlivněná vstupním programem (např. chyba alokace paměti, atd.)
} errorCode;

/* list of all reserved keywords which cannot be used as identifiers */
const char* keywords[] = {
    "else",
    "float64",
    "for",
    "func",
    "if",
    "int",
    "package",
    "return",
    "string"
};

/* Size of char buffer for scanner */
#define MAX_CHAR_BUFFER_SIZE 50

/* list of all posible automaton states */
typedef enum {
    AS_Default,
    AS_Identif,
    AS_Int,
    AS_Float,
    AS_String,
    AS_String_Escape,
    AS_Comm_Start,
    AS_BlockComm,
    AS_BlockComm_End,
    AS_LineComm
} state;

/* list of all token types */
typedef enum {
    TOK_Int,
    TOK_Float,
    TOK_Identif,
    TOK_String
} tokenType;

/* Represents one token each token has type and some have value of either type int, float or string */
typedef struct {
    tokenType type;
    union {
        int i;
        float f;
        char str[MAX_CHAR_BUFFER_SIZE];
    };
} Token;