#include "tokenizer.h"
#include <stdio.h>
#include <unistd.h>

int tokenizer(char *input, char *argv[], size_t argv_max) {
  // to mark if we're currently working on a word or a space or tab
  int in_word = 0;

  // index to buffer argv
  int argc = 0;

  // loop to change spaces or tabs to EOL
  // the tokenizer
  for (size_t i = 0; input[i] != '\0'; i++) {
    char c = input[i];
    // if space or tab, replace with EOL
    if (c == ' ' || c == '\t' || c == '\n' || c == ';') {
      input[i] = '\0';
      // checking if we encounter a ; and adding ; to argv if yes
      if (c == ';' && argc >= argv_max) {
        argv[argc++] = ";";
      }
      // end_mark = 0;
      in_word = 0;
    }
    // if we are in a word, we store the address of the beggining to that word
    // in argv then post-increment argc
    else if (!in_word) {
      argv[argc++] = &input[i];
      in_word = 1;
      if (argc >= argv_max)
        break;
    }
  }

  // set the last part of argv to NULL
  argv[argc] = NULL;
  return argc;
}
