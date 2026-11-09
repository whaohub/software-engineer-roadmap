#!/usr/bin/env bash
# 机器可执行的验收：任何 agent / 任何机器跑这一个命令，就知道仓库是否健康、哪些专题没做完
#   ./scripts/check.sh          报告 + 硬性错误返回非 0
#   ./scripts/check.sh --quiet  只输出问题
set -uo pipefail
cd "$(dirname "${BASH_SOURCE[0]}")/.." || exit 1

QUIET=0
[ "${1:-}" = "--quiet" ] && QUIET=1

FAIL=0
bold() { printf '\n\033[1m%s\033[0m\n' "$*"; }
ok()   { [ "$QUIET" -eq 1 ] || printf '  \033[32m✓\033[0m %s\n' "$*"; }
bad()  { printf '  \033[31m✗\033[0m %s\n' "$*"; FAIL=$((FAIL + 1)); }
warn() { printf '  \033[33m!\033[0m %s\n' "$*"; }

# ---------------------------------------------------------------- 1. 仓库卫生
bold "1. 仓库卫生"

junk=$(git ls-files | grep -E '(^|/)build/|CMakeCache\.txt|\.(o|a|so|out|dSYM)$' || true)
if [ -z "$junk" ]; then
  ok "无构建产物入库"
else
  bad "有 $(printf '%s\n' "$junk" | wc -l | tr -d ' ') 个构建产物被跟踪"
  printf '%s\n' "$junk" | head -5 | sed 's/^/      /'
fi

secrets=$(git grep -nE 'sk-[A-Za-z0-9]{16,}|AKIA[0-9A-Z]{16}|BEGIN (RSA|OPENSSH) PRIVATE KEY' -- . ':!*example*' ':!*.md' 2>/dev/null || true)
if [ -z "$secrets" ]; then
  ok "无明文密钥"
else
  bad "疑似密钥泄漏："
  printf '%s\n' "$secrets" | head -3 | sed 's/^/      /'
fi

if git ls-files | grep -qE '(^|/)\.env$|secrets\.zsh'; then
  bad "密钥文件被跟踪（应为 gitignore）"
else
  ok "密钥文件未入库"
fi

