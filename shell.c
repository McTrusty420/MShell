#include "execute.h"
#include "tokenizer.h"
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
  ssize_t nread = -1;

  // setting status code
  int last_status = 0;

  // for tracking prevDir
  char *pwd = getcwd(NULL, 0);

  char *target = NULL;

  // getline requires somewhere to put data, something to store the capacity
  // to dynamically realloc mem, and a FILE/Stream to take input from
  while (1) {
    printf("$ ");
    nread = getline(&input, &cap, stdin);
    if (nread == -1)
      break;

    // stripping the input of any \n
    if (nread > 0 && input[nread - 1] == '\n') {
      input[nread - 1] = '\0';
    }
    // flushing out any remaining $, since it doesn't terminate with \n the  $
    // remains in the buffer (C string goof)
    fflush(stdout);
    // to mark if we're currently working on a word or a space or tab
    int in_word = 0;
    // buffer to split words
    char *argv[64];

    int argc = tokenizer(input, argv, 64);

    for (int i = 0; i < argc; i++) {
      fprintf(stdout, "argv:%s\n", argv[i]);
    }

    if (argc == 0) {
      continue;
    }

    // builtin handling for cd, since cd won't work on a fork/child
    // the child will change dir but that won't affect the parent
    if (strcmp(argv[0], "cd") == 0) {
      if (argv[1] == NULL || strcmp(argv[1], "~") == 0) {
        target = getenv("HOME");
        if (target == NULL) {
          fprintf(stderr, "no home set\n");
          continue;
        }
      } else if (strcmp(argv[1], "-") == 0) {
        target = pwd;
      } else {
        target = argv[1];
      }

      // for tracking new pwd
      char *newPwd = getcwd(NULL, 0);

      // if target is not found, newPwd is freed
      if (chdir(target) != 0) {
        fprintf(stderr, "%s not found\n", argv[1]);
        free(newPwd);
        last_status = 1;
      } else { // if it is found, old pwd is updated to newPwd
        free(pwd);
        pwd = newPwd;
      }
    }
    // everything other than cd
    else {
      last_status = execute(argv);
    }
    fprintf(stdout, "%d\n", last_status);
  }
  // free mem taken by input
  free(input);
  free(pwd);
  return 0;
}
