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
/*
package main

func main() {
    a = 5
    b, c = 5, 5
    d,e,f=0,0,5
}
*/

/* Program 5, testing for loop, should compile OK */
/*
package main

func main() {
    for b := 6 ; 6 ; a = 5 {
        a = 5
    }
    
    for a := 5; (1+3) * 6 ; {
        a = 4
    }

    for ; 5; {
        b = 5
    }

    for ; 12 + 3; a,b = 1,2 {

    }
}
*/
/* Program 6, expression testing, should compile OK */

package main

func main() {
    a = 5.6 + 6.8
    b := 4. * (1+.8)
}
