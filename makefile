newshell: newshell.c
	gcc -o newshell newshell.c -I.
clean:
	rm newshell
