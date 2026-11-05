// mini-pipeline · M1 验收用例
// 规则：这些用例不许改实现语义来「迁就」你的实现；如果语义要变，先改 TASK.md 并说明理由。
//
// 现在它们全红，这是设计好的起点。
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

// 7. 单线程 100 万次 acquire/release 不泄漏
TEST_F(BufferPoolTest, NoLeakSingleThread) {
  for (int i = 0; i < 1000000; ++i) {
    auto b = pool_.acquire();
    ASSERT_TRUE(b) << "循环第 " << i << " 次失败";
    b.reset();
  }
  EXPECT_EQ(pool_.stats().in_use, 0u);
}

// 8. 多线程压测：引用计数必须线程安全
TEST_F(BufferPoolTest, ConcurrentAcquireRelease) {
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

// 9. 不同块不能重叠
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

// 10. 块起始地址要满足最大对齐
TEST_F(BufferPoolTest, DataIsAligned) {
  auto a = pool_.acquire();
  ASSERT_TRUE(a);
  EXPECT_EQ(reinterpret_cast<std::uintptr_t>(a.data()) % alignof(std::max_align_t), 0u);
}

// 11. 空句柄的所有操作都必须安全
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

// 12. 峰值统计正确
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

}  // namespace
