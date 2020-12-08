all: 
	gcc -g -c cshell.c cshell.h
	gcc -o cshell cshell.o
	gcc -g test.c -o test

clean:
	rm -f *.o *.a *.h.gch
	rm -f cshell
	rm -f test
	rm -f target.txt