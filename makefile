shell: shell.c tokenizer.c
	gcc shell.c tokenizer.c -o shell
run: shell
	./shell
debug: shell.c tokenizer.c
	gcc -Wall -Wextra -g shell.c -o shell
clean:
	rm shell
