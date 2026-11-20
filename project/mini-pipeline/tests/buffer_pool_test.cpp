// mini-pipeline · M1 验收用例
// 规则：这些用例不许改实现语义来「迁就」你的实现；如果语义要变，先改 TASK.md
// 并说明理由。
//
//   阶段 1   基础流程（单线程）          BufferPoolTest
//   阶段 1b  空句柄安全                  BufferSafetyTest
//   阶段 1c  赋值/生命周期矩阵           BufferPoolAssignTest
//   阶段 1d  引用计数边界（补充）         BufferPoolAssignTest
//   阶段 2   单线程长跑（慢，100 万次）   BufferPoolSoakTest
//   阶段 3   并发        BufferPoolConcurrencyTest
//   阶段 3b  并发用例补齐（补充）         BufferPoolConcurrencyTest
//   阶段 3c  全局池并发                  BufferPoolGlobalTest
//
// 日常开发只跑基础流程：
//   ./buffer_pool_test --gtest_filter='BufferPoolTest.*:BufferSafetyTest.*'
// 阶段 2 单独跑：
//   ./buffer_pool_test --gtest_filter='BufferPoolSoakTest.*'
// 阶段 3（等 U4 把计数改成原子、free list 加锁后再看）：
//   ./buffer_pool_test --gtest_filter='BufferPoolConcurrencyTest.*'
#include <gtest/gtest.h>

#include <atomic>
#include <condition_variable>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <vector>

#include "mini_pipeline/buffer_pool.hpp"

