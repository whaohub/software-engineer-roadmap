# Q003: exec 原理

## 1. exec 系统调用概述

**exec** 是 UNIX/Linux 中"运行新程序"的核心机制。调用成功后，当前进程的**代码段、数据段、堆、栈全部被新程序的映像替换**，进程从头开始执行新程序的入口（`main`）。

```c
#include <unistd.h>

// 只有 execve 是真正的系统调用，其余五个是 libc 封装
int execve(const char *pathname, char *const argv[], char *const envp[]);
```

exec 有两个违反直觉的特点：

| 特点 | 说明 |
|------|------|
| **成功不返回** | 进程映像已被替换，旧代码不复存在，自然无法"返回" |
| **失败返回 -1** | 只有出错时（如文件不存在）才返回，并设置 `errno` |

```c
printf("before exec\n");
if (execve("/bin/ls", argv, envp) == -1) {
    perror("execve");          // 走到这里说明 exec 失败了
}
printf("after exec\n");        // 只有 exec 失败才会执行到这行
```

> 一句话总结：**exec 不是"启动一个新进程"，而是"把当前进程改头换面成另一个程序"，PID 始终不变。**

---

## 2. exec 系列函数（六个变体）

### 2.1 函数签名一览

| 函数 | 路径/PATH | 参数形式 | 环境变量 | 说明 |
|:-----|:--------:|:--------:|:--------:|:-----|
| `execl` | 路径 | 列表 `...` | 继承 | l = list |
| `execv` | 路径 | 数组 `argv[]` | 继承 | v = vector |
| `execle` | 路径 | 列表 `...` | **自定义 envp** | l + e |
| `execve` | 路径 | 数组 `argv[]` | **自定义 envp** | 真正的系统调用 |
| `execlp` | **PATH 搜索** | 列表 `...` | 继承 | l + p |
| `execvp` | **PATH 搜索** | 数组 `argv[]` | 继承 | v + p |

### 2.2 记忆方法

```
exec ─┬─ p → 按 PATH 环境变量搜索可执行文件
      ├─ l → 参数以"列表"形式逐个传入，末尾以 NULL 结束
      ├─ v → 参数以"数组"形式传入（char *argv[]）
      └─ e → 可以自定义环境变量（char *envp[]）

组合：execlp = p + l，execle = l + e，execvp = p + v
```

```c
#include <unistd.h>

// execl：指定路径 + 参数列表（参数必须从 argv[0] 开始，以 NULL 结尾）
execl("/bin/ls", "ls", "-l", "/tmp", NULL);

// execlp：只给文件名，自动到 PATH 中搜索
execlp("ls", "ls", "-l", "/tmp", NULL);

// execv：参数数组
char *argv[] = {"ls", "-l", "/tmp", NULL};
execv("/bin/ls", argv);

// execvp：文件名 + PATH 搜索 + 参数数组
execvp("ls", argv);

// execle：自定义环境变量
char *envp[] = {"PATH=/usr/bin", "HOME=/root", NULL};
execle("/bin/ls", "ls", "-l", "/tmp", NULL, envp);

// execve：系统调用版，参数数组 + 环境变量
execve("/bin/ls", argv, envp);
```

### 2.3 细节注意

1. `l` 系列最后一个参数必须是 **NULL**（`(char *)0`），作为参数列表的结束标记
2. `p` 系列若文件名含 `/`，则当作路径直接使用；否则按 `PATH` 逐目录查找
3. `e` 系列传入自定义环境变量后，**新程序看不到原环境变量**（完全替换，不是追加）

> 一句话总结：**六兄弟本质是 execve 的语法糖，`p` 管搜索路径、`l/v` 管参数形态、`e` 管环境变量。**

---

## 3. 进程映像替换原理

### 3.1 替换过程

exec 将新程序加载进当前进程的地址空间，替换原有内容：

