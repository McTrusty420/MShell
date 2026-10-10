#include "execute.h"
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int execute(char *argv[]) {

  int last_status = 0;

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
    if (WIFEXITED(status)) {  // WIFEXITED is wait if exited, tells us if the
                              // child exited normally
      last_status = WEXITSTATUS(status); // WEXITSTATUS is wait exit status,
                                         // tells us the exit code
    }

    // if the fork  fails
  } else {
    fprintf(stderr, "fork failed\n");
  }
  return last_status;
}
