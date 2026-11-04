#!/usr/bin/env bash
# 生成某天的计划文件（默认明天），已存在则不覆盖
# 用法: ./scripts/new-plan.sh [YYYY-MM-DD]
set -uo pipefail
cd "$(dirname "${BASH_SOURCE[0]}")/.." || exit 1

# 默认日期：已有计划则取「最新计划的次日」，否则取明天
next_day() { # $1=YYYY-MM-DD
  if date -j -f "%Y-%m-%d" "$1" -v+1d +%Y-%m-%d >/dev/null 2>&1; then
    date -j -f "%Y-%m-%d" "$1" -v+1d +%Y-%m-%d
  else
    date -d "$1 +1 day" +%Y-%m-%d
  fi
}

target="${1:-}"
if [ -z "$target" ]; then
  latest=$(ls -1 plans/*.md 2>/dev/null | grep -E '/[0-9]{4}-[0-9]{2}-[0-9]{2}\.md$' | sort | tail -1)
  if [ -n "$latest" ]; then
    target=$(next_day "$(basename "$latest" .md)")
  elif date -v+1d +%Y-%m-%d >/dev/null 2>&1; then
    target=$(date -v+1d +%Y-%m-%d)
  else
    target=$(date -d "+1 day" +%Y-%m-%d)
  fi
fi

case "$target" in
  [0-9][0-9][0-9][0-9]-[0-9][0-9]-[0-9][0-9]) ;;
  *) echo "日期格式必须是 YYYY-MM-DD，收到: $target" >&2; exit 1 ;;
esac

if date -j -f "%Y-%m-%d" "$target" +%A >/dev/null 2>&1; then
  weekday=$(date -j -f "%Y-%m-%d" "$target" +%A)
else
  weekday=$(date -d "$target" +%A)
fi

mkdir -p plans
file="plans/${target}.md"
if [ -f "$file" ]; then
  echo "已存在: $file"
  echo "编辑: \${EDITOR:-vim} $file"
  exit 0
fi

prev=$(ls -1 plans/*.md 2>/dev/null | grep -E '/[0-9]{4}-[0-9]{2}-[0-9]{2}\.md$' | sort | tail -1)
prev_link=""
[ -n "$prev" ] && prev_link="上一份：[$(basename "$prev" .md)](./$(basename "$prev")) · "

cat > "$file" <<EOF
# 计划 ${target}（${weekday}）

> 阶段 **S0（W1）** · 预算 **55 分钟**
> ${prev_link}方向：[PLAN.md](../PLAN.md) · 契约：[AGENTS.md](../AGENTS.md)
> 开场：\`make status\` → 只推进本文件「1. 现在做」的第一件事。

## 1. 现在做（按顺序，最多 3 条）

### Block A · 25 分钟｜（填任务）

- 做什么：
- 命令：
- 验收：
- 完成标志：\`git commit -m "..."\`

### Block B · 20 分钟｜（填任务）

- 做什么：
- 命令：
- 验收：

### Block C · 10 分钟｜收工

\`\`\`bash
make check
# PROGRESS.md 追加一行 + NEXT.md 更新 + 本文件「复盘」填 3 行
git add -A && git commit -m "chore(plan): ${target} 复盘 + 更新 NEXT/PROGRESS"
\`\`\`

## 2. 有余力再做

- [ ]

## 3. 今天明确不做

- ❌

## 复盘（收工填）

- 做完了什么：
- 没做完的为什么：
- 明天第一件事：
EOF

echo "已创建: $file"
echo "下一步: 填好「1. 现在做」，并让 NEXT.md 的第 1 条指向它"
echo "查看:   make plan"
