#include "tokenizer.h"
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

      // fork() clones the entire program, ie this program :) uses same amount
      // of resources
      pid_t pid = fork();

      // pid 0 means the fork was successful and the child got the id 0, so
      // we're running execvp only for the child otherwise we get the double
      // execution bug;
      if (pid == 0) {
        // execvp is a wrapper for execve and requires the command - argv[0] -
        // so that the program knows what it's gonna load and the command
        // array
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
        // waits till pid dies and fills exit info to status, 0 means no
        // special options (3rd argument is for options)
        waitpid(pid, &status, 0); // status stores a bitfield that contains info
                                  // on how the child exited and the exit code
        if (WIFEXITED(status)) { // WIFEXITED is wait if exited, tells us if the
                                 // child exited normally
          last_status = WEXITSTATUS(status); // WEXITSTATUS is wait exit status,
                                             // tells us the exit code
        }

        // if the fork  fails
      } else {
        fprintf(stderr, "fork failed\n");
      }
    }
    fprintf(stdout, "%d\n", last_status);
  }
  // free mem taken by input
  free(input);
  free(pwd);
  return 0;
}