```
exec 之前                        exec 之后
┌──────────────────┐           ┌──────────────────┐
│  栈  Stack        │           │  新程序的栈       │
│  堆  Heap         │  ────▶   │  新程序的堆       │
│  BSS 段           │  替换     │  新程序的 BSS     │
│  数据段 Data      │           │  新程序的数据段   │
│  代码段 Text      │           │  新程序的代码段   │
└──────────────────┘           └──────────────────┘
         │                              │
         └──── 同一进程，PID 不变 ───────┘
```

### 3.2 什么变了 / 什么没变

| 类别 | 是否保留 | 说明 |
|:-----|:--------:|:-----|
| **PID / PPID** | ✅ 保留 | 进程身份不变，还是同一个进程 |
| **打开的文件描述符** | ✅ 默认保留 | 除非设置了 `FD_CLOEXEC`（见 §3.3） |
| **当前工作目录** | ✅ 保留 | `chdir` 的设置不丢失 |
| **umask、资源限制** | ✅ 保留 | 进程级属性不受影响 |
| **已捕获的信号处理** | ❌ 重置 | 被 `signal` 捕获的信号恢复为默认动作 |
| **忽略的信号（SIG_IGN）** | ✅ 保留 | 忽略状态继续有效 |
| **代码段/数据段/堆/栈** | ❌ 全部替换 | 新程序的映像取代旧内容 |
| **全局/静态变量** | ❌ 重置 | 变量被重新初始化，旧值全部丢失 |

### 3.3 FD_CLOEXEC — 控制 fd 是否随 exec 关闭

默认 exec 后 fd 依然打开，这可能导致**新程序意外继承**（如泄漏文件、泄露安全信息）。关闭方式：

```c
#include <fcntl.h>

int fd = open("data.txt", O_RDONLY | O_CLOEXEC);  // 推荐：创建时即标记

// 或对已有 fd 追加标记
fcntl(fd, F_SETFD, FD_CLOEXEC);                   // exec 时自动关闭
```

> 一句话总结：**exec 换的是"程序映像"，不换"进程身份"；fd 默认跟着走，想让它走就标记 CLOEXEC。**

---

## 4. exec 失败的原因

exec 只有失败才返回，常见 `errno`：

| errno | 原因 | 场景 |
|:-----:|:-----|:-----|
| **ENOENT** | 文件不存在 | 路径写错，或 `p` 系列在 PATH 中找不到 |
| **EACCES** | 权限不足 | 文件无执行权限（缺 `x`）或目录不可访问 |
| **ENOEXEC** | 不是可执行格式 | 对脚本/文本文件直接 exec（无 shebang 时） |
| **E2BIG** | 参数/环境变量过多 | argv/envp 超过 `ARG_MAX` 限制 |
| **ENOMEM** | 内存不足 | 无法为新映像分配内存 |

```c
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

if (execvp("non_exist_cmd", argv) == -1) {
    printf("exec 失败: %s\n", strerror(errno));   // No such file or directory
}
```

---

## 5. exec 与 fork 的配合

### 5.1 经典模式：fork + exec + wait

exec 替换的是"当前进程"，所以**想运行新程序而又保留父进程**，必须先 fork：

```
        fork()                  exec()               wait()
父进程 ──────▶ 创建子进程 ──────── 等待 ────────────────▶ 回收子进程
                 │                │                     │
子进程 ──────────┘ ──▶ 进入子进程分支 ─▶ 映像替换 ────────┘
                        (pid==0)       运行新程序        (退出)
```

```c
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(1);
    } else if (pid == 0) {
        // 子进程：exec 替换映像，运行新程序
        execl("/bin/ls", "ls", "-l", NULL);
        perror("exec");        // exec 失败才走到这里
        exit(127);
    }
    // 父进程：等待子进程结束
    int status;
    waitpid(pid, &status, 0);
    printf("子进程已结束\n");
    return 0;
}
```

> 这就是 **shell 执行外部命令**（如 `ls`、`grep`）的原理：fork 出子进程，子进程 exec，父进程 wait。

### 5.2 为什么 fork 后立即 exec？（结合 Q002 的 COW）

