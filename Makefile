all:
	gcc -o gdndbg gdndbg.c namespace.c

clean:
	rm -f gdndbg
