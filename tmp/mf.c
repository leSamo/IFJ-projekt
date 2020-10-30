#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    int currentChar;
    int whiteSpace[3] = {' ', '\t', '\n'};
    char token[20][20];
    int letter = 0;
    int word = 0;

    // remove whitespaces from beginnning
    while (currentChar == whiteSpace[0] || currentChar == whiteSpace[1])
    {
        currentChar = getchar();
    }

    // loop through every char in file
    do
    {
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
}