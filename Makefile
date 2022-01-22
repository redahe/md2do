all: parse_md calc_beutify

parse_md: parse_md.c
	gcc -g -o parse_md parse_md.c

calc_beutify: calc_beutify.c
	gcc -o calc_beutify calc_beutify.c
