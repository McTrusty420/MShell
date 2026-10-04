#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
  char *input = NULL;
  // cap required for getlin() to track dynamic alloc with realloc
  size_t cap = 0;
  ssize_t len = 1;

  // getline requires somewhere to put data, something to store the capacity
  // to dynamically realloc mem, and a FILE/Stream to take input from
  while (1) {
    printf("$ ");
    len = getline(&input, &cap, stdin);
    if (len == -1)
      break;

    // stripping the input of any \n
    if (len > 0 && input[len - 1] == '\n') {
      input[len - 1] = '\0';
    }
    // flushing out any remaining $, since it doesn't terminate with \n the  $
    // remains in the buffer (C string goof)
    fflush(stdout);
    // to mark if we're currently working on a word or a space or tab
    int in_word = 0;
    // buffer to split words
    char *argv[64];
    // index to buffer argv
    int argc = 0;

    // loop to change spaces or tabs to EOL
    for (size_t i = 0; input[i] != '\0'; i++) {
      char c = input[i];
      // if space or tab, replace with EOL
      if (c == ' ' || c == '\t') {
        input[i] = '\0';
        in_word = 0;
      }
      // if we are in a word, we store the address of the beggining to that word
      // in argv then post-increment argc
      else if (!in_word) {
        argv[argc++] = &input[i];
        in_word = 1;
        if (argc >= 63)
          break;
      }
    }
    // set the last part of argv to NULL
    argv[argc] = NULL;

    // builtin handling for cd, since cd won't work on a fork/child
    // the child will change dir but that won't affect the parent
    if (strcmp(argv[0], "cd") == 0) {
      if (argv[1] == NULL || strcmp(argv[1], "~")) {
        const char *home = getenv("HOME");
        if (home == NULL) {
          fprintf(stderr, "no home set\n");
        }
        chdir(home);
        // fprintf(stderr, "no arguments\n");
      } else if (chdir(argv[1]) != 0) {
        fprintf(stderr, "%s %s\n", argv[1], strerror(errno));
      } else if (strcmp(argv[1], "..")) {
        chdir();
      }
    }
    // everything other than cd
    else {

      // fork() clones the entire program, ie this program :) uses same amount
      // of resources
      pid_t pid = fork();

      // pid 0 means the fork was successful and the child got the id 0, so
      // we're running execvp only for the child otherwise we get the double
      // execution bug;
      if (pid == 0) {
        // execvp is a wrapper for execve and requires the command - argv[0] -
        // so that the program knows what it's gonna load and the command array
        // - argv
        // - so that the executed program, for example - ls, knows what
        // arguments it has, which starts from argv[1];
        if (execvp(argv[0], argv) == -1) {
          fprintf(stderr, "%s %s\n", argv[0], strerror(errno));
          _exit(127);
        }
        // If pid isn't 0 we just wait
      } else if (pid > 0) {
        int status;
        // waits till pid dies and fills exit info to status, 0 means no special
        // options (3rd argument is for options)
        waitpid(pid, &status, 0);

        // if the fork  fails
      } else {
        fprintf(stderr, "fork failed\n");
      }
    }
  }
  // free mem taken by input
  free(input);
  return 0;
}
