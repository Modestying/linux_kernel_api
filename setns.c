#define _GNU_SOURCE
#include <fcntl.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/utsname.h>
#include <unistd.h>

#define errExit(msg)                                                           \
  do {                                                                         \
    perror(msg);                                                               \
    exit(EXIT_FAILURE);                                                        \
  } while (0)

int main(int argc, char *argv[]) {
  int fd;

  if (argc < 3) {
    fprintf(stderr, "%s /proc/PID/ns/FILE cmd args...\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  fd = open(argv[1], O_RDONLY); /* Get file descriptor for namespace */
  if (fd == -1)
    errExit("open");

  if (setns(fd, 0) == -1) /* Join that namespace */
    errExit("setns");

  execvp(argv[2], &argv[2]); /* Execute a command in namespace */
  struct utsname uts;

  if (uname(&uts) == -1)
    errExit("uname");
  printf("uts.nodename in child:  %s\n", uts.nodename);
  errExit("execvp");
}