namespace {

constexpr std::size_t kBlockSize = 256;
constexpr std::uint32_t kBlockCount = 64;

// ═══════════════════════════════════════════════════════════════
// 阶段 1：基础流程（单线程）
// 目标：acquire → 拷贝 → move → reset → 析构，记账正确、地址对齐
// ═══════════════════════════════════════════════════════════════
class BufferPoolTest : public ::testing::Test {
protected:
  mp::BufferPool pool_{kBlockSize, kBlockCount};
};

// 1. 句柄必须足够小：能塞进通道槽位
TEST_F(BufferPoolTest, HandleSizeFitsIn16Bytes) {
  EXPECT_LE(sizeof(mp::Buffer), 16u)
      << "句柄膨胀了，检查控制块是不是放在了堆上";
}

// 2. acquire 基本契约
TEST_F(BufferPoolTest, AcquireBasic) {
  auto buf = pool_.acquire();
  ASSERT_TRUE(buf) << "池是空的却拿不到块";
  EXPECT_NE(buf.data(), nullptr);
  EXPECT_EQ(buf.size(), kBlockSize);
  EXPECT_EQ(buf.use_count(), 1u);
}

// 3. 拷贝共享同一块内存，引用数 +1
TEST_F(BufferPoolTest, CopySharesBlock) {
  auto a = pool_.acquire();
  ASSERT_TRUE(a);
  auto b = a;
  EXPECT_EQ(a.data(), b.data());
  EXPECT_EQ(a.use_count(), 2u);
  EXPECT_EQ(b.use_count(), 2u);
}

// 4. move 转移所有权，不改变引用数
TEST_F(BufferPoolTest, MoveLeavesSourceEmpty) {
  auto a = pool_.acquire();
  ASSERT_TRUE(a);
  void *p = a.data();
  auto b = std::move(a);
  EXPECT_FALSE(a) << "move 之后源句柄应为空";
  ASSERT_TRUE(b);
  EXPECT_EQ(b.data(), p);
  EXPECT_EQ(b.use_count(), 1u);
}

// 5. reset 归还块，统计归零
TEST_F(BufferPoolTest, ResetReturnsBlock) {
  {
    auto a = pool_.acquire();
    ASSERT_TRUE(a);
    EXPECT_EQ(pool_.stats().in_use, 1u);
    a.reset();
    EXPECT_FALSE(a);
    EXPECT_EQ(pool_.stats().in_use, 0u) << "reset 之后块没有还回池";
  }
  EXPECT_EQ(pool_.stats().in_use, 0u);
}

// 6. 池耗尽：语义必须明确（返回空句柄，或抛 std::bad_alloc）
TEST_F(BufferPoolTest, PoolExhaustionIsDefined) {
  std::vector<mp::Buffer> held;
  held.reserve(kBlockCount);
  for (std::uint32_t i = 0; i < kBlockCount; ++i) {
    held.push_back(pool_.acquire());
    ASSERT_TRUE(held.back())
        << "第 " << i << " 次 acquire 就失败了，容量没算对";
  }
  bool defined = false;
#if defined(__cpp_exceptions)
  try {
    auto extra = pool_.acquire();
    defined = !extra; // 语义 A：返回空句柄
  } catch (const std::bad_alloc &) {
    defined = true; // 语义 B：抛 bad_alloc
  }
#else
  auto extra = pool_.acquire();
  defined = !extra;
#endif
  EXPECT_TRUE(defined)
      << "池耗尽后既没返回空句柄也没抛异常 —— 这会让调用方拿到野指针";
}

// 7. 不同块不能重叠
TEST_F(BufferPoolTest, BlocksDoNotAlias) {
  auto a = pool_.acquire();
  auto b = pool_.acquire();
  ASSERT_TRUE(a);
  ASSERT_TRUE(b);
  const auto *pa = reinterpret_cast<const std::byte *>(a.data());
  const auto *pb = reinterpret_cast<const std::byte *>(b.data());
  ASSERT_NE(pa, pb);
  const auto diff = pa < pb ? pb - pa : pa - pb;
  EXPECT_GE(static_cast<std::size_t>(diff), kBlockSize) << "两块内存重叠了";
}

// 8. 块起始地址要满足最大对齐
TEST_F(BufferPoolTest, DataIsAligned) {
  auto a = pool_.acquire();
  ASSERT_TRUE(a);
  EXPECT_EQ(reinterpret_cast<std::uintptr_t>(a.data()) %
                alignof(std::max_align_t),
            0u);
}

// 9. 峰值统计正确
TEST_F(BufferPoolTest, StatsPeak) {
  std::vector<mp::Buffer> held;
  for (int i = 0; i < 3; ++i) {
    held.push_back(pool_.acquire());
    ASSERT_TRUE(held.back());
  }
  EXPECT_GE(pool_.stats().peak, 3u);
  held.clear();
  EXPECT_EQ(pool_.stats().in_use, 0u);
  EXPECT_GE(pool_.stats().peak, 3u) << "peak 不该被清零";
}

// ═══════════════════════════════════════════════════════════════
// 阶段 1b：空句柄安全
// 目标：默认构造的句柄，所有接口都不得解引用空池
// ═══════════════════════════════════════════════════════════════

// 10. 空句柄的所有操作都必须安全
TEST(BufferSafetyTest, EmptyHandleIsSafe) {
  mp::Buffer empty;
  EXPECT_FALSE(empty);
  EXPECT_EQ(empty.data(), nullptr);
  EXPECT_EQ(empty.size(), 0u);
  EXPECT_EQ(empty.use_count(), 0u);
  empty.reset();
  mp::Buffer copy = empty;
  EXPECT_FALSE(copy);
}

// ═══════════════════════════════════════════════════════════════
// 阶段 1c：赋值运算符 & 生命周期边界
// 目标：覆盖拷贝/移动赋值的四个象限、reset 在多持有者场景的语义、
//       析构顺序与 refcount 归零归还的正确性
// ═══════════════════════════════════════════════════════════════
class BufferPoolAssignTest : public ::testing::Test {
protected:
  mp::BufferPool pool_{kBlockSize, kBlockCount};
};

// 13. valid = empty（拷贝赋值）：原来唯一持有，other 为空 → 原块被归还，this
// 变空
TEST_F(BufferPoolAssignTest, ValidAssignedFromEmptyCopy) {
  auto a = pool_.acquire(); // a 持有，refcount=1，in_use=1
  ASSERT_TRUE(a);
  mp::Buffer empty; // empty 空句柄
  EXPECT_EQ(pool_.stats().in_use, 1u);

  a = empty; // 覆盖

  EXPECT_FALSE(a) << "赋值后 this 应变成空";
  EXPECT_EQ(pool_.stats().in_use, 0u) << "原块 refcount 归零后应被归还";
}

// 14. valid = empty（移动赋值）：同上，走移动路径
TEST_F(BufferPoolAssignTest, ValidAssignedFromEmptyMove) {
  auto a = pool_.acquire();
  ASSERT_TRUE(a);
  mp::Buffer empty;

  a = std::move(empty);

  EXPECT_FALSE(a);
  EXPECT_EQ(pool_.stats().in_use, 0u);
}

// 15. empty = valid（拷贝赋值）：空 this 接管 other 的块
TEST_F(BufferPoolAssignTest, EmptyAssignedFromValidCopy) {
  mp::Buffer empty;           // 空 this
  auto src = pool_.acquire(); // src 持有，refcount=1
  ASSERT_TRUE(src);

  empty = src; // 接管

  ASSERT_TRUE(empty);
  EXPECT_EQ(empty.data(), src.data());
  EXPECT_EQ(empty.use_count(), 2u); // 双方共享
  EXPECT_EQ(src.use_count(), 2u);
  EXPECT_EQ(pool_.stats().in_use, 1u); // 池视角：还是那一块
}

// 16. empty = valid（移动赋值）：空 this 接管所有权，src 被置空
TEST_F(BufferPoolAssignTest, EmptyAssignedFromValidMove) {
  mp::Buffer empty;
  auto src = pool_.acquire();
  ASSERT_TRUE(src);
  void *p = src.data();

  empty = std::move(src);

  ASSERT_TRUE(empty);
  EXPECT_FALSE(src); // 源被置空
  EXPECT_EQ(empty.data(), p);
  EXPECT_EQ(empty.use_count(), 1u); // 转移后 refcount 不变
  EXPECT_EQ(pool_.stats().in_use, 1u);
}

// 17. valid(块A) = valid(块B)：两个都持有，赋值 → A 被归还（若 refcount
// 归零），接管 B
TEST_F(BufferPoolAssignTest, ValidOverwritesUniqueOwnerCopy) {
  auto a = pool_.acquire(); // 块 5，refcount=1
  auto b = pool_.acquire(); // 块 6，refcount=1
  ASSERT_TRUE(a);
  ASSERT_TRUE(b);
  std::byte *old_data = a.data();
  EXPECT_EQ(pool_.stats().in_use, 2u);

  a = b; // a 原来唯一持有块 5 → 应被归还

  EXPECT_EQ(a.data(), b.data());
  EXPECT_EQ(a.use_count(), 2u);
  EXPECT_EQ(pool_.stats().in_use, 1u) // 块 5 归还，只剩块 6
      << "原块唯一持有，覆盖后应归还";

  // 确认 old_data 那块真的被归还：acquire 应能再次拿到同一块（池里只有它了）
  auto recycled = pool_.acquire();
  ASSERT_TRUE(recycled);
  EXPECT_EQ(recycled.data(), old_data) << "原块被归还后应能被再次 acquire 出来";
}

// 18. valid(块A, refcount=3) = valid(块B)：A 还有其他持有者 → 不归还，只减 1
TEST_F(BufferPoolAssignTest, ValidOverwritesSharedOwner) {
  auto a = pool_.acquire(); // 块 5
  auto a2 = a;              // refcount=2
  auto a3 = a;              // refcount=3
  auto b = pool_.acquire(); // 块 6, refcount=1
  ASSERT_TRUE(a);

  a = b; // a 释放块 5 的一份（refcount 3→2），接管块 6（refcount 1→2）

  // 块 5 还有 a2 和 a3，不应归还
  EXPECT_EQ(pool_.stats().in_use, 2u) << "块 5 还有 2 个持有者，不应归还";
  EXPECT_EQ(a2.use_count(), 2u) << "a 释放后块 5 还有 2 份引用（a2 和 a3）";
  EXPECT_EQ(a.use_count(), 2u) // a 和 b 共享块 6
      << "a 接管块 6 后，与 b 共享 → refcount 应为 2";
  EXPECT_EQ(b.use_count(), 2u);
  EXPECT_EQ(a.data(), b.data());
}

// 19. empty = empty（拷贝赋值）：空 this 赋空 other → 无事发生
TEST_F(BufferPoolAssignTest, EmptyAssignedFromEmptyCopy) {
  mp::Buffer e1;
  mp::Buffer e2;
  e1 = e2; // 不得崩
  EXPECT_FALSE(e1);
  EXPECT_FALSE(e2);
  EXPECT_EQ(pool_.stats().in_use, 0u);
}

// 20. empty = empty（移动赋值）：同上
TEST_F(BufferPoolAssignTest, EmptyAssignedFromEmptyMove) {
  mp::Buffer e1;
  mp::Buffer e2;
  e1 = std::move(e2);
  EXPECT_FALSE(e1);
  EXPECT_FALSE(e2);
}

// 21. 自赋值（拷贝）：a = a 不崩，refcount 不变
TEST_F(BufferPoolAssignTest, SelfAssignmentCopy) {
  auto a = pool_.acquire();
  ASSERT_TRUE(a);
  auto *data = a.data();
  EXPECT_EQ(a.use_count(), 1u);

  a = a; // 自赋值保护：if (this == &other) return *this;

  EXPECT_TRUE(a);
  EXPECT_EQ(a.data(), data);
  EXPECT_EQ(a.use_count(), 1u); // refcount 不变
  EXPECT_EQ(pool_.stats().in_use, 1u);
}

// 22. 自赋值（移动）：同上
TEST_F(BufferPoolAssignTest, SelfAssignmentMove) {
  auto a = pool_.acquire();
  ASSERT_TRUE(a);
  auto *data = a.data();

  a = std::move(a); // 自赋值保护

  EXPECT_TRUE(a);
  EXPECT_EQ(a.data(), data);
  EXPECT_EQ(a.use_count(), 1u);
  EXPECT_EQ(pool_.stats().in_use, 1u);
}

// 23. reset 在多持有者场景：refcount > 1 时 reset，块不应归还
TEST_F(BufferPoolAssignTest, ResetOnSharedBlockDoesNotReturnIt) {
  auto a = pool_.acquire(); // refcount=1
  auto b = a;               // refcount=2
  ASSERT_TRUE(a);
  EXPECT_EQ(pool_.stats().in_use, 1u);

  a.reset(); // a 那份 refcount 减到 1，未归零

  EXPECT_FALSE(a);
  EXPECT_TRUE(b);
  EXPECT_EQ(b.use_count(), 1u);       // 还有 b 持有
  EXPECT_EQ(pool_.stats().in_use, 1u) // 池视角：块仍被借出
      << "多持有者时 reset 不应归还块";

  b.reset(); // 最后一份，归零

  EXPECT_EQ(pool_.stats().in_use, 0u); // 这次才归还
}

// 24. 多持有者逐个析构，顺序不影响最后归还
TEST_F(BufferPoolAssignTest, MultiHolderDestructionOrder) {
  auto a = pool_.acquire();
  auto b = a;
  auto c = b; // refcount=3
  ASSERT_TRUE(a);
  EXPECT_EQ(pool_.stats().in_use, 1u);

  {
    auto d = std::move(c); // c 置空，refcount 仍 3
    EXPECT_FALSE(c);
    EXPECT_EQ(d.use_count(), 3u);
    // d 在作用域内析构 → refcount=2
  }
  // 出作用域：d 析构，refcount=2

  EXPECT_EQ(a.use_count(), 2u);
  EXPECT_EQ(pool_.stats().in_use, 1u); // 仍未归还

  {
    auto e = b; // refcount=3
    EXPECT_EQ(e.use_count(), 3u);
    // e 析构 → refcount=2
  }

  a.reset(); // refcount=1
  EXPECT_EQ(pool_.stats().in_use, 1u);

  b.reset(); // refcount=0，归还
  EXPECT_EQ(pool_.stats().in_use, 0u);
}

// 25. reset 后 acquire 应能复用到同一块（验证块真的归还了）
TEST_F(BufferPoolAssignTest, BlockReusedAfterReset) {
  auto a = pool_.acquire();
  ASSERT_TRUE(a);
  std::byte *data = a.data();
  EXPECT_EQ(pool_.stats().in_use, 1u);
  a.reset();
  EXPECT_EQ(pool_.stats().in_use, 0u);

  // 池里只有这一块空闲（假设未被别人拿走），应该再次拿到同一块
  auto b = pool_.acquire();
  ASSERT_TRUE(b);
  EXPECT_EQ(b.data(), data) << "reset 归还的块应能被后续 acquire 复用";
  EXPECT_EQ(pool_.stats().in_use, 1u);
}

// 26. move 赋值后 other 被置空，other 析构安全
TEST_F(BufferPoolAssignTest, MoveAssignmentSourceDestructsSafely) {
  mp::Buffer a;
  {
    auto b = pool_.acquire();
    ASSERT_TRUE(b);
    a = std::move(b);
    EXPECT_FALSE(b); // b 被置空
    EXPECT_TRUE(a);
  }
  // b 在这里析构——空句柄析构必须安全
  EXPECT_TRUE(a);
  EXPECT_EQ(pool_.stats().in_use, 1u);
}

// ═══════════════════════════════════════════════════════════════
// 阶段 1d：引用计数边界（补充）
// 目标：现有矩阵只测了「唯一持有者 = 空句柄」，漏了「共享持有者 = 空句柄」。
//       补上后能看到一个真实缺陷：赋值自空句柄时旧块只减引用、却不把 this
//       置空。
// ═══════════════════════════════════════════════════════════════

// 27. 共享持有者 = empty（拷贝赋值）：旧块还有其他持有者 → this
// 必须变空，块不得归还
TEST_F(BufferPoolAssignTest, SharedOwnerAssignedFromEmptyCopy) {
  auto a = pool_.acquire();
  auto a2 = a; // 块 A：refcount=2
  ASSERT_TRUE(a);
  ASSERT_EQ(pool_.stats().in_use, 1u);

  mp::Buffer empty;
  a = empty; // 覆盖为空：只是减一份引用，不能保留旧块

  EXPECT_FALSE(a) << "从空句柄拷贝赋值后，this 必须变空";
  EXPECT_TRUE(a2);
  EXPECT_EQ(a2.use_count(), 1u) << "只剩 a2 一个持有者";
  EXPECT_EQ(pool_.stats().in_use, 1u) << "块仍被 a2 持有，不得归还";
}

// 28. 共享持有者 = empty（移动赋值）：同 27，走移动路径
TEST_F(BufferPoolAssignTest, SharedOwnerAssignedFromEmptyMove) {
  auto a = pool_.acquire();
  auto a2 = a; // refcount=2
  ASSERT_TRUE(a);

  mp::Buffer empty;
  a = std::move(empty);

  EXPECT_FALSE(a) << "从空句柄移动赋值后，this 必须变空";
  EXPECT_TRUE(a2);
  EXPECT_EQ(a2.use_count(), 1u);
  EXPECT_EQ(pool_.stats().in_use, 1u);
}

// 29. 幽灵引用不得让在用块被再次发出（27/28 缺陷的后果用例）
//     路径：a 与 a2 共享块 → a = empty 后 a 仍非空且 rc 已被减掉
//          → 再用 a = b 覆盖时会把 a2 的块提前归还 → acquire
//          把它发给别人（同一块双持有）
TEST_F(BufferPoolAssignTest, PhantomReferenceCannotReuseBlockOfLiveHandle) {
  auto a = pool_.acquire();
  auto a2 = a; // 块 A：refcount=2
  ASSERT_TRUE(a);
  std::byte *block = a2.data();

  mp::Buffer empty;
  a = empty; // 若实现正确，a 变空、A 仍归 a2 独占

  auto b = pool_.acquire(); // 块 B
  ASSERT_TRUE(b);
  a = b; // 正确的实现在这里不应归还 A

  EXPECT_EQ(pool_.stats().in_use, 2u)
      << "块 A（a2 持有）与块 B（a/b 持有）都应在用，A 不该被归还";

  auto c = pool_.acquire();
  ASSERT_TRUE(c);
  EXPECT_EQ(pool_.stats().in_use, 3u)
      << "A/B/C 三块都应在用；少一块说明块 A 被提前归还了";
  EXPECT_NE(c.data(), block) << "块 A 仍被 a2 持有，却又被 acquire 发给了 c —— "
                                "同一块双持有（潜在 UAF）";
  EXPECT_EQ(a2.use_count(), 1u) << "块 A 只归 a2 持有；c 拿到的是另一块";
}

// 30. reset 幂等：对已置空的句柄再次 reset 不得二次归还
TEST_F(BufferPoolAssignTest, ResetTwiceIsIdempotent) {
  auto a = pool_.acquire();
  ASSERT_TRUE(a);
  a.reset();
  EXPECT_FALSE(a);
  a.reset(); // 空句柄调用：无副作用
  EXPECT_FALSE(a);
  EXPECT_EQ(pool_.stats().in_use, 0u) << "重复 reset 不得二次归还";
}

// 31. 从空句柄移动构造：源、目标都必须是空，且不碰引用计数
TEST_F(BufferPoolAssignTest, MoveConstructFromEmptyHandle) {
  mp::Buffer empty;
  mp::Buffer moved{std::move(empty)};
  EXPECT_FALSE(moved);
  EXPECT_FALSE(empty);
  EXPECT_EQ(moved.use_count(), 0u);
  EXPECT_EQ(pool_.stats().in_use, 0u);
}

// ═══════════════════════════════════════════════════════════════
// 阶段 2：单线程长跑（慢，默认不跑）
// 目标：100 万次 acquire/reset 记账不漂移
// ═══════════════════════════════════════════════════════════════
class BufferPoolSoakTest : public ::testing::Test {
protected:
  mp::BufferPool pool_{kBlockSize, kBlockCount};
};

// 11. 单线程 100 万次 acquire/release 不泄漏
TEST_F(BufferPoolSoakTest, NoLeakSingleThread) {
  for (int i = 0; i < 1000000; ++i) {
    auto b = pool_.acquire();
    ASSERT_TRUE(b) << "循环第 " << i << " 次失败";
    b.reset();
  }
  EXPECT_EQ(pool_.stats().in_use, 0u);
}

// ═══════════════════════════════════════════════════════════════
// 阶段 3：并发（U4 之后再看）
// 预期：ref_counts_ 还是普通 uint32_t、free_idx_ 还是 vector 时，这里必红
// ═══════════════════════════════════════════════════════════════
class BufferPoolConcurrencyTest : public ::testing::Test {
protected:
  mp::BufferPool pool_{kBlockSize, kBlockCount};
};

// 12. 多线程压测：引用计数必须线程安全
TEST_F(BufferPoolConcurrencyTest, ConcurrentAcquireRelease) {
  constexpr int kThreads = 8;
  constexpr int kIters = 20000;
  std::atomic<int> failures{0};
  std::vector<std::thread> threads;
  threads.reserve(kThreads);
  for (int t = 0; t < kThreads; ++t) {
    threads.emplace_back([this, &failures] {
      for (int i = 0; i < kIters; ++i) {
        auto b = pool_.acquire();
        if (!b) {
          ++failures;
          continue;
        }
        auto c = b; // 竞争点：引用计数 +1
        (void)c;
      }
    });
  }
  for (auto &th : threads)
    th.join();
  EXPECT_EQ(failures.load(), 0) << "并发下 acquire 失败，说明池的记账坏了";
  EXPECT_EQ(pool_.stats().in_use, 0u) << "并发下出现泄漏";
}

// ═══════════════════════════════════════════════════════════════
// 阶段 3b：并发用例补齐
// 原有 12 号只覆盖「每线程各自 acquire + 拷贝」。以下补的是没测到的并发面：
//   - 同一个句柄对象被多线程同时拷贝（原子引用计数的核心场景）
//   - 同一块的多个句柄被多线程同时 reset（最后一次归还只发生一次）
//   - 池接近耗尽时的记账：活句柄数 ≤ capacity，join 后必须归零
//   - 同一块绝不会同时发给两个线程（free list 重复入队 / 双重释放检测）
//   - stats() 与 acquire/release 并发（R7「并发读不崩」，TSan 会判）
//   - 句柄跨线程 move（生产者-消费者交接）
// ═══════════════════════════════════════════════════════════════

// 让所有线程尽量同时起跑，放大竞争窗口
void rendezvous(std::atomic<int> &ready, std::atomic<bool> &go) {
  ready.fetch_add(1);
  while (!go.load(std::memory_order_acquire))
    std::this_thread::yield();
}

// 32. 同一个句柄被 8 个线程同时拷贝：引用数必须精确 +N，析构后精确回落
TEST_F(BufferPoolConcurrencyTest, ConcurrentCopyOfSameHandle) {
  constexpr int kThreads = 8;
  auto shared = pool_.acquire();
  ASSERT_TRUE(shared);

  std::atomic<int> ready{0};
  std::atomic<bool> go{false};
  std::vector<mp::Buffer> copies(static_cast<std::size_t>(kThreads));
  std::vector<std::thread> threads;
  threads.reserve(kThreads);
  for (int t = 0; t < kThreads; ++t) {
    threads.emplace_back([&shared, &copies, &ready, &go, t] {
      rendezvous(ready, go);
      copies[static_cast<std::size_t>(t)] = shared; // 竞争点：并发 fetch_add
    });
  }
  while (ready.load() < kThreads)
    std::this_thread::yield();
  go.store(true, std::memory_order_release);
  for (auto &th : threads)
    th.join();

  EXPECT_EQ(shared.use_count(), static_cast<std::uint32_t>(kThreads) + 1u)
      << "并发拷贝丢了引用计数（少加）或重复加";
  EXPECT_EQ(pool_.stats().in_use, 1u);

  copies.clear();
  EXPECT_EQ(shared.use_count(), 1u) << "副本析构后应精确回落";
  shared.reset();
  EXPECT_EQ(pool_.stats().in_use, 0u);
}

// 33. 同一块的 N 个句柄被 N 个线程同时 reset：只有最后一次归还，之后池完整可用
TEST_F(BufferPoolConcurrencyTest, ConcurrentResetOfSharedHandles) {
  constexpr int kThreads = 8;
  auto shared = pool_.acquire();
  ASSERT_TRUE(shared);

  std::vector<mp::Buffer> handles(static_cast<std::size_t>(kThreads), shared);
  ASSERT_EQ(shared.use_count(), static_cast<std::uint32_t>(kThreads) + 1u);

  std::atomic<int> ready{0};
  std::atomic<bool> go{false};
  std::vector<std::thread> threads;
  threads.reserve(kThreads);
  for (int t = 0; t < kThreads; ++t) {
    threads.emplace_back([&handles, &ready, &go, t] {
      auto h =
          std::move(handles[static_cast<std::size_t>(t)]); // 每线程独占一个句柄
      rendezvous(ready, go);
      h.reset();
    });
  }
  while (ready.load() < kThreads)
    std::this_thread::yield();
  go.store(true, std::memory_order_release);
  for (auto &th : threads)
    th.join();

  EXPECT_EQ(shared.use_count(), 1u) << "少减或多减都说明引用计数有竞态";
  shared.reset();
  EXPECT_EQ(pool_.stats().in_use, 0u) << "最后一次 reset 应恰好归还一次";

  // 若发生 double release，free list 里会有重复索引，下面会拿到别名或超出容量
  std::vector<mp::Buffer> all;
  all.reserve(kBlockCount);
  for (std::uint32_t i = 0; i < kBlockCount; ++i) {
    all.push_back(pool_.acquire());
    ASSERT_TRUE(all.back())
        << "第 " << i << " 次 acquire 失败，free list 可能被破坏";
  }
  EXPECT_EQ(pool_.stats().in_use, kBlockCount);
  all.clear();
  EXPECT_EQ(pool_.stats().in_use, 0u);
}

// 34. 每线程跑完整生命周期（拷贝/move/赋值/reset）多轮，join 后零泄漏
TEST_F(BufferPoolConcurrencyTest, ConcurrentHandleLifecycleChurn) {
  constexpr int kThreads = 8;
  constexpr int kIters = 5000;
  std::atomic<int> failures{0};
  std::vector<std::thread> threads;
  threads.reserve(kThreads);
  for (int t = 0; t < kThreads; ++t) {
    threads.emplace_back([this, &failures] {
      for (int i = 0; i < kIters; ++i) {
        auto a = pool_.acquire();
        if (!a) {
          ++failures;
          continue;
        }
        auto b = a;            // rc=2
        auto c = std::move(b); // rc=2，b 空
        mp::Buffer d;
        d = c;      // rc=3
        d.reset();  // rc=2
        auto e = a; // rc=3
        e.reset();  // rc=2
        a.reset();  // rc=1
        c.reset();  // rc=0 → 归还
      }
    });
  }
  for (auto &th : threads)
    th.join();

  EXPECT_EQ(failures.load(), 0) << "64 块 / 8 线程不应出现 acquire 失败";
  EXPECT_EQ(pool_.stats().in_use, 0u) << "并发生命周期操作后出现泄漏";
}

// 35. 同一块绝不能同时发给两个线程（检测 free list 重复入队 / 提前归还）
TEST_F(BufferPoolConcurrencyTest, NoTwoLiveHandlesShareABlockUnderContention) {
  constexpr int kThreads = 8;
  constexpr int kIters = 5000;

  // 先单线程把所有块借一遍，建立 data 指针 → 块序号 的映射
  std::unordered_map<const std::byte *, std::uint32_t> ordinal;
  {
    std::vector<mp::Buffer> all;
    all.reserve(kBlockCount);
    for (std::uint32_t i = 0; i < kBlockCount; ++i) {
      all.push_back(pool_.acquire());
      ASSERT_TRUE(all.back());
      ASSERT_TRUE(ordinal.emplace(all.back().data(), i).second)
          << "初始分配的块就有别名";
    }
  }
  ASSERT_EQ(ordinal.size(), kBlockCount);
  EXPECT_EQ(pool_.stats().in_use, 0u);

  // 每个块一个 owner 槽位：抢不到说明同一块被发给了两个线程
  std::vector<std::atomic<int>> owner(kBlockCount);
  for (auto &o : owner)
    o.store(-1);
  std::atomic<int> duplicates{0};
  std::atomic<int> failures{0};

  std::vector<std::thread> threads;
  threads.reserve(kThreads);
  for (int t = 0; t < kThreads; ++t) {
    threads.emplace_back([&, t] {
      for (int i = 0; i < kIters; ++i) {
        auto b = pool_.acquire();
        if (!b) {
          ++failures; // 8 线程 < 64 块，正常不应失败
          continue;
        }
        auto it = ordinal.find(b.data());
        if (it == ordinal.end()) {
          ++duplicates; // 发出了一个不属于池的地址
          continue;
        }
        int expected = -1;
        if (!owner[it->second].compare_exchange_strong(expected, t)) {
          ++duplicates; // 该块已被别的线程持有 → 双持有
        }
        std::this_thread::yield();
        owner[it->second].store(-1);
      } // b 在这里析构并归还
    });
  }
  for (auto &th : threads)
    th.join();

  EXPECT_EQ(failures.load(), 0);
  EXPECT_EQ(duplicates.load(), 0)
      << "同一块同时被两个线程持有：free list 被破坏";
  EXPECT_EQ(pool_.stats().in_use, 0u);
}

// 36. 线程数 > 块数时：活句柄峰值不超过 capacity，耗尽返回空句柄，join 后归零
TEST_F(BufferPoolConcurrencyTest, ConcurrentExhaustionAccounting) {
  constexpr int kThreads = 32;
  constexpr int kIters = 2000;
  std::atomic<int> live{0};
  std::atomic<int> max_live{0};
  std::atomic<int> ok{0};
  std::atomic<int> rejected{0};

  std::vector<std::thread> threads;
  threads.reserve(kThreads);
  for (int t = 0; t < kThreads; ++t) {
    threads.emplace_back([&] {
      for (int i = 0; i < kIters; ++i) {
        auto b = pool_.acquire();
        if (!b) {
          ++rejected;
          std::this_thread::yield();
          continue;
        }
        ++ok;
        const int now = live.fetch_add(1) + 1;
        int prev = max_live.load();
        while (now > prev && !max_live.compare_exchange_weak(prev, now)) {
        }
        std::this_thread::yield();
        live.fetch_sub(1);
      }
    });
  }
  for (auto &th : threads)
    th.join();

  EXPECT_GT(ok.load(), 0);
  EXPECT_LE(max_live.load(), static_cast<int>(kBlockCount))
      << "同时借出的块数超过了容量，说明同一个块被重复发出";
  EXPECT_EQ(live.load(), 0);
  EXPECT_EQ(pool_.stats().in_use, 0u) << "耗尽压测后出现泄漏/负计数";
  EXPECT_GE(pool_.stats().peak, 1u);
  EXPECT_LE(pool_.stats().peak, kBlockCount);
}

// 37. stats() 与 acquire/release 并发（R7：并发读不得产生数据竞争）
//     Debug 构建下可能「碰巧」过；TSan 构建必须干净，否则说明 stats_
//     需要加锁/原子化。
TEST_F(BufferPoolConcurrencyTest, ConcurrentStatsReadDuringChurn) {
  constexpr int kThreads = 4;
  std::atomic<bool> stop{false};
  std::vector<std::thread> churn;
  churn.reserve(kThreads);
  for (int t = 0; t < kThreads; ++t) {
    churn.emplace_back([this, &stop] {
      while (!stop.load(std::memory_order_relaxed)) {
        auto b = pool_.acquire();
        if (b) {
          auto c = b;
          (void)c;
        }
      }
    });
  }

  for (int i = 0; i < 5000; ++i) {
    const auto s =
        pool_.stats(); // 竞争点：与 acquire/release 里的 stats_ 写入并发
    EXPECT_EQ(s.capacity, kBlockCount);
    EXPECT_LE(s.in_use, kBlockCount) << "读到了越界的 in_use";
    EXPECT_LE(s.peak, kBlockCount);
  }

  stop.store(true, std::memory_order_relaxed);
  for (auto &th : churn)
    th.join();
  EXPECT_EQ(pool_.stats().in_use, 0u);
}

// 38. 句柄跨线程 move：生产者 acquire 后 move 进队列，消费者拿到的 use_count
// 必须是 1
TEST_F(BufferPoolConcurrencyTest, ProducerConsumerMoveAcrossThreads) {
  constexpr int kItems = 5000;
  std::mutex mu;
  std::vector<mp::Buffer> queue;
  std::atomic<int> consumed{0};
  std::atomic<int> bad_count{0};

  std::thread consumer([&] {
    while (consumed.load() < kItems) {
      mp::Buffer b;
      {
        std::lock_guard<std::mutex> lk(mu);
        if (!queue.empty()) {
          b = std::move(queue.back());
          queue.pop_back();
        }
      }
      if (!b) {
        std::this_thread::yield();
        continue;
      }
      if (b.use_count() != 1u)
        ++bad_count; // 交接后应只有消费者持有
      b.reset();
      consumed.fetch_add(1);
    }
  });

  std::thread producer([&] {
    for (int i = 0; i < kItems; ++i) {
      mp::Buffer b = pool_.acquire();
      while (!b) { // 池满：等消费者归还
        std::this_thread::yield();
        b = pool_.acquire();
      }
      {
        std::lock_guard<std::mutex> lk(mu);
        queue.push_back(std::move(b));
      }
    }
  });

  producer.join();
  consumer.join();

  EXPECT_EQ(bad_count.load(), 0)
      << "move 跨线程后引用计数不为 1，说明 move 实现有问题";
  EXPECT_EQ(consumed.load(), kItems);
  EXPECT_EQ(pool_.stats().in_use, 0u) << "生产者-消费者跑完仍有块未归还";
}

// ═══════════════════════════════════════════════════════════════
// 阶段 3c：全局池并发
// ═══════════════════════════════════════════════════════════════

// 39. global_pool() 的静态局部初始化是线程安全的（C++11 magic
// static），且是同一实例
TEST(BufferPoolGlobalTest, ConcurrentAccessReturnsSameInstance) {
  constexpr int kThreads = 8;
  mp::BufferPool *expected = &mp::global_pool();
  std::atomic<int> mismatched{0};
  std::atomic<int> failures{0};

  std::vector<std::thread> threads;
  threads.reserve(kThreads);
  for (int t = 0; t < kThreads; ++t) {
    threads.emplace_back([expected, &mismatched, &failures] {
      if (&mp::global_pool() != expected)
        ++mismatched;
      for (int i = 0; i < 1000; ++i) {
        auto b = mp::global_pool().acquire();
        if (!b) {
          ++failures;
          continue;
        }
        auto c = b; // 拷贝也要并发安全
        (void)c;
      }
    });
  }
  for (auto &th : threads)
    th.join();

  EXPECT_EQ(mismatched.load(), 0) << "多线程拿到了不同的全局池实例";
  EXPECT_EQ(failures.load(), 0) << "256 块 / 8 线程不应 acquire 失败";
  EXPECT_EQ(mp::global_pool().stats().in_use, 0u);
}

} // namespace
