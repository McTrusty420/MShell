#ifndef TOKENIZER_H_INCLUDED
#define TOKENIZER_H_INCLUDED
#include <stddef.h>

// the tokenizer splits input into words, delimiter is whitespace (space and
// tab) fills argv with the tokens and
// argv_max is the max size of argv which can be anything specified and it
// doesn't go past that. then it write '\0' at the ending of every token it's
// updated in place, ie no separate storage for the modified string, it's
// updated into char *input itself
//
// requirements:
// input has to be a writeable buffer
// token pointers are valid only until input is modified again (they point
// into input)
//
// by design:
// argv[0 to argc-1]: contains all the tokens
// argv[argc]: NULL, required for execvp() to work
// if input is empty, argv[0] = NULL, there will be nothing else in argv
// argc: the number of tokens in argv excluding NULL at the end

int tokenizer(char *input, char *argv[], size_t argv_max);

#endif // !TOKENIZER_H_INCLUDED
