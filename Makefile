CFLAGS = -ansi -Wall -g -O0 -Wwrite-strings -Wshadow -pedantic-errors -fstack-protector-all -Wextra

PROGS = tstSh

all: $(PROGS)

tstSh: testShell.o
	gcc $(CFLAGS) -o tstSh -lreadline testShell.o

testShell.o: testShell.c
	gcc $(CFLAGS) -c testShell.c

clean:
	rm -f *.o *.tmp $(PROGS)
