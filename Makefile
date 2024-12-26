all: parse_md calc_beutify todo_beutify

parse_md: parse_md.c
	gcc -g -o parse_md parse_md.c

parse_routines: parse_routines.c
	gcc -g -o parse_routines parse_routines.c

calc_beutify: calc_beutify.c
	gcc -g -o calc_beutify calc_beutify.c

todo_beutify: todo_beutify.c
	gcc -g -o todo_beutify todo_beutify.c