- fork 用**写时复制**：父子共享只读物理页
- 子进程紧接着 exec：映像被整体替换，**之前的物理页几乎不会被写**
- 结果：**fork + exec 模式几乎零拷贝**，只有页表复制成本

```c
// 错误示范：fork 后还在子进程里做大量初始化再 exec，浪费 COW 优势
if (pid == 0) {
    // ... 大量写操作，触发大量缺页复制 ...
    execvp("ls", argv);   // 之前的工作全部白费（映像被替换）
}
```

### 5.3 exec 失败处理

exec 失败时子进程不会自动退出，会继续执行 exec 之后的代码。**务必在 exec 后立即检查并退出**，否则可能父子进程同时执行后续代码：

```c
if (pid == 0) {
    execvp(cmd, argv);
    perror("exec");       // 失败：打印错误
    _exit(127);           // 立即退出，避免继续执行父进程逻辑
}
```

---

## 6. 常见面试陷阱

### 6.1 exec 成功"不返回"——死代码问题

```c
execl("/bin/ls", "ls", NULL);
printf("永远不会打印（除非 exec 失败）\n");
```

exec 成功后映像已替换，`printf` 所在代码已不存在。所以 exec 之后的代码**只可能是错误处理分支**。

### 6.2 exec 后 PID 不变，为什么？

exec 只是替换进程的**地址空间内容**，`task_struct`（PCB）里的 PID、PPID、打开文件表、信号掩码等**进程属性原样保留**。可以理解为"同一具身体换了新的灵魂"。

### 6.3 fd 继承导致的泄漏

```c
int fd = open("secret.txt", O_RDONLY);  // 未设 CLOEXEC
execl("/bin/cat", "cat", "/etc/passwd", NULL);
// 新程序 cat 意外继承了 fd，可读取 secret.txt
```

解决：创建时加 `O_CLOEXEC`，或 `fcntl(fd, F_SETFD, FD_CLOEXEC)`。

### 6.4 已捕获信号被重置

```c
signal(SIGINT, my_handler);   // 捕获
execl("/bin/ls", "ls", NULL);
// exec 后 my_handler 的地址在新映像中无意义
// SIGINT 的处理被恢复为默认行为（终止进程）
```

POSIX 规定：exec 后**被捕获的信号恢复默认动作**，但 **SIG_IGN 忽略的信号继续忽略**。

### 6.5 exec 不能执行 shell 内置命令

```c
execlp("cd", "cd", "/tmp", NULL);   // 失败！cd 是 shell 内置命令，不是可执行文件
```

shell 在执行命令前会先判断是否为内置命令（`cd`、`export`、`echo` 等），内置命令直接在当前 shell 进程执行，不 fork 不 exec。

### 6.6 用 shell 执行带通配符/重定向的命令

exec 不解析通配符、管道、重定向，这些是 **shell 的语法**。若要执行完整命令，交给 `/bin/sh -c`：

```c
execl("/bin/sh", "sh", "-c", "ls *.c | grep main", NULL);
```

---

## 7. 面试要点总结

1. **成功不返回，失败返回 -1**：exec 之后只写错误处理代码
2. **六个变体**：`p`=PATH 搜索、`l`=参数列表、`v`=参数数组、`e`=自定义环境变量；`execve` 是底层系统调用
3. **换映像不换进程**：PID、父进程、fd、cwd、umask 保留；代码段/数据段/堆/栈被替换
4. **fd 默认保留**：要 exec 时关闭，用 `O_CLOEXEC` / `FD_CLOEXEC`
5. **信号处理**：已捕获的信号重置为默认，SIG_IGN 保持忽略
6. **fork + exec + wait**：shell 执行外部命令的标准流程，配合 COW 近乎零拷贝
7. **exec 不解析 shell 语法**：通配符、重定向、管道需交给 `/bin/sh -c`

---

## 参考
- 《UNIX 环境高级编程》(APUE) 第 8 章
- man 3: execl, execv, execle, execve, execlp, execvp
- 前置知识：[Q002 fork 流程](./Q002_fork流程.md)
