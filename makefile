meuShell: 	meuShell.o
			gcc meuShell.o -o meuShell

meuShell.o: meuShell.c
			gcc -c -g -Wall -Wextra -pedantic -std=c99 meuShell.c

clean:      
			rm meuShell.o meuShell
