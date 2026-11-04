#!/usr/bin/env bash
# 新机器自检：确认能编译、能跑测试、能离线复用 gtest
# 用法: ./scripts/bootstrap.sh
set -uo pipefail
cd "$(dirname "${BASH_SOURCE[0]}")/.." || exit 1

OS=$(uname -s)
bold() { printf '\n\033[1m%s\033[0m\n' "$*"; }
ok()   { printf '  \033[32m✓\033[0m %s\n' "$*"; }
bad()  { printf '  \033[31m✗\033[0m %s\n' "$*"; MISSING=$((MISSING + 1)); }
MISSING=0

install_hint() {
  case "$OS" in
    Darwin) printf '      brew install %s\n' "$1" ;;
    Linux)  printf '      sudo apt install -y %s\n' "$1" ;;
    *)      printf '      请手动安装 %s\n' "$1" ;;
  esac
}

bold "1. 必备工具"
for bin in git cmake c++ make; do
  if command -v "$bin" >/dev/null 2>&1; then
    ver=$("$bin" --version 2>/dev/null | head -1)
    ok "$bin  (${ver:0:60})"
  else
    bad "缺少 $bin"; install_hint "$bin"
  fi
done

bold "2. 版本要求"
if command -v cmake >/dev/null 2>&1; then
  cmake_ver=$(cmake --version | head -1 | awk '{print $3}')
  major=$(printf '%s' "$cmake_ver" | cut -d. -f1)
  minor=$(printf '%s' "$cmake_ver" | cut -d. -f2)
  if [ "$major" -gt 3 ] || { [ "$major" -eq 3 ] && [ "$minor" -ge 14 ]; }; then
    ok "cmake $cmake_ver ≥ 3.14"
  else
    bad "cmake $cmake_ver < 3.14（algorithms 专题要求）"
  fi
fi
printf '  \033[32m✓\033[0m C++17 检查: '
printf 'int main(){auto p=std::make_unique<int>(1);return *p-1;}\n' > /tmp/_bootstrap_check.cpp
printf '#include <memory>\n' | cat - /tmp/_bootstrap_check.cpp > /tmp/_bootstrap_check2.cpp
if c++ -std=c++17 -Wall -Wextra /tmp/_bootstrap_check2.cpp -o /tmp/_bootstrap_check 2>/dev/null; then
  ok "c++ -std=c++17 编译通过"
else
  bad "C++17 编译失败，请检查编译器"
fi
rm -f /tmp/_bootstrap_check*.cpp /tmp/_bootstrap_check

bold "3. 可选工具（缺失不影响，只影响体验）"
for bin in python3 gdb lldb perf strace valgrind heaptrack; do
  if command -v "$bin" >/dev/null 2>&1; then ok "$bin"; else printf '  \033[33m-\033[0m %s（可选）\n' "$bin"; fi
done

if [ "$OS" = "Darwin" ]; then
  printf '  \033[33m!\033[0m perf / strace / bpftrace 仅 Linux 可用；阶段 D 需要一台 Linux（
'
  printf '      本地容器、OrbStack、或远端开发机均可）\n'
fi

bold "4. gtest 离线复用"
gt=$(ls -d algorithms/src/01-array-string/build/_deps/googletest-src 2>/dev/null || true)
if [ -n "$gt" ]; then
  ok "已存在本地 gtest 源码：$gt"
else
  printf '  \033[33m!\033[0m 无本地 gtest 缓存 —— 首次 configure 需要联网下载 v1.14.0\n'
  printf '      离线机器可先拷贝该目录，再用:\n'
  printf '      cmake -S . -B build -DFETCHCONTENT_SOURCE_DIR_GOOGLETEST=<path>/googletest-src\n'
fi

bold "5. 仓库状态"
command -v git >/dev/null 2>&1 && git log --oneline -1 | sed 's/^/  最近提交: /'
printf '  未提交改动: %s 项\n' "$(git status --porcelain 2>/dev/null | wc -l | tr -d ' ')"

bold ""
if [ "$MISSING" -eq 0 ]; then
  printf '\033[32m工具链就绪\033[0m —— 接下来运行 ./scripts/status.sh\n'
else
  printf '\033[31m%d 项必备工具缺失\033[0m\n' "$MISSING"
fi
exit "$MISSING"
