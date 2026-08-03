# mini_shell — 迷你 Shell 🐚

综合练习 **Q001(进程) / Q002(fork) / Q003(exec)** 的 demo 任务。

## 构建与运行

```bash
# 配置 + 构建
cmake -B build -S .
cmake --build build

# 运行
./build/mini_shell

# 清理
rm -rf build
```

## 练习目标（4 个 TODO）

| # | 函数 | 核心知识 | 对应 Issue |
|:-:|:-----|:--------|:----------|
| 1 | `read_command` | 行读取与 EOF 处理 | Q001 |
| 2 | `parse_command` | 字符串切分 → argv | Q001 |
| 3 | `run_command` | `fork + execvp + waitpid` | Q002 + Q003 |
| 4 | `main` | 主循环串联 | 综合 |

## 验收标准

- [ ] 能运行 `ls` `pwd` `echo` 等外部命令（PATH 搜索）
- [ ] 每条命令在独立子进程中执行，shell 不被替换
- [ ] 命令结束后打印子进程 PID 与退出码
- [ ] 输入 `exit` 或 Ctrl-D 退出
- [ ] 命令不存在时打印错误且 shell 不崩溃
- [ ] （进阶）支持 `cd` 内建命令
