#include <stdio.h>
#include <stdlib.h>

typedef enum
{
    AS_WhiteSign,
    AS_BlockComm,
    AS_LineComm,
    AS_Comm,
    AS_Token,
    AS_Empty
} AS;

//check if is int "a" is letter in ASCII
int isLetter(int a)
{
    if ((a >= 'a' && a <= 'z') || (a >= 'A' && a <= 'Z'))
    {
        return 0;
    }
    return -1;
}

//check if is int "a" is number in ASCII
int isNumber(int a)
{
    if (a >= '0' && a <= '9')
    {
        return 0;
    }
    return -1;
}

//check if AutomateState is Comment
int CommentActive(AS AutomatState)
{
    if (AutomatState == AS_Comm || AutomatState == AS_LineComm || AutomatState == AS_BlockComm)
    {
        return 1;
    }
    return 0;
}

int main(int argc, char *argv[])
{
    int currentChar, lastChar;
    char token[20][20];
    int letter = 0;
    int word = 0;
    AS AutomatState = AS_Empty;

    // loop through every char in file
    while ((currentChar = getchar()) != EOF)
    {
        if ((currentChar == ' ' || currentChar == '\t' || currentChar == '\n') && !CommentActive(AutomatState))
        {
            AutomatState = AS_WhiteSign;
        }
        else if (currentChar == '/' && !CommentActive(AutomatState))
        {
            AutomatState = AS_Comm;
        }
        else if ((isLetter(currentChar) || isNumber(currentChar)) && !CommentActive(AutomatState))
        {
            AutomatState = AS_Token;
        }

        switch (AutomatState)
        {
        case AS_WhiteSign:

            break;

        case AS_Comm:
            if (currentChar == '/' && lastChar == '/')
                AutomatState = AS_LineComm;
            else if (currentChar == '*' && lastChar == '/')
                AutomatState = AS_BlockComm;
            break;

        case AS_LineComm:
            if (currentChar == '\n')
                AutomatState = AS_Empty;
            break;

        case AS_BlockComm:
            if (currentChar == '/' && lastChar == '*')
                AutomatState = AS_Empty;
            break;

        case AS_Token:
            if ((lastChar == ' ' || lastChar == '\t' || lastChar == '\n') && letter != 0)
            {
                word++;
                letter = 0;
            }
            token[word][letter] = currentChar;
            letter++;
            break;

        deafult:
            break;
        }
        lastChar = currentChar;
    }

    // just to check
    for (int i = 0; i < 5; i++)
    {
        printf("%s\n", token[i]);
    }

    return 0;
}