package main

func main() {
	a := 5 >= 0
	a = 8
	print(5, 5.5, "ahoj", a)

	if a != 8 {
		print("pos")
	} else {
		print("neg")
	}
}