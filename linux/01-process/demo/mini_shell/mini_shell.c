// ============================================================
// mini_shell.c — 迷你 Shell
// 综合练习 Q001(进程) / Q002(fork) / Q003(exec)
// ============================================================
// 编译: cmake -B build && cmake --build build
// 运行: ./build/mini_shell
// ============================================================

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_LINE 1024
#define MAX_ARGS 64

// ------------------------------------------------------------
// TODO 1: read_command — 读取一行命令
// 提示:
//   - fgets(buf, size, stdin) 读取一行
//   - 去掉行尾的 '\n'
//   - 遇到 EOF 返回 NULL（用 feof(stdin) 判断），main 据此退出
// ------------------------------------------------------------
char *read_command(char *buf, int size) {
  // TODO: 在这里实现
  return NULL;
}

// ------------------------------------------------------------
// TODO 2: parse_command — 把一行命令拆成 argv 数组
// 提示:
//   - strtok(buf, " \t\n") 按空白字符切分
//   - argv 必须以 NULL 结尾
//   - 返回实际参数个数（argv[0] 是命令名本身）
// ------------------------------------------------------------
int parse_command(char *buf, char **argv, int max_args) {
  // TODO: 在这里实现
  return 0;
}

// ------------------------------------------------------------
// TODO 3: run_command — fork + exec + wait 核心（Q002 + Q003）
// 提示:
//   1. fork() 创建子进程
//   2. 子进程 (pid == 0):
//      - execvp(argv[0], argv)   ← p 变体，自动按 PATH 搜索 (Q003)
//      - exec 失败: perror() 打印错误后 _exit(127)
//   3. 父进程 (pid > 0):
//      - waitpid(pid, &status, 0) 等待回收子进程 (Q004 预告)
//      - WEXITSTATUS(status) 取出退出码并打印
//   4. fork 失败 (pid < 0): perror 后 return
// ------------------------------------------------------------
void run_command(char **argv) {
  // TODO: 在这里实现
}

// ------------------------------------------------------------
// TODO 4: shell 主循环
// 提示:
//   - 打印提示符 "$ "，记得 fflush(stdout) 刷新
//   - read_command 返回 NULL(EOF/Ctrl-D) 或输入 "exit" → 退出
//   - parse_command 解析后若参数为 0 则跳过（空行）
//   - 调用 run_command 执行
// ------------------------------------------------------------
int main(void) {
  char line[MAX_LINE];
  char *argv[MAX_ARGS];

  while (1) {
    // TODO: 打印提示符并刷新
    // TODO: 读取命令（EOF 则退出）
    // TODO: 解析命令（空行跳过）
    // TODO: 执行命令
  }
  return 0;
}
