#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/unistd.h>
#include <sys/wait.h>

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
    for (int k = 0; k < argc; k++)
      printf("argv[%d] = \"%s\"\n", k, argv[k]);
  }
  // free mem taken by input
  free(input);
  return 0;
}
