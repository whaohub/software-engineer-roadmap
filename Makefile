# 统一入口：任何 agent（Copilot / Cursor / Claude Code / Codex）与任何终端都能用
# 用法：make 或 make help

SHELL := /bin/bash
.DEFAULT_GOAL := help

# 默认专题（跑测试时用）：make test DIR=algorithms/src/02-linked-list
DIR ?= algorithms/src/02-linked-list

# 复用已下载的 gtest，避免每次联网；目录不存在时自动退回联网下载
GTEST_SRC := $(CURDIR)/algorithms/src/01-array-string/build/_deps/googletest-src
GTEST_FLAG := $(if $(wildcard $(GTEST_SRC)/CMakeLists.txt),-DFETCHCONTENT_SOURCE_DIR_GOOGLETEST=$(GTEST_SRC),)

.PHONY: help status check bootstrap test test-asan test-tsan probe demo clean plan plan-new

help: ## 显示可用命令
	@echo "dev_tool · 学习仓库统一入口"
	@echo
	@grep -hE '^[a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) \
		| awk 'BEGIN {FS = ":.*?## "}; {printf "  make %-12s %s\n", $$1, $$2}'
	@echo
	@echo "示例:"
	@echo "  make status        # 上下文（含最新计划）"
	@echo "  make plan          # 直接看今天的执行清单"
	@echo "  make plan-new      # 生成明天的计划模板"
	@echo "  make test DIR=algorithms/src/02-linked-list"
	@echo "  make test DIR=project/mini-pipeline       # 实战项目 M1"
	@echo "  make test-asan DIR=project/mini-pipeline  # 内存/UB 检查"
	@echo "  make test-tsan DIR=project/mini-pipeline  # 数据竞争检查"
	@echo "  make probe DIR=project/mini-pipeline      # Linux 侧验收（STRACE/PERF）"
	@echo "  make demo FILE=cpp/04-smart-pointers/src/mini_shared_ptr.cpp"

status: ## 上下文全景（当前阶段 / 计划 / 下一步 / 未提交改动）
	@./scripts/status.sh

plan: ## 查看最新一份每日计划（plans/YYYY-MM-DD.md）
	@latest=$$(ls -1 plans/[0-9]*.md 2>/dev/null | sort | tail -1); \
	if [ -z "$$latest" ]; then echo "plans/ 下暂无计划，先跑 make plan-new"; exit 1; fi; \
	echo ">> $$latest"; echo; cat "$$latest"

plan-new: ## 生成下一天的计划文件（模板）
	@./scripts/new-plan.sh

check: ## 仓库验收（卫生 / 死链 / 四件套矩阵 / 密钥）
	@./scripts/check.sh

bootstrap: ## 新机器工具链自检
	@./scripts/bootstrap.sh

test: ## 构建并跑某个专题的 gtest：DIR=...
	@echo ">> $(DIR)"
	@cd $(DIR) && cmake -S . -B build $(GTEST_FLAG) >/dev/null && cmake --build build -j8 && cd build && ctest --output-on-failure

test-asan: ## ASan+UBSan 跑专题测试：DIR=...
	@cd $(DIR) && cmake -S . -B build-asan -DMP_SANITIZE=address,undefined $(GTEST_FLAG) >/dev/null \
	  && cmake --build build-asan -j8 && cd build-asan && ctest --output-on-failure

test-tsan: ## TSan 跑专题测试（查数据竞争）：DIR=...
	@cd $(DIR) && cmake -S . -B build-tsan -DMP_SANITIZE=thread $(GTEST_FLAG) >/dev/null \
	  && cmake --build build-tsan -j8 && cd build-tsan && ctest --output-on-failure

probe: ## Linux 侧验收（RSS / syscall / page-fault / cache）：DIR=project/mini-pipeline
	@./$(DIR)/scripts/linux-probe.sh $(DIR)/build/buffer_pool_test

demo: ## 编译并运行单个 C++ 文件（C++17 + 告警全开）：FILE=...
	@test -n "$(FILE)" || { echo "用法: make demo FILE=cpp/04-smart-pointers/src/xxx.cpp"; exit 1; }
	@set -e; out=/tmp/roadmap-demo/$$(basename "$(FILE)" .cpp); \
	mkdir -p "$$(dirname "$$out")"; \
	c++ -std=c++17 -Wall -Wextra -O0 -g -pthread "$(FILE)" -o "$$out"; \
	"$$out"

clean: ## 清理本地产物（build/ 与 demo 临时文件）
	@find . -type d -name build -not -path "./third_party/*" -prune -exec rm -rf {} + 2>/dev/null || true
	@rm -rf /tmp/roadmap-demo
	@echo "已清理 build/ 与 /tmp/roadmap-demo"
