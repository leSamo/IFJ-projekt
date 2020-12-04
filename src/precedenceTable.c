/* ======== precedenceTable.c ========
 * Project: IFJ 2020/21 project
 * Team: 067, variant I
 * Author: 
 * Date: November 2020
 * ================================= */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    GreaterSign = 1, // >
    LessSign = 2,    // <
    EqualSign = 3,   // =
    Empty = -1
} PAT_Elemenet;

typedef enum {
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

PAT_Elemenet PA_Table[7][7] = {
    {GreaterSign, LessSign, LessSign, GreaterSign, LessSign, GreaterSign, GreaterSign},
    {GreaterSign, GreaterSign, LessSign, GreaterSign, LessSign, GreaterSign, GreaterSign},
    {LessSign, LessSign, LessSign, Equal, LessSign, LessSign, Empty},
    {GreaterSign, GreaterSign, Empty, GreaterSign, Empty, GreaterSign, GreaterSign},
    {GreaterSign, GreaterSign, Empty, GreaterSign, Empty, GreaterSign, GreaterSign},
    {LessSign, LessSign, LessSign, GreaterSign, LessSign, Empty, GreaterSign},
    {LessSign, LessSign, LessSign, Empty, LessSign, LessSign, Empty}};

int PAT_GetSign(PAT_Header Row, PAT_Header Column) {
    return PA_Table[Row][Column];
}