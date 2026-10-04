shell: shell.c
	gcc shell.c -o shell
run: shell.c shell
	./shell
debug: shell.c 
	gcc -Wall -Wextra -g shell.c -o shell
clean:
	rm shell
