all: parse_md calc_beautify todo_beautify

parse_md: parse_md.c
	gcc -g -o parse_md parse_md.c

parse_routines: parse_routines.c
	gcc -g -o parse_routines parse_routines.c

calc_beautify: calc_beautify.c
	gcc -g -o calc_beautify calc_beautify.c

todo_beautify: todo_beautify.c
	gcc -g -o todo_beautify todo_beautify.c
