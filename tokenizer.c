#include "tokenizer.h"
#include <stdio.h>
#include <unistd.h>

int tokenizer(char *input, char *argv[], size_t argv_max) {
  // to mark if we're currently working on a word or a space or tab
  int in_word = 0;

  // index to buffer argv
  int argc = 0;

  int end_mark = 1;

  // loop to change spaces or tabs to EOL
  // the tokenizer
  for (size_t i = 0; input[i] != '\0'; i++) {
    char c = input[i];
    // if space or tab, replace with EOL
    if (c == ' ' || c == '\t' || c == '\n' || c == ';') {
      input[i] = '\0';
      if (c == ';')
        argv[argc++] = ";";
      // end_mark = 0;
      in_word = 0;
    }
    // if we are in a word, we store the address of the beggining to that word
    // in argv then post-increment argc
    else if (!in_word) {
      printf("start of in word\n");
      argv[argc++] = &input[i];
      printf("%s argc=%d\n", argv[argc - 1], argc - 1);
      in_word = 1;
      // if (!end_mark) {
      //   printf("start of end mark\n");
      //   argv[argc++] = ";";
      //   printf("%s argc=%d\n", argv[argc - 1], argc - 1);
      // }
      if (argc >= argv_max)
        break;
    }
  }

  // set the last part of argv to NULL
  argv[argc] = NULL;
  return argc;
}