# bash 3.2 陷阱：$var 紧跟中文会被当成变量名的一部分（set -u 下直接崩溃）
if grep -nE '\$[A-Za-z_][A-Za-z0-9_]*[^\x00-\x7f]' scripts/*.sh >/dev/null 2>&1; then
  if LC_ALL=C grep -nE '\$[A-Za-z_][A-Za-z0-9_]*[^\x00-\x7f]' scripts/*.sh >/dev/null 2>&1; then
    bad "脚本里有 \$var 紧跟中文（中文紧跟变量名会被 bash 当成变量名）"
  else
    ok "脚本中的变量展开写法规避了 bash 3.2 中文陷阱"
  fi
else
  ok "脚本中的变量展开写法规避了 bash 3.2 中文陷阱"
fi

# ---------------------------------------------------------------- 2. Markdown 内链
bold "2. Markdown 内链"
if ! command -v python3 >/dev/null 2>&1; then
  warn "缺 python3，跳过内链检查"
else
  python3 - <<'PY'
import re, subprocess, os, sys
out = subprocess.run(["git", "-c", "core.quotepath=off", "ls-files", "-z", "*.md"],
                     capture_output=True, text=True).stdout
files = [f for f in out.split("\0") if f]
bad = []
for f in files:
    base = os.path.dirname(f)
    try:
        text = open(f, encoding="utf-8").read()
    except Exception:
        continue
    for m in re.finditer(r"\[[^\]]*\]\((\.{1,2}/[^)\s#]+)", text):
        target = m.group(1)
        if not os.path.exists(os.path.normpath(os.path.join(base or ".", target))):
            bad.append(f"{f} -> {target}")
if bad:
    print(f"  \033[31m✗\033[0m 死链 {len(bad)} 处")
    for b in bad[:10]:
        print("      " + b)
    sys.exit(1)
print(f"  \033[32m✓\033[0m {len(files)} 个 md 无死链")
PY
  [ $? -ne 0 ] && FAIL=$((FAIL + 1))
fi

# ---------------------------------------------------------------- 3. 专题完成度
bold "3. 专题完成度矩阵（文档 / 源码 / 测试）"
printf '  %-34s %-6s %-6s %-6s %s\n' "专题" "文档" "源码" "测试" "判定"
for d in cpp/*/ linux/*/ middleware/*/ project/*/ algorithms/src/*/; do
  [ -d "$d" ] || continue
  case "$d" in
    */build/*|linux/apue/|linux/assembly/) continue ;;
  esac
  name=$(printf '%s' "$d" | sed 's:/$::')
  docs=$(find "$d" -name '*.md' -not -path '*/build/*' -not -name 'README.md' 2>/dev/null | wc -l | tr -d ' ')
  src=$(find "$d" \( -name '*.c' -o -name '*.cpp' -o -name '*.h' -o -name '*.hpp' \) -not -path '*/build/*' -not -path '*/third_party/*' 2>/dev/null | wc -l | tr -d ' ')
  tests=$(find "$d" -name '*_test.cpp' -not -path '*/build/*' 2>/dev/null | wc -l | tr -d ' ')

  if [ "$docs" -eq 0 ] && [ "$src" -eq 0 ]; then
    verdict="空（仅 README）"
  elif [ "$src" -eq 0 ]; then
    verdict="缺源码 ⚠"
  elif [ "$docs" -eq 0 ] && [ "$tests" -eq 0 ]; then
    verdict="骨架（待文档+测试）"
  elif [ "$docs" -eq 0 ]; then
    verdict="缺文档 ⚠"
  elif [ "$tests" -eq 0 ]; then
    verdict="缺测试 ⚠"
  else
    verdict="✅ 基本完整（数据待补）"
  fi
  printf '  %-34s %-6s %-6s %-6s %s\n' "$name" "$docs" "$src" "$tests" "$verdict"
done

# ---------------------------------------------------------------- 4. 操作层文件
bold "4. 操作层文件"
for f in AGENTS.md NEXT.md PROGRESS.md PLAN.md README.md Makefile plans/README.md \
         project/mini-pipeline/REQUIREMENTS.md project/mini-pipeline/TASK.md \
         project/mini-pipeline/README.md project/mini-pipeline/docs/ADR.md \
         project/mini-pipeline/docs/PREREQ.md project/mini-pipeline/docs/LINUX-PREREQ.md \
         project/mini-pipeline/scripts/linux-probe.sh .vscode/tasks.json \
         CLAUDE.md .github/copilot-instructions.md .cursor/rules/workflow.mdc \
         scripts/status.sh scripts/check.sh scripts/bootstrap.sh; do
  if [ -f "$f" ]; then ok "$f"; else bad "缺少 $f"; fi
done
if [ -f NEXT.md ]; then
  n=$(grep -c '^- \[ \]' NEXT.md || true)
  [ "${n:-0}" -gt 3 ] && warn "NEXT.md 有 $n 条待办（规则：最多 3 条）" || ok "NEXT.md 待办数量合规（${n:-0}）"
fi

# ---------------------------------------------------------------- 4.5 每日计划
bold "4.5 每日计划（plans/）"
badnames=""
for f in plans/*.md; do
  [ -e "$f" ] || continue
  b=$(basename "$f")
  case "$b" in
    README.md) continue ;;
    [0-9][0-9][0-9][0-9]-[0-9][0-9]-[0-9][0-9].md) ;;
    *) badnames="$badnames $b" ;;
  esac
done
if [ -z "$badnames" ]; then
  ok "命名规范（YYYY-MM-DD.md）"
else
  bad "以下文件名不符合 YYYY-MM-DD.md:$badnames"
fi
latest=$(ls -1 plans/[0-9]*.md 2>/dev/null | sort | tail -1)
if [ -n "$latest" ]; then
  d=$(basename "$latest" .md); t=$(date +%Y-%m-%d)
  if [ "$d" \< "$t" ]; then
    warn "最新计划是 ${d}，今天（${t}）还没建 → make plan-new"
  else
    ok "最新计划: $latest"
  fi
  if ! grep -q '^- 做完了什么：.\+' "$latest"; then
    dim_note="最新计划尚未填写复盘"
    printf '  \033[33m!\033[0m %s\n' "$dim_note"
  fi
else
  warn "plans/ 下暂无计划文件 → make plan-new"
fi

# ---------------------------------------------------------------- 5. 未跟踪文件
bold "5. 未跟踪文件（确认是否该提交）"
others=$(git ls-files --others --exclude-standard | head -10)
if [ -z "$others" ]; then
  ok "无未跟踪文件"
else
  warn "有未跟踪文件："
  printf '%s\n' "$others" | sed 's/^/      /'
fi

bold ""
if [ "$FAIL" -eq 0 ]; then
  printf '\033[32m检查通过\033[0m\n'
else
  printf '\033[31m%d 项硬性检查未通过\033[0m\n' "$FAIL"
fi
exit "$FAIL"
