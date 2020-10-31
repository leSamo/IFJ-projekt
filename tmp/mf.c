#include <stdio.h>
#include <stdlib.h>

enum AS
{
    AS_WhiteSign,
    AS_BlockComm,
    AS_LineComm,
    AS_Comm,
    AS_Token
};

int main(int argc, char *argv[])
{
    int currentChar, lastChar;
    char token[20][20];
    int letter = 0;
    int word = 0;
    int AutomatState;

    // loop through every char in file
    while ((currentChar = getchar()) != EOF)
    {
        if ((currentChar == ' ' || currentChar == '\t' || currentChar == '\n') &&
            AutomatState != AS_Comm && AutomatState != AS_LineComm && AutomatState != AS_BlockComm)
        {
            AutomatState = AS_WhiteSign;
        }
        else if (currentChar == '/')
        {
            AutomatState = AS_Comm;
        }
        else if ((currentChar >= 'a' && currentChar <= 'z') ||
                 (currentChar >= 'A' && currentChar <= 'Z') ||
                 (currentChar >= '0' && currentChar <= '9') &&
                     AutomatState != AS_Comm &&
                     AutomatState != AS_LineComm &&
                     AutomatState != AS_BlockComm)
        {
            AutomatState = AS_Token;
            if ((lastChar == ' ' || lastChar == '\t' || lastChar == '\n') && letter != 0)
            {
                word++;
                letter = 0;
            }
        }

        switch (AutomatState)
        {
        case AS_WhiteSign:

            break;

        case AS_Comm:
            printf("ahoj\n");
            if (currentChar == '/' && lastChar == '/')
                AutomatState = AS_LineComm;
            break;

        case AS_LineComm:
            printf("ahojda");
            break;

        case AS_BlockComm:
            break;

        case AS_Token:
            token[word][letter] = currentChar;
            letter++;
            break;

        deafult:
            break;
        }
        lastChar = currentChar;
    }

    printf("%s\n", token[0]);
    printf("%s\n", token[1]);
    printf("%s\n", token[2]);
    printf("%s\n", token[3]);

    return 0;
}
/*

        currentChar = getchar();

        switch (currentChar)
        {
        case EOF:
            break;
        case ' ':
        case '\t':
        case '\n':
            break;
        // COMMENT
        case '/':
            currentChar = getchar();
            // line comment
            if (currentChar == '/')
            {
                do
                {
                    currentChar = getchar();
                } while (currentChar != '\n');
            }
            // block comment
            else if (currentChar == '*')
            {
                int prevChar;
                do
                {
                    prevChar = currentChar;
                    currentChar = getchar();
                    //printf("%d, %d\n", prevChar, currentChar);

                } while ((currentChar != '/') || (prevChar != '*'));
            }
            // TODO: variable starting with /
            break;
        // TOKENS
        default:
            do
            {
                token[word][letter] = currentChar;
                letter++;
                currentChar = getchar();
            } while (currentChar != ' ' && currentChar != '\t');
            word++;
            letter = 0;
            break;
        }

    } while (currentChar != EOF);

    printf("%s\n", token[0]);
    printf("%s\n", token[1]);
    printf("%s\n", token[2]);
    printf("%s\n", token[3]);

    return 0;
}*/