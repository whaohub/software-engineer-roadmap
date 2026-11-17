// Demo 2 · 引入 FreeList
//
// 两个版本，用 -DUSE_FREE_LIST 切换：
//   版本 A（默认）：vector<bool> 遍历找空闲块，O(N) acquire，FIFO
//   版本 B（加 -DUSE_FREE_LIST）：free list 栈，O(1) acquire/release，LIFO
//
// 思考场景（先答完再写代码，写在便签上）：
//   1. 100 万 Chunk + vector<bool> 遍历找空闲块，最坏情况每次 acquire
//      要遍历多少次？跑 100 万次借还要多久？（别写代码，先算最坏复杂度）
//   2. 遍历法是 FIFO（先借编号小的）。刚还了块 7 立刻再借，会优先给块 0
//      （如果它空闲），而不是刚还的块 7——CPU cache 命中率低。
//      怎么让池优先借给你刚还回来的块？
//   3. 多线程同时调用 acquire()，两个线程会不会同时遍历到同一个 false 位置，
//      都以为自己拿到了这个块？
//
// 编译运行：
//   版本 A：g++ -std=c++17 -Wall -Wextra demo/mem_pool_demo2.cpp -o /tmp/mpd2a
//   && /tmp/mpd2a 版本 B：g++ -std=c++17 -Wall -Wextra -DUSE_FREE_LIST
//   demo/mem_pool_demo2.cpp -o /tmp/mpd2b && /tmp/mpd2b
//
// 性能测试（加 -DPERF_TEST）：
//   版本 A：g++ -std=c++17 -Wall -Wextra -DPERF_TEST demo/mem_pool_demo2.cpp -o
//   /tmp/mpd2ap && /tmp/mpd2ap 版本 B：g++ -std=c++17 -Wall -Wextra -DPERF_TEST
//   -DUSE_FREE_LIST demo/mem_pool_demo2.cpp -o /tmp/mpd2bp && /tmp/mpd2bp
//   对比：两次跑出来的 ms 差距就是 free list 的价值

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <vector>

#ifdef PERF_TEST
#include <chrono>
#endif

// ── 版本选择 ──────────────────────────────────────────────────
// 默认版本 A，-DUSE_FREE_LIST 启用版本 B
#ifdef USE_FREE_LIST
#define POOL_VERSION "B (free list)"
#else
#define POOL_VERSION "A (vector<bool> traversal)"
#endif

class StatefulPool {
public:
  StatefulPool(std::size_t chunk_size, std::uint32_t chunk_count);
  ~StatefulPool();

  StatefulPool(const StatefulPool &) = delete;
  StatefulPool &operator=(const StatefulPool &) = delete;

  std::byte *acquire();
  void release(std::byte *p);
  std::byte *block(std::uint32_t index) const;

  std::size_t chunk_size() const { return chunk_size_; }
  std::uint32_t chunk_count() const { return chunk_count_; }
  std::uint32_t in_use() const { return in_use_; }
  std::uint32_t index_of(std::byte *p) const;

private:
  std::size_t chunk_size_{0};
  std::uint32_t chunk_count_{0};
  std::byte *memory_ = nullptr;

#ifndef USE_FREE_LIST
  // 版本 A：vector<bool> 遍历找空闲块
  std::vector<bool> used_;
#else
  // 版本 B：free list 栈
  std::vector<std::uint32_t> free_idx_;
#endif

  std::uint32_t in_use_ = 0;
};

// ═══════════════════════════════════════════════════════════════════════
// 实现区
// ═══════════════════════════════════════════════════════════════════════

StatefulPool::StatefulPool(std::size_t chunk_size, std::uint32_t chunk_count)
    : chunk_size_(chunk_size), chunk_count_(chunk_count) {
  memory_ = new std::byte[chunk_size_ * chunk_count_];

#ifndef USE_FREE_LIST
  used_.resize(chunk_count_, false);
#else
  free_idx_.reserve(chunk_count_);
  // 初始化：0,1,2,...,N-1 全部压栈 → 栈顶是 N-1
  for (std::uint32_t i = 0; i < chunk_count_; ++i) {
    free_idx_.push_back(i);
  }
#endif

  std::printf("[构造] 版本=%s  chunk_size=%zu  chunk_count=%u  arena=%p\n",
              POOL_VERSION, chunk_size, chunk_count, (void *)memory_);
}

