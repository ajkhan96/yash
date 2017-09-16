.DEFAULT_GOAL := yash

yash: yash.c parse.c
	gcc yash.c parse.c -o yash

yash_debug: yash.c parse.c
	gcc -g yash.c parse.c -o yash_debug
