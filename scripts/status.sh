#!/usr/bin/env bash
# 一键拿到全部上下文：当前阶段 / 下一步 / 未提交改动 / 最近提交 / 仓库健康摘要
# 用法: ./scripts/status.sh
set -uo pipefail
cd "$(dirname "${BASH_SOURCE[0]}")/.." || exit 1

bold() { printf '\033[1m%s\033[0m\n' "$*"; }
dim()  { printf '\033[1;90m%s\033[0m\n' "$*"; }

bold "═══ 仓库 ═══"
printf '路径: %s\n' "$PWD"
printf '分支: %s\n' "$(git rev-parse --abbrev-ref HEAD 2>/dev/null || echo '(非 git 仓库)')"
printf '最近提交:\n'
git log --oneline -5 2>/dev/null | sed 's/^/  /'

bold ""
bold "═══ 当前阶段（PROGRESS.md）═══"
grep -E '^\| (S0|[A-E]) ' PROGRESS.md 2>/dev/null | awk -F'|' '{for(i=2;i<=5;i++) gsub(/^ +| +$/,"",$i); printf "  %s (%s) %s — %s\n", $2, $3, $4, $5}' \
  || dim "  (未找到 PROGRESS.md)"

bold ""
bold "═══ 下一步（NEXT.md）═══"
if [ -f NEXT.md ]; then
  # 打印 "现在做" 段落到下一个二级标题
  awk '/^## 1\. 现在做/{flag=1} flag&&/^## /&&!/^## 1\./{flag=0} flag' NEXT.md | sed 's/^/  /'
else
  dim "  (未找到 NEXT.md)"
fi

bold ""
bold "═══ 工作区状态 ═══"
if [ -z "$(git status --porcelain 2>/dev/null)" ]; then
  printf '  干净（无未提交改动）\n'
else
  printf '  有未提交改动 —— 先收尾再开新任务：\n'
  git status --short | head -15 | sed 's/^/    /'
  n=$(git status --porcelain | wc -l | tr -d ' ')
  [ "$n" -gt 15 ] && printf '    … 共 %s 项\n' "$n"
fi

bold ""
bold "═══ 健康摘要（./scripts/check.sh 的简版）═══"
tracked=$(git ls-files 2>/dev/null | grep -cE '(^|/)build/|CMakeCache\.txt' || true)
printf '  被跟踪的构建产物: %s\n' "${tracked:-0}"
printf '  有内容的专题: %s\n' "$(grep -c '✅\|🚧' PROGRESS.md 2>/dev/null || echo '?')"
printf '  未跟踪文件数: %s\n' "$(git ls-files --others --exclude-standard 2>/dev/null | wc -l | tr -d ' ')"

bold ""
dim "提示: 完整检查用 ./scripts/check.sh；工具链自检用 ./scripts/bootstrap.sh"
