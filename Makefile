all: shell

shell: shell.o
	gcc -o shell shell.o

shell.o: shell.c
	gcc shell.c

clean:
	rm -f shell.o shell 

