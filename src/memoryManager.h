/* ======== memoryManager.h ========
 * Project: IFJ 2020/21 project
 * Team: 067, variant I
 * Author: Samuel Olekšák (xoleks00)
 * Date: November, December 2020
 * Description: Allocating and disposing of various data structures
 * ================================= */

#include "stringBuffer.h"

/* Allocates a new char array of length + 1 and remembers its reference to dispose of it later*/
char* newString(unsigned int length);

/* Deallocates scanner buffer and all strings allocated with newString function */
void deallocateAll();
