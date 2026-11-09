#!/usr/bin/env bash
# M1 的 Linux 侧验收：把"零系统调用 / 零堆分配 / 内存只增长一次"变成可观测的数字
#
# 用法:
#   ./scripts/linux-probe.sh [可执行文件] [--max-rss-mb 64]
#   默认: build/buffer_pool_test
#
# 退出码: 0 = 全部通过；1 = 有项不达标或环境不支持
set -uo pipefail
cd "$(dirname "${BASH_SOURCE[0]}")/.." || exit 1

BIN="build/buffer_pool_test"
MAX_RSS_MB=64
while [ $# -gt 0 ]; do
  case "$1" in
    --max-rss-mb) MAX_RSS_MB="${2:-64}"; shift 2 ;;
    -h|--help) sed -n '2,10p' "$0" | sed 's/^# \{0,1\}//'; exit 0 ;;
    *) BIN="$1"; shift ;;
  esac
done

bold() { printf '\n\033[1m%s\033[0m\n' "$*"; }
ok()   { printf '  \033[32m✓\033[0m %s\n' "$*"; }
bad()  { printf '  \033[31m✗\033[0m %s\n' "$*"; FAIL=$((FAIL + 1)); }
warn() { printf '  \033[33m!\033[0m %s\n' "$*"; }
FAIL=0

OS=$(uname -s)
if [ ! -x "$BIN" ]; then
  echo "找不到可执行文件: $BIN" >&2
  echo "先构建: make test DIR=project/mini-pipeline" >&2
  exit 1
fi

if [ "$OS" != "Linux" ]; then
  bold "当前系统: $OS —— 这些验证只在 Linux 上有效"
  warn "macOS 没有 strace / perf / pmap（dtruss 需 sudo 且不通用）"
  echo
  echo "  用容器跑（在仓库根目录）:"
  echo "    docker run --rm -it -v \"\$PWD\":/w -w /w gcc:13 bash -lc '\\"
  echo "      apt-get update -qq && apt-get install -y -qq strace time && \\"
  echo "      cmake -S project/mini-pipeline -B /tmp/b -DFETCHCONTENT_SOURCE_DIR_GOOGLETEST=/w/algorithms/src/01-array-string/build/_deps/googletest-src && \\"
  echo "      cmake --build /tmp/b -j4 && ./project/mini-pipeline/scripts/linux-probe.sh /tmp/b/buffer_pool_test'"
  echo
  echo "  或者用 OrbStack/远端 Linux 机器，把仓库同步过去再跑本脚本。"
  exit 1
fi

# ---------------------------------------------------------------- 1. 内存上限
bold "1. 最大常驻内存（RSS）"
rss_line=$(/usr/bin/time -v "$BIN" 2>&1 >/dev/null | grep -i "Maximum resident set size" || true)
rss_kb=$(printf '%s' "$rss_line" | awk '{print $NF}')
if [ -n "${rss_kb:-}" ]; then
  rss_mb=$((rss_kb / 1024))
  if [ "$rss_mb" -le "$MAX_RSS_MB" ]; then
    ok "最大 RSS = ${rss_mb} MB（阈值 ${MAX_RSS_MB} MB）"
  else
    bad "最大 RSS = ${rss_mb} MB，超过阈值 ${MAX_RSS_MB} MB —— 检查是否在热路径上分配"
  fi
else
  warn "拿不到 RSS（缺 /usr/bin/time？apt install time）"
fi

# ---------------------------------------------------------------- 2. 系统调用
bold "2. 启动后是否还有 brk/mmap（热路径零 syscall）"
if command -v strace >/dev/null 2>&1; then
  strace -c -f -e trace=brk,mmap,munmap "$BIN" >/dev/null 2>/tmp/mp_strace.txt || true
  if [ -s /tmp/mp_strace.txt ]; then
    sed 's/^/    /' /tmp/mp_strace.txt | head -12
    echo "    期望：brk/mmap 次数是常数，不随迭代数增长（把用例迭代数调大再跑一次对比）"
  else
    warn "strace 未产出统计（可能被权限限制）"
  fi
else
  warn "未安装 strace（apt install strace）"
fi

# ---------------------------------------------------------------- 3. page fault / cache
bold "3. page-fault 与 cache 统计"
if command -v perf >/dev/null 2>&1; then
  perf stat -e page-faults,minor-faults,cache-misses,cache-references,context-switches \
    "$BIN" 2>&1 >/dev/null | sed 's/^/    /' | head -12
  echo "    期望：预热后 page-faults 不再随迭代数增长；伪共享修好后 cache-misses 明显下降"
else
  warn "未安装 perf（apt install linux-perf）"
fi

# ---------------------------------------------------------------- 4. 线程栈
bold "4. 线程与栈（压测用例运行时的观测建议）"
echo "    运行时另开终端:  pmap \$(pgrep -f buffer_pool_test | head -1) | tail -5"
echo "    期望：8 条线程栈各 8MB 虚拟（ulimit -s），但 RSS 远小于它"

bold ""
if [ "$FAIL" -eq 0 ]; then
  printf '\033[32mprobe 通过\033[0m —— 把上面的输出贴进 docs/M1-句柄与内存池.md\n'
else
  printf '\033[31m%d 项不达标\033[0m\n' "$FAIL"
fi
exit "$FAIL"
