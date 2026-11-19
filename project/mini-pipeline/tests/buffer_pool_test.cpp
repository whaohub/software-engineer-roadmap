// mini-pipeline · M1 验收用例
// 规则：这些用例不许改实现语义来「迁就」你的实现；如果语义要变，先改 TASK.md 并说明理由。
//
//   阶段 1   基础流程（单线程）          BufferPoolTest
//   阶段 1b  空句柄安全                  BufferSafetyTest
//   阶段 2   单线程长跑（慢，100 万次）   BufferPoolSoakTest
//   阶段 3   并发        BufferPoolConcurrencyTest
//
// 日常开发只跑基础流程：
//   ./buffer_pool_test --gtest_filter='BufferPoolTest.*:BufferSafetyTest.*'
// 阶段 2 单独跑：
//   ./buffer_pool_test --gtest_filter='BufferPoolSoakTest.*'
// 阶段 3（等 U4 把计数改成原子、free list 加锁后再看）：
//   ./buffer_pool_test --gtest_filter='BufferPoolConcurrencyTest.*'
#include <gtest/gtest.h>

#include <atomic>
#include <cstddef>
#include <cstring>
#include <thread>
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
  EXPECT_LE(sizeof(mp::Buffer), 16u) << "句柄膨胀了，检查控制块是不是放在了堆上";
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
  void* p = a.data();
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
    ASSERT_TRUE(held.back()) << "第 " << i << " 次 acquire 就失败了，容量没算对";
  }
  bool defined = false;
#if defined(__cpp_exceptions)
  try {
    auto extra = pool_.acquire();
    defined = !extra;  // 语义 A：返回空句柄
  } catch (const std::bad_alloc&) {
    defined = true;    // 语义 B：抛 bad_alloc
  }
#else
  auto extra = pool_.acquire();
  defined = !extra;
#endif
  EXPECT_TRUE(defined) << "池耗尽后既没返回空句柄也没抛异常 —— 这会让调用方拿到野指针";
}

// 7. 不同块不能重叠
TEST_F(BufferPoolTest, BlocksDoNotAlias) {
  auto a = pool_.acquire();
  auto b = pool_.acquire();
  ASSERT_TRUE(a);
  ASSERT_TRUE(b);
  const auto* pa = reinterpret_cast<const std::byte*>(a.data());
  const auto* pb = reinterpret_cast<const std::byte*>(b.data());
  ASSERT_NE(pa, pb);
  const auto diff = pa < pb ? pb - pa : pa - pb;
  EXPECT_GE(static_cast<std::size_t>(diff), kBlockSize) << "两块内存重叠了";
}

