#include "apue.h"
#include <stdio.h>
#include <unistd.h>

int globalvar = 6;
char buf[] = "a write to stdout\n";

int main() {
  pid_t pid = fork();
  int tmp = 10;

  if (write(STDOUT_FILENO, buf, sizeof(buf) - 1) < 0)
    err_sys("write error");
  printf("before fork\n");

  switch (pid) {
  case -1:
    err_sys("fork error");
  case 0:
    globalvar++;
    printf("This is the child process pid: %d tmp: %d addr: %p\n", getpid(),
           tmp += 10, (void *)&tmp);
    break;
  default:
    printf("This is the parent process pid: %d tmp: %d addr: %p\n", getpid(),
           tmp += 20, (void *)&tmp);
    sleep(2);
    break;
  }

  printf("pid: %d global var: %d\n", getpid(), globalvar);
  return 0;
}