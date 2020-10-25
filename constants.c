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

// list of all reserved keywords which cannot be used as identifiers
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
