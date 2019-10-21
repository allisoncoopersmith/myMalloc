all: first

first: memgrind.c
	gcc -O -g -o memgrind memgrind.c

clean:
	rm -rf first
