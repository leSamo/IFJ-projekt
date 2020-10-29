
#include <stdio.h>
#include <stdlib.h>

int main (int argc, char *argv[]) {  
    char sign;
    int white_space[3] = {32,10,9}; // "9", "10" breaks condition - check
    char token[20][20];
    int letter = 0;
    int word = 0;

    sign = getchar();
      // remove whitespaces from beginnning
    while (sign == white_space[0] || sign == white_space[1]){
        sign = getchar();   
    }   
    do { 
        // START state

        if (sign == 47){
            sign = getchar();
                if (sign == 47){
                    //line comment
                    while (sign != 10)
                        sign = getchar();                   
                } else if (sign == 42){
                    // block comment
                    Label: 
                    while (sign != 42){
                        sign = getchar();
                    }
                    sign = getchar();
                    if (sign != 47) goto Label;  // !!! needs further checking to prevent segfault                  
                }           
        } else {
            if (sign != white_space[0]){   // chained condition breaks parsing ((sign != white_space[0]) || (sign != white_space[1]) || (sign != white_space[2]) )
            token[word][letter]=sign;
            letter++;
            } else {           
                word++;   
                letter = 0;         
            }       
            sign = getchar();  
        }

    } while (sign != EOF);


    printf("%s\n", token[0]);
    printf("%s\n", token[1]);
    

    return 0;
}