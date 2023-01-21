all: cshell

cshell:	cshell.c cshell.h
	gcc -o cshell cshell.c

clean:
	rm cshell
