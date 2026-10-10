shell: shell.c tokenizer.c execute.c
	gcc shell.c tokenizer.c execute.c -o shell
run: shell
	./shell
debug: shell.c tokenizer.c execute.c
	gcc -Wall -Wextra -g shell.c execute.c -o shell
clean:
	rm shell
