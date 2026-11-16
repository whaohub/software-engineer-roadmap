// Demo 1 · 固定内存池框架（仅骨架，无实现）
//
// 目标：理解 Pool 和 Chunk 的所有权关系
//   1. 一次性分配 3 个固定大小的 Chunk
//   2. 能够通过 Index 找到对应内存
//   3. 不实现 FreeList（下一个 demo 再加）
//
// 编译：g++ -std=c++17 -Wall -Wextra demo/mem_pool_demo1.cpp -o /tmp/mpd1
// 运行：/tmp/mpd1  （当前会断言失败，等你填完实现）

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdio>

/// 固定内存池：拥有 N 个等大的 Chunk，通过索引访问。
/// 只有 Pool 一个所有者，Chunk 跟着 Pool 一起生灭。
class SimplePool {
 public:
  // ─── 构造 / 析构 ────────────────────────────────────────────
  // 一次性分配 chunk_size * chunk_count 字节的连续内存。
  SimplePool(std::size_t chunk_size, std::uint32_t chunk_count);

  // 释放所有 Chunk 占用的内存。Pool 析构 = 所有 Chunk 一起销毁。
  ~SimplePool();

  // 禁止拷贝（内存所有权唯一）
  SimplePool(const SimplePool&) = delete;
  SimplePool& operator=(const SimplePool&) = delete;

  // ─── 访问 ──────────────────────────────────────────────────
  // 通过索引拿到第 index 个 Chunk 的起始地址。
  // index 越界时行为未定义（调用方保证）。
  std::byte* block(std::uint32_t index) const;

  std::size_t chunk_size() const { return chunk_size_; }
  std::uint32_t chunk_count() const { return chunk_count_; }

 private:
  std::size_t chunk_size_;
  std::uint32_t chunk_count_;
  // TODO(Demo1): 加一个成员存那块"一次性分配的连续内存"，类型是什么？
};

// ═══════════════════════════════════════════════════════════════════════
// 以下是实现区，全是 TODO 桩，等你自己填
// ═══════════════════════════════════════════════════════════════════════

SimplePool::SimplePool(std::size_t chunk_size, std::uint32_t chunk_count)
    : chunk_size_(chunk_size), chunk_count_(chunk_count) {
  // TODO(Demo1):
  //   1. 算总字节数 = chunk_size * chunk_count
  //   2. 一次性 new 出这么多字节，存到私有成员里
  //   3. 打印一行日志（chunk_size、chunk_count、起始地址）
  assert(false && "你还没填 SimplePool 的构造函数");
}

SimplePool::~SimplePool() {
  // TODO(Demo1): 释放构造函数里 new 出来的那块连续内存
  assert(false && "你还没填 SimplePool 的析构函数");
}

std::byte* SimplePool::block(std::uint32_t index) const {
  // TODO(Demo1):
  //   用"连续内存起始地址 + index * chunk_size_"算出第 index 个 Chunk 的起始地址
  //   就一行算术
  assert(false && "你还没填 block()");
}

// ═══════════════════════════════════════════════════════════════════════
// main · 只搭了测试框架，验证点已经写好，等实现填完就能跑
// ═══════════════════════════════════════════════════════════════════════

int main() {
  // ── 基本验证 ──────────────────────────────────────────────
  // 3 个 Chunk，每个 256 字节
  SimplePool pool(256, 3);
  std::printf("Pool 构造完毕\n");

  // 用 index 拿到每个 Chunk 的地址，打印出来
  std::byte* b0 = pool.block(0);
  std::byte* b1 = pool.block(1);
  std::byte* b2 = pool.block(2);
  std::printf("block(0)=%p\nblock(1)=%p\nblock(2)=%p\n",
              (void*)b0, (void*)b1, (void*)b2);

  // ── 你自己验证的三个点（填完实现后应该都通过） ──────────────
  // 验证 1：三个地址互不相同
  assert(b0 != b1 && b1 != b2 && b0 != b2);

  // 验证 2：相邻块的地址差刚好等于 chunk_size（这就是"连续内存"的含义）
  assert(static_cast<std::size_t>(b1 - b0) == 256);
  assert(static_cast<std::size_t>(b2 - b1) == 256);

  // 验证 3：Pool 析构时，所有 Chunk 跟着销毁（用 valgrind 确认无泄漏）

  std::printf("\n🎉 Demo1 通过：Pool 拥有 3 个 Chunk，连续排列，可通过 index 访问\n");
  return 0;
}
