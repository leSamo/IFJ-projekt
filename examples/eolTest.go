package main /* Incomplete hint whether EOL are forbidden/required/optional: */

func add(i int, /* EOL optional */
	j int /* EOL forbidden */) (int) /* EOL forbidden */ { /* EOL required */
		return i + j
}

func main() /* EOL forbidden */ {
	i := 0
	i = add( /* EOL optional*/
		1 /* EOL forbidden */, /* EOL optional*/
		1 /* EOL forbidden */)
	print(i, "\n")
	i = /* EOL optional */
		1 /* EOL forbidden */ + ( /* EOL optional */
		1) /* EOL forbidden */ * /* EOL optional */
			(2) /* EOL required */
	if i > /* EOL optional */
		0 /* EOL forbidden */ { /* EOL required */
			print(i)
	} else /* EOL forbidden */ { /* EOL required */
	} /* EOL required */
}