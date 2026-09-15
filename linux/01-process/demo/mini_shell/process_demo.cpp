#include "apue.h"
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

int globalvar = 6;
char buf[] = "a write to stdout\n";

int main() {
  int tmp = 10;

  if (write(STDOUT_FILENO, buf, sizeof(buf) - 1) < 0)
    err_sys("write error");
  printf("before fork\n");

  pid_t pid = fork();
  switch (pid) {
  case -1:
    err_sys("fork error");
  case 0:
    
    break;
  default:
    auto tid = wait(NULL);
    int status;
    if(WIFEXITED(status))
    printf("child process exited with tid: %d\n", tid);
    break;
  }

  printf("pid: %d global var: %d\n", getpid(), globalvar);
  return 0;
}