myshell.x: myshell.c
	gcc myshell.c -std=c99 -o myshell.x
clear:
	rm -f myshell.x
	rm -f myshell
	rm -f myshell.o

