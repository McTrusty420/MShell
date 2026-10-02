shell: shell.c
	gcc shell.c -o shell
run: shell.c shell
	./shell
clean:
	rm shell
