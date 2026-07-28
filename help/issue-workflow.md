# Git Issue 工作流指南

## 概述

本文档描述本项目的**每日计划 → Git Issue 管理**工作流。每个学习日（Day N）按照统一流程分解任务、创建 Issue、输出学习成果。

---

## 工作流全景

```
每日计划 (Daily Plan)
    │
    ▼
分解任务 ──────────────────────────────────┐
    │                                        │
    ├── 理论学习任务 (learning)              │
    ├── 知识输出任务 (documentation)          │── 创建 GitHub Issues
    └── Demo 实现任务 (demo, backlog)        │
                                              │
    ▼                                          ▼
创建输出文件 (Qxxx_xxx.md) ────→ Commit & Push
```

---

## 步骤详解

### Step 1: 定义每日计划

在开始 Day N 之前，先明确：

| 要素 | 说明 | 示例 (Day1) |
|:----|------|-------------|
| **主题** | 当日学习的核心专题 | RAII + 对象生命周期 |
| **学习内容** | 需要掌握的知识点清单 | 什么是 RAII、异常安全、栈/堆生命周期、构造/析构 |
| **Demo** | 需要实现的代码示例 | File, Mutex, LockGuard, Logger |
| **输出** | 需要产出的文档 | Q001_RAII.md, Q002_对象生命周期.md |

### Step 2: 创建输出文件

根据每日计划，在对应专题目录下创建学习输出文件。

**命名规范：**
```
Q{编号}_{主题英文或拼音}.md
```

**文件位置：**
```
cpp/{专题编号}-{专题名称}/
```

**示例：**
- `cpp/01-object-lifetime/Q001_RAII.md`
- `cpp/01-object-lifetime/Q002_对象生命周期.md`

### Step 3: 创建 GitHub Issues

将每日计划分解为 **3 类 Issue**：

#### 3.1 理论学习任务 (learning)

| 标签 | `dayN`, `learning` |
|:----|:-------------------|
| **内容** | 明确的学习要点列表 |
| **验收标准** | Checklist 形式的自测项 |
| **依赖** | 无（可并行执行） |

**模板：**

```markdown
## 学习内容

### 1. {主题 1}
- {要点 1}
- {要点 2}

### 2. {主题 2}
- {要点 1}
- {要点 2}

### 参考文件
- {文件路径}

### 验收标准
- [ ] {标准 1}
- [ ] {标准 2}
```

#### 3.2 知识输出任务 (documentation)

| 标签 | `dayN`, `documentation` |
|:----|:------------------------|
| **内容** | 明确要输出的文档及内容范围 |
| **依赖** | 依赖对应的学习任务 Issue |

**模板：**

```markdown
## 输出内容

整理 {主题} 核心知识，输出到 {文件名}

### 要求
- {要求 1}
- {要求 2}

### 依赖
- #{关联的学习 Issue 编号}
```

#### 3.3 Demo 实现任务 (demo, backlog)

| 标签 | `dayN`, `demo`, `backlog` |
|:----|:--------------------------|
| **内容** | Demo 的简要描述和实现要点 |
| **状态** | 默认标记为 backlog，暂不实现 |

**模板：**

```markdown
## Demo 说明

实现 {Demo 名称}，展示 {概念} 在 {场景} 中的应用

### 实现要点
- {要点 1}
- {要点 2}

### 依赖
- #{关联的学习 Issue 编号}
```

### Step 4: 标签体系

| 标签 | 颜色 | 用途 | 示例 |
|:----|:----:|:----|:----|
| `dayN` | `#0E8A16` 🟢 | 归属某一天的计划 | `day1`, `day2` |
| `learning` | `#1D76DB` 🔵 | 理论学习任务 | 理解 RAII 概念 |
| `documentation` | `#0E8A16` 🟢 | 文档输出任务 | 撰写 Q001 |
| `demo` | `#BFD4F2` 🔷 | 代码实现任务 | 实现 Logger |
| `backlog` | `#B60205` 🔴 | 待办暂缓 | 暂不实现的 Demo |

### Step 5: Commit & Push

完成输出文件后，提交到 Git 仓库：

```bash
# 添加输出文件
git add cpp/{专题目录}/Q*.md

# 提交并关联 Issue
git commit -m "feat(dayN): add Qxxx description

Closes #N"

# 推送到远程
git push
```

---

## 实际案例：Day1

### 计划摘要

| 项目 | 内容 |
|:----|------|
| **主题** | RAII + 对象生命周期 |
| **学习内容** | RAII 概念、异常安全、栈/堆生命周期、构造/析构 |
| **Demo** | File, Mutex, LockGuard, Logger |
| **输出** | Q001_RAII.md, Q002_对象生命周期.md |

### 创建的 Issues

| # | 标题 | 标签 |
|:-:|:----|:----:|
| #2 | 学习任务：理解 RAII 核心概念 | `day1`, `learning` |
| #3 | 学习任务：掌握 C++ 对象生命周期 | `day1`, `learning` |
| #4 | 知识输出：撰写 Q001_RAII.md | `day1`, `documentation` |
| #5 | 知识输出：撰写 Q002_对象生命周期.md | `day1`, `documentation` |
| #6 | [Backlog] Demo：实现 File 类 | `day1`, `demo`, `backlog` |
| #7 | [Backlog] Demo：实现 Mutex 类 | `day1`, `demo`, `backlog` |
| #8 | [Backlog] Demo：实现 LockGuard 类 | `day1`, `demo`, `backlog` |
| #9 | [Backlog] Demo：实现 Logger 类 | `day1`, `demo`, `backlog` |

### 依赖关系图

```
Day1
 ├── #2 (RAII 学习) ─────────→ #4 (Q001 输出)
 ├── #3 (生命周期学习) ──────→ #5 (Q002 输出)
 └── #6-#9 (Demo) ────────── backlog
```

---

## 快速参考卡片

### Issue 创建命令模板

```bash
# 理论学习
gh issue create \
  --title "[Day{N}] 学习任务：{主题}" \
  --label "day{N},learning" \
  --body "..."

# 知识输出
gh issue create \
  --title "[Day{N}] 知识输出：撰写 {文件名}" \
  --label "day{N},documentation" \
  --body "..."

# Demo backlog
gh issue create \
  --title "[Day{N}][Backlog] Demo：实现 {类名}" \
  --label "day{N},demo,backlog" \
  --body "..."
```

### 文件操作速查

```bash
# 创建输出文件
touch cpp/{专题目录}/Q{编号}_{主题}.md

# 提交
git add cpp/{专题目录}/Q*.md
git commit -m "feat(dayN): add Qxxx"
git push
```

---

## 最佳实践

1. **标签先行** — 在创建 Issue 之前先把本日标签建好
2. **依赖明确** — 在 Issue body 中用 `Depends on #N` 标注依赖关系
3. **验收清单** — 每个学习 Issue 都附带 checklist，完成即打勾
4. **输出即提交** — 输出文件完成后立即 commit，保持与 Issue 同步
5. **Backlog 分离** — Demo 等长周期任务用 backlog 标签标记，不影响主线进度
6. **批量创建** — 同一天的多个 Issue 可以批量创建，提高效率
