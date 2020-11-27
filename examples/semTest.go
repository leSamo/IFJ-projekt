package main

func main() {
	a := 5
	a = 6

	if a > 5 {
		a := 6.6
		a = 5.3
	} else {
		a := "ahoj"
		a = "cau"
	}
}
/*
func main() {

	b := 4
	a := b + 5
	c := a + b + b
	d := 5.5
	e := 5.8
	f := d + e
	a = 5
	b = a + 4

	a,b = 5,6
	s := "ahoj"
	s,a = inputs()
	a,b = inputi()
	d,a = inputf()

	a = ahoj(5, 6)
	a,b = multi(a)

	noRet(5)
}

func noRet(a int) {

}

func multi(a int) (int, int) {

}

func ahoj(a int, b int) (int) {
	i := 0
	f := 0.0
	s := "ahoj"
	f = int2float(i)
	i = float2int(f)
	i = len(s)
	s, i = substr(s, 1, 1)
	i, i = ord("ahoj", 1)
	s, i = chr("sss", 0)
	print(i,f,s)
}


func cau() (int, int) {
	return a, 45
}
*/