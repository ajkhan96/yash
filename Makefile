.DEFAULT_GOAL := yash

yash: yash.c parse.c job.c
	gcc yash.c parse.c job.c -o yash

yash_debug: yash.c parse.c job.c
	gcc -g yash.c parse.c job.c -o yash_debug