StatefulPool::~StatefulPool() { delete[] memory_; }

std::uint32_t StatefulPool::index_of(std::byte *p) const {
  return static_cast<std::uint32_t>((p - memory_) / chunk_size_);
}

std::byte *StatefulPool::block(std::uint32_t index) const {
  return memory_ + index * chunk_size_;
}

std::byte *StatefulPool::acquire() {
#ifndef USE_FREE_LIST
  // 版本 A：遍历找第一个 false（O(N)，FIFO）
  for (std::uint32_t i = 0; i < chunk_count_; ++i) {
    if (!used_[i]) {
      used_[i] = true;
      in_use_++;
      return block(i);
    }
  }
  return nullptr;
#else
  // 版本 B：free list 栈弹顶（O(1)，LIFO）
  if (free_idx_.empty())
    return nullptr;
  std::uint32_t i = free_idx_.back();
  free_idx_.pop_back();
  in_use_++;
  return block(i);
#endif
}

void StatefulPool::release(std::byte *p) {
  std::uint32_t idx = index_of(p);
  assert(idx < chunk_count_);
#ifndef USE_FREE_LIST
  used_[idx] = false;
#else
  free_idx_.push_back(idx);
#endif
  in_use_--;
}

// ═══════════════════════════════════════════════════════════════════════
// main · 测试框架
// ═══════════════════════════════════════════════════════════════════════

