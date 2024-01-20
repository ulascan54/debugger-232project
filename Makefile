all: link run check

link:
	gcc processor.c -o output -lncurses
run:
	./output

check: check-link check-run

check-link: 
	gcc expanded.c -o check

check-run:

	./check
