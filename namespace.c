#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/utsname.h>
#include <sys/wait.h>
#include <unistd.h>
#define errExit(msg)                                                           \
  do {                                                                         \
    perror(msg);                                                               \
    exit(EXIT_FAILURE);                                                        \
  } while (0)

static int /* Start function for cloned child */
childFunc(void *arg) {
  struct utsname uts;

  /* Change hostname in UTS namespace of child */

  if (sethostname(arg, strlen(arg)) == -1)
    errExit("sethostname");

  /* Retrieve and display hostname */

  if (uname(&uts) == -1)
    errExit("uname");
  printf("uts.nodename in child:  %s\n", uts.nodename);

  /* Keep the namespace open for a while, by sleeping.
     This allows some experimentation--for example, another
     process might join the namespace. */

  sleep(500);

  return 0; /* Child terminates now */
}

#define STACK_SIZE (1024 * 1024) /* Stack size for cloned child */

int main(int argc, char *argv[]) {
  char *stack;    /* Start of stack buffer */
  char *stackTop; /* End of stack buffer */
  pid_t pid;
  struct utsname uts;

  if (argc < 2) {
    fprintf(stderr, "Usage: %s <child-hostname>\n", argv[0]);
    exit(EXIT_SUCCESS);
  }

  /* Allocate memory to be used for the stack of the child */

  stack = mmap(NULL, STACK_SIZE, PROT_READ | PROT_WRITE,
               MAP_PRIVATE | MAP_ANONYMOUS | MAP_STACK, -1, 0);
  if (stack == MAP_FAILED)
    errExit("mmap");

  stackTop = stack + STACK_SIZE; /* Assume stack grows downward */

  /* Create child that has its own UTS namespace;
     child commences execution in childFunc() */

  pid = clone(childFunc, stackTop, CLONE_NEWNS | SIGCHLD, argv[1]);
  if (pid == -1)
    errExit("clone");
  printf("clone() returned %ld\n", (long)pid);

  /* Parent falls through to here */

  sleep(1); /* Give child time to change its hostname */

  /* Display hostname in parent's UTS namespace. This will be
     different from hostname in child's UTS namespace. */

  if (uname(&uts) == -1)
    errExit("uname");
  printf("uts.nodename in parent: %s\n", uts.nodename);

  if (waitpid(pid, NULL, 0) == -1) /* Wait for child */
    errExit("waitpid");
  printf("child has terminated\n");
  return 0;
  exit(EXIT_SUCCESS);
}