int main() {
  std::printf("╔══════════════════════════════════════════════════╗\n");
  std::printf("║  Demo2 · 版本 %-34s ║\n", POOL_VERSION);
  std::printf("╚══════════════════════════════════════════════════╝\n");

  // ── 测试 1：基本借还 + 池耗尽 ────────────────────────────────
  std::printf("\n[测试 1] 基本借还 + 池耗尽\n");
  StatefulPool pool(256, 3);

  std::byte *a = pool.acquire();
  std::byte *b = pool.acquire();
  std::byte *c = pool.acquire();
  assert(a && b && c && a != b && b != c && a != c);
  std::printf("  借 3 块成功，in_use=%u\n", pool.in_use());

  std::byte *d = pool.acquire();
  assert(d == nullptr);
  std::printf("  第 4 次借返回 nullptr（池耗尽）✅\n");

  pool.release(b);
  assert(pool.in_use() == 2);
  std::printf("  归还 b，in_use=%u ✅\n", pool.in_use());

  // ── 测试 2：借还顺序（对比版本 A FIFO vs 版本 B LIFO） ─────────
  std::printf("\n[测试 2] 借还顺序（版本 A=FIFO, 版本 B=LIFO）\n");
  StatefulPool pool2(256, 4);

  std::byte *p0 = pool2.acquire();
  std::byte *p1 = pool2.acquire();
  std::byte *p2 = pool2.acquire();
  std::printf("  借 3 块 → index=%u, %u, %u\n", pool2.index_of(p0),
              pool2.index_of(p1), pool2.index_of(p2));

  pool2.release(p1);
  pool2.release(p2);
  std::printf("  还 index=%u, 还 index=%u\n", pool2.index_of(p1),
              pool2.index_of(p2));

  std::byte *px = pool2.acquire();
  std::uint32_t got = pool2.index_of(px);
  std::uint32_t last_released = pool2.index_of(p2); // 最后还的那个
  std::printf("  再借拿到 index=%u  （最后释放的是 %u）\n", got, last_released);

#ifdef USE_FREE_LIST
  // 版本 B：LIFO → 必须拿到最后释放的那个
  if (got == last_released) {
    std::printf("  ✅ LIFO 生效（版本 B 优先复用最近释放的）\n");
  } else {
    std::printf("  ❌ LIFO 失败！期望 %u 实际 %u\n", last_released, got);
    assert(false);
  }
#else
  // 版本 A：FIFO → 拿第一个空闲块（不一定是最后释放的）
  std::printf("  ℹ️  版本 A 是 FIFO，可能不是刚释放的（这就是慢的原因之一）\n");
#endif

  pool2.release(p0);
  pool2.release(px);
  assert(pool2.in_use() == 0);
  std::printf("  全部归还 ✅\n");

  // ── 测试 3：耗尽 → 归还 → 再借 ────────────────────────────────
  std::printf("\n[测试 3] 耗尽 → 归还 → 再借\n");
  StatefulPool pool3(256, 2);
  std::byte *x1 = pool3.acquire();
  std::byte *x2 = pool3.acquire();
  std::byte *x3 = pool3.acquire();
  assert(x1 && x2);
  assert(x3 == nullptr);
  std::printf("  借 2 块，第 3 块耗尽返回 nullptr ✅\n");

  pool3.release(x2);
  std::byte *x4 = pool3.acquire();
  assert(x4 != nullptr);
#ifdef USE_FREE_LIST
  assert(pool3.index_of(x4) == pool3.index_of(x2));
  std::printf("  还 x2 后再借，拿到的就是刚还的 index=%u ✅\n",
              pool3.index_of(x4));
#else
  std::printf("  还 x2 后再借，拿到 index=%u（版本 A 不保证是刚还的）\n",
              pool3.index_of(x4));
#endif

  pool3.release(x1);
  pool3.release(x4);
  assert(pool3.in_use() == 0);
  std::printf("  全部归还 ✅\n");

  // ── 测试 4：100 万次借还不泄漏 ────────────────────────────────
  std::printf("\n[测试 4] 100 万次借还循环（正确性）\n");
  StatefulPool loop_pool(128, 16);
  for (int i = 0; i < 1000000; ++i) {
    std::byte *p = loop_pool.acquire();
    assert(p);
    loop_pool.release(p);
  }
  assert(loop_pool.in_use() == 0);
  std::printf("  100 万次无泄漏 ✅\n");

#ifdef PERF_TEST
  // ── 性能测试 1：好 case（每次借还循环，块空闲在低位） ───────────
  std::printf("\n[性能 1] 好 case：acquire→release 循环\n");
  StatefulPool big1(128, 1000000);
  auto t0 = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < 1000000; ++i) {
    std::byte *p = big1.acquire();
    assert(p);
    big1.release(p);
  }
  auto t1 = std::chrono::high_resolution_clock::now();
  auto ms1 =
      std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
  std::printf("  版本 %s 耗时：%ld ms\n", POOL_VERSION, ms1);

  // ── 性能测试 2：坏 case（先借满 N 块，再循环换块） ────────────
  // 这是版本 A 真正 O(N) 的场景：池满载时，每次 release 把低位块标 false，
  // 但 acquire 要从 0 开始遍历，找到最后一个 true 之后的那个 false。
  // 版本 B 永远 O(1)。
  std::printf("\n[性能 2] 坏 case：先借满 N 块，再循环换块\n");
  std::printf("  版本 A 这里会暴露 O(N) 遍历；版本 B 仍是 O(1)\n");

  StatefulPool big2(128, 100000); // 10 万块（100 万太耗时）
  std::vector<std::byte *> held;
  held.reserve(100000);
  // 借满
  for (int i = 0; i < 100000; ++i) {
    held.push_back(big2.acquire());
    assert(held.back());
  }
  std::printf("  借满 10 万块，开始 10 万次换块...\n");

  auto t2 = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < 100000; ++i) {
    // 归还 held[i] → 池里第 i 块变空闲
    big2.release(held[i]);
    // 再借：版本 A 要遍历 i 个 true 才找到空位；版本 B O(1) 弹栈
    std::byte *fresh = big2.acquire();
    assert(fresh);
    held[i] = fresh;
  }
  auto t3 = std::chrono::high_resolution_clock::now();
  auto ms2 =
      std::chrono::duration_cast<std::chrono::milliseconds>(t3 - t2).count();
  std::printf("  版本 %s 耗时：%ld ms\n", POOL_VERSION, ms2);
  std::printf("  ↑ 版本 A 这里应该比版本 B 慢 10x~100x\n");
  // 版本 A (vector<bool> traversal) 耗时：38374 ms
  // 版本 B (free list) 耗时：2 ms
#endif

  std::printf("\n🎉 版本 %s 全部通过\n", POOL_VERSION);
  return 0;
}