// 8. 块起始地址要满足最大对齐
TEST_F(BufferPoolTest, DataIsAligned) {
  auto a = pool_.acquire();
  ASSERT_TRUE(a);
  EXPECT_EQ(reinterpret_cast<std::uintptr_t>(a.data()) % alignof(std::max_align_t), 0u);
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

// 13. valid = empty（拷贝赋值）：原来唯一持有，other 为空 → 原块被归还，this 变空
TEST_F(BufferPoolAssignTest, ValidAssignedFromEmptyCopy) {
  auto a = pool_.acquire();            // a 持有，refcount=1，in_use=1
  ASSERT_TRUE(a);
  mp::Buffer empty;                    // empty 空句柄
  EXPECT_EQ(pool_.stats().in_use, 1u);

  a = empty;                            // 覆盖

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
  mp::Buffer empty;                    // 空 this
  auto src = pool_.acquire();           // src 持有，refcount=1
  ASSERT_TRUE(src);

  empty = src;                          // 接管

  ASSERT_TRUE(empty);
  EXPECT_EQ(empty.data(), src.data());
  EXPECT_EQ(empty.use_count(), 2u);    // 双方共享
  EXPECT_EQ(src.use_count(), 2u);
  EXPECT_EQ(pool_.stats().in_use, 1u); // 池视角：还是那一块
}

// 16. empty = valid（移动赋值）：空 this 接管所有权，src 被置空
TEST_F(BufferPoolAssignTest, EmptyAssignedFromValidMove) {
  mp::Buffer empty;
  auto src = pool_.acquire();
  ASSERT_TRUE(src);
  void* p = src.data();

  empty = std::move(src);

  ASSERT_TRUE(empty);
  EXPECT_FALSE(src);                   // 源被置空
  EXPECT_EQ(empty.data(), p);
  EXPECT_EQ(empty.use_count(), 1u);    // 转移后 refcount 不变
  EXPECT_EQ(pool_.stats().in_use, 1u);
}

// 17. valid(块A) = valid(块B)：两个都持有，赋值 → A 被归还（若 refcount 归零），接管 B
TEST_F(BufferPoolAssignTest, ValidOverwritesUniqueOwnerCopy) {
  auto a = pool_.acquire();            // 块 5，refcount=1
  auto b = pool_.acquire();            // 块 6，refcount=1
  ASSERT_TRUE(a);
  ASSERT_TRUE(b);
  std::byte* old_data = a.data();
  EXPECT_EQ(pool_.stats().in_use, 2u);

  a = b;                                // a 原来唯一持有块 5 → 应被归还

  EXPECT_EQ(a.data(), b.data());
  EXPECT_EQ(a.use_count(), 2u);
  EXPECT_EQ(pool_.stats().in_use, 1u)  // 块 5 归还，只剩块 6
      << "原块唯一持有，覆盖后应归还";

  // 确认 old_data 那块真的被归还：acquire 应能再次拿到同一块（池里只有它了）
  auto recycled = pool_.acquire();
  ASSERT_TRUE(recycled);
  EXPECT_EQ(recycled.data(), old_data)
      << "原块被归还后应能被再次 acquire 出来";
}

// 18. valid(块A, refcount=3) = valid(块B)：A 还有其他持有者 → 不归还，只减 1
TEST_F(BufferPoolAssignTest, ValidOverwritesSharedOwner) {
  auto a = pool_.acquire();            // 块 5
  auto a2 = a;                          // refcount=2
  auto a3 = a;                          // refcount=3
  auto b = pool_.acquire();             // 块 6, refcount=1
  ASSERT_TRUE(a);

  a = b;                                // a 释放块 5 的一份（refcount 3→2），接管块 6（refcount 1→2）

  // 块 5 还有 a2 和 a3，不应归还
  EXPECT_EQ(pool_.stats().in_use, 2u)
      << "块 5 还有 2 个持有者，不应归还";
  EXPECT_EQ(a2.use_count(), 2u)
      << "a 释放后块 5 还有 2 份引用（a2 和 a3）";
  EXPECT_EQ(a.use_count(), 2u)          // a 和 b 共享块 6
      << "a 接管块 6 后，与 b 共享 → refcount 应为 2";
  EXPECT_EQ(b.use_count(), 2u);
  EXPECT_EQ(a.data(), b.data());
}

// 19. empty = empty（拷贝赋值）：空 this 赋空 other → 无事发生
TEST_F(BufferPoolAssignTest, EmptyAssignedFromEmptyCopy) {
  mp::Buffer e1;
  mp::Buffer e2;
  e1 = e2;  // 不得崩
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
  auto* data = a.data();
  EXPECT_EQ(a.use_count(), 1u);

  a = a;  // 自赋值保护：if (this == &other) return *this;

  EXPECT_TRUE(a);
  EXPECT_EQ(a.data(), data);
  EXPECT_EQ(a.use_count(), 1u);  // refcount 不变
  EXPECT_EQ(pool_.stats().in_use, 1u);
}

// 22. 自赋值（移动）：同上
TEST_F(BufferPoolAssignTest, SelfAssignmentMove) {
  auto a = pool_.acquire();
  ASSERT_TRUE(a);
  auto* data = a.data();

  a = std::move(a);  // 自赋值保护

  EXPECT_TRUE(a);
  EXPECT_EQ(a.data(), data);
  EXPECT_EQ(a.use_count(), 1u);
  EXPECT_EQ(pool_.stats().in_use, 1u);
}

// 23. reset 在多持有者场景：refcount > 1 时 reset，块不应归还
TEST_F(BufferPoolAssignTest, ResetOnSharedBlockDoesNotReturnIt) {
  auto a = pool_.acquire();            // refcount=1
  auto b = a;                           // refcount=2
  ASSERT_TRUE(a);
  EXPECT_EQ(pool_.stats().in_use, 1u);

  a.reset();                             // a 那份 refcount 减到 1，未归零

  EXPECT_FALSE(a);
  EXPECT_TRUE(b);
  EXPECT_EQ(b.use_count(), 1u);         // 还有 b 持有
  EXPECT_EQ(pool_.stats().in_use, 1u)   // 池视角：块仍被借出
      << "多持有者时 reset 不应归还块";

  b.reset();                             // 最后一份，归零

  EXPECT_EQ(pool_.stats().in_use, 0u);  // 这次才归还
}

// 24. 多持有者逐个析构，顺序不影响最后归还
TEST_F(BufferPoolAssignTest, MultiHolderDestructionOrder) {
  auto a = pool_.acquire();
  auto b = a;
  auto c = b;                           // refcount=3
  ASSERT_TRUE(a);
  EXPECT_EQ(pool_.stats().in_use, 1u);

  {
    auto d = std::move(c);               // c 置空，refcount 仍 3
    EXPECT_FALSE(c);
    EXPECT_EQ(d.use_count(), 3u);
    // d 在作用域内析构 → refcount=2
  }
  // 出作用域：d 析构，refcount=2

  EXPECT_EQ(a.use_count(), 2u);
  EXPECT_EQ(pool_.stats().in_use, 1u);  // 仍未归还

  {
    auto e = b;                          // refcount=3
    EXPECT_EQ(e.use_count(), 3u);
    // e 析构 → refcount=2
  }

  a.reset();                             // refcount=1
  EXPECT_EQ(pool_.stats().in_use, 1u);

  b.reset();                             // refcount=0，归还
  EXPECT_EQ(pool_.stats().in_use, 0u);
}

// 25. reset 后 acquire 应能复用到同一块（验证块真的归还了）
TEST_F(BufferPoolAssignTest, BlockReusedAfterReset) {
  auto a = pool_.acquire();
  ASSERT_TRUE(a);
  std::byte* data = a.data();
  EXPECT_EQ(pool_.stats().in_use, 1u);
  a.reset();
  EXPECT_EQ(pool_.stats().in_use, 0u);

  // 池里只有这一块空闲（假设未被别人拿走），应该再次拿到同一块
  auto b = pool_.acquire();
  ASSERT_TRUE(b);
  EXPECT_EQ(b.data(), data)
      << "reset 归还的块应能被后续 acquire 复用";
  EXPECT_EQ(pool_.stats().in_use, 1u);
}

// 26. move 赋值后 other 被置空，other 析构安全
TEST_F(BufferPoolAssignTest, MoveAssignmentSourceDestructsSafely) {
  mp::Buffer a;
  {
    auto b = pool_.acquire();
    ASSERT_TRUE(b);
    a = std::move(b);
    EXPECT_FALSE(b);                    // b 被置空
    EXPECT_TRUE(a);
  }
  // b 在这里析构——空句柄析构必须安全
  EXPECT_TRUE(a);
  EXPECT_EQ(pool_.stats().in_use, 1u);
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
        auto c = b;  // 竞争点：引用计数 +1
        (void)c;
      }
    });
  }
  for (auto& th : threads) th.join();
  EXPECT_EQ(failures.load(), 0) << "并发下 acquire 失败，说明池的记账坏了";
  EXPECT_EQ(pool_.stats().in_use, 0u) << "并发下出现泄漏";
}

}  // namespace
