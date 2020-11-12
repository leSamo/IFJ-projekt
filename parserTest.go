/* Program 1, should compile OK */
/*
package main

func main() {
    a := 5
    b := 6
}
*/
/* Program 2, doesn't have EOF at the end, should produce syntactic error */
/*
package main

func main() {
    a := 5
}

bla bla bla
*/

/* Program 3, has if-else, should compile OK */
/*
package main

func main() {
    if some_exp {
        a := 5
    } else {
        b := 6
        c := 7
    }
}
*/

/* Program 4, testing differrent assignment types, should compile OK */

package main

func main() {
    a = 5
}
