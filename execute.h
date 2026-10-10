#ifndef EXECUTE_H_INCLUDED
#define EXECUTE_H_INCLUDED

// executes a given command and nothing else
// forks, execvp's, waits and returns exit status
//
// requirements:
// argv must be NULL terminated because that's required by execvp (already taken
// care of in tokenizer) argv[0] must not be NULL, caller checks empty before
// calling
//
// by design:
// does not modify argv or anything it points to returns the executed command's
// exit status (through waitpid)
//
// returns exit status or 127 if execvp fails

int execute(char *argv[]);

#endif
