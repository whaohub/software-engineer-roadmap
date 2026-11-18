// Demo 3 · 引入 Buffer（资源句柄）
// 目标：理解为什么裸指针不够，为什么需要"句柄"这一层封装
//
// Demo 1/2 裸指针没解决的三个问题：
//   1. 归属猜测：release(void*) 假设指针属于这个池，多池就炸
//   2. 双重释放 / 悬垂：裸指针不知道"自己已经被还过了"
//   3. 忘记归还：泄漏（Demo 4 引用计数解决，Demo 3 先铺自动归还的结构）
//
// 这一版 Buffer 不支持复制（复制 = 双重析构 = 双重归还），Demo 4 加引用计数。
//
// 编译：g++ -std=c++17 -Wall -Wextra demo/mem_pool_demo3.cpp -o /tmp/mpd3 &&
// /tmp/mpd3

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <utility>
#include <vector>

// 前向声明：Buffer 要能访问 Pool 的 release（friend）
class FixedPool;

/// 资源句柄：持有「块属于哪个池 + 块的索引」，自动归还。
/// 禁止拷贝（Demo 4 加引用计数才放开拷贝）。
class Buffer {
public:
  Buffer() noexcept = default; // 空句柄：pool=nullptr, index=0
  ~Buffer();                   // 析构自动归还

  // 禁止拷贝（两个 Buffer 指向同一块 → 两个析构 → 双重归还）
  Buffer(const Buffer &) = delete;
  Buffer &operator=(const Buffer &) = delete;

  // 允许移动（转移所有权，源置空，不归还）
  Buffer(Buffer &&other) noexcept;
  Buffer &operator=(Buffer &&other) noexcept;

  // 访问接口
  std::byte *data() const noexcept;
  std::size_t size() const noexcept;
  explicit operator bool() const noexcept { return pool_ != nullptr; }

  // 手动归还（等价于析构，后置空）
  void reset() noexcept;

private:
  friend class FixedPool;

  // 只有 Pool 能构造 Buffer（外部不能随便 new 一个 Buffer{xxx, 5}）
  Buffer(FixedPool *pool, std::uint32_t index) noexcept;

  FixedPool *pool_ = nullptr;
  std::uint32_t index_ = 0;
};

/// 固定块池（沿用 Demo 2 的 free list 版本，但 acquire 返回 Buffer）
class FixedPool {
public:
  FixedPool(std::size_t chunk_size, std::uint32_t chunk_count);
  ~FixedPool();

  FixedPool(const FixedPool &) = delete;
  FixedPool &operator=(const FixedPool &) = delete;

  // 借一块，返回 Buffer 句柄；池耗尽返回空 Buffer
  Buffer acquire();

  std::size_t chunk_size() const { return chunk_size_; }
  std::uint32_t chunk_count() const { return chunk_count_; }
  std::uint32_t in_use() const { return in_use_; }

private:
  friend class Buffer;
  // Buffer 析构时调这个归还
  void release(std::uint32_t index) noexcept;

  std::size_t chunk_size_{0};
  std::uint32_t chunk_count_{0};
  std::byte *memory_ = nullptr;
  std::vector<std::uint32_t> free_idx_;
  std::uint32_t in_use_ = 0;
};

// ═══════════════════════════════════════════════════════════════════════
// 实现区 · 全是 TODO 桩，等你自己填
// ═══════════════════════════════════════════════════════════════════════

// ── Buffer ──────────────────────────────────────────────────
Buffer::Buffer(FixedPool *pool, std::uint32_t index) noexcept
    : pool_(pool), index_(index) {}

Buffer::~Buffer() {
  if (pool_) {
    pool_->release(index_);
    pool_ = nullptr;
  }
}

Buffer::Buffer(Buffer &&other) noexcept
    : pool_(other.pool_), index_(other.index_) {
  other.pool_ = nullptr;
  other.index_ = 0;
}

Buffer &Buffer::operator=(Buffer &&other) noexcept {
  // TODO(Demo3):
  //   1. 先释放自己持有的块（如果有的话）
  //   2. 接管 other 的 pool_/index_
  //   3. other 置空
  if (pool_)
    pool_->release(index_);

  pool_ = other.pool_;
  index_ = other.index_;
  other.pool_ = nullptr;
  other.index_ = 0;
  return *this;
}

std::byte *Buffer::data() const noexcept {
  if (!pool_)
    return nullptr;
  return pool_->memory_ + index_ * pool_->chunk_size_;
}

std::size_t Buffer::size() const noexcept {
  return pool_ ? pool_->chunk_size_ : 0;
}

void Buffer::reset() noexcept {
  if (pool_) {
    pool_->release(index_);
    pool_ = nullptr;
  }
}

// ── FixedPool ────────────────────────────────────────────────
FixedPool::FixedPool(std::size_t chunk_size, std::uint32_t chunk_count)
    : chunk_size_(chunk_size), chunk_count_(chunk_count) {
  memory_ = new std::byte[chunk_size_ * chunk_count_];
  free_idx_.reserve(chunk_count_);
  for (std::uint32_t i = 0; i < chunk_count_; ++i) {
    free_idx_.push_back(i);
  }
  std::printf("[构造] FixedPool: chunk_size=%zu, chunk_count=%u, arena=%p\n",
              chunk_size, chunk_count, (void *)memory_);
}

FixedPool::~FixedPool() {
  delete[] memory_;
  assert(in_use_ == 0 && "还有 Buffer 活着就析构 Pool！");
}

Buffer FixedPool::acquire() {
  if (!free_idx_.empty()) {
    auto idx = free_idx_.back();
    free_idx_.pop_back();
    in_use_++;
    return Buffer{this, idx};
  }
  return Buffer{};
}

void FixedPool::release(std::uint32_t index) noexcept {
  free_idx_.push_back(index);
  in_use_--;
}

// ═══════════════════════════════════════════════════════════════════════
// main · 测试框架
// ═══════════════════════════════════════════════════════════════════════

int main() {
  std::printf("╔══════════════════════════════════════════════════╗\n");
  std::printf("║  Demo3 · Buffer 句柄（归属 + 自动归还 + 移动）    ║\n");
  std::printf("╚══════════════════════════════════════════════════╝\n");

  // ── 测试 1：Buffer 析构自动归还（对比 Demo 2 裸指针必须手动 release）──
  std::printf("\n[测试 1] 自动归还（离开作用域 = 自动 release）\n");
  {
    FixedPool pool(256, 4);
    std::printf("  acquire 前 in_use=0\n");
    {
      Buffer b = pool.acquire();
      assert(b);
      std::printf("  acquire 后 in_use=%u\n", pool.in_use());
      // b 离开作用域应该自动归还
    }
    std::printf("  b 析构后 in_use=%u  （期望 0）\n", pool.in_use());
    assert(pool.in_use() == 0);
    std::printf("  ✅ Buffer 析构自动归还\n");
  } // Pool 析构时 in_use_ 必须是 0（FixedPool::~FixedPool() 的 assert 会验证）

  // ── 测试 2：空句柄安全（二次析构安全） ────────────────────────
  std::printf("\n[测试 2] 空句柄 + reset 手动归还\n");
  FixedPool pool2(256, 2);
  Buffer empty;
  assert(!empty);
  std::printf("  空 Buffer 构造 OK，bool()=false ✅\n");

  Buffer b1 = pool2.acquire();
  assert(b1);
  std::printf("  借一块，in_use=%u\n", pool2.in_use());
  b1.reset();  // 手动归还
  assert(!b1); // reset 后置空
  std::printf("  reset 后置空 ✅\n");
  b1.reset(); // 再 reset 一次也不该崩
  std::printf("  空 Buffer 再 reset 也安全 ✅\n");
  assert(pool2.in_use() == 0);

  // ── 测试 3：移动语义（转移所有权，源置空） ─────────────────────
  std::printf("\n[测试 3] 移动语义（源置空，不双重归还）\n");
  {
    FixedPool pool3(256, 3);
    Buffer src = pool3.acquire();
    std::printf("  src 初始 in_use=%u\n", pool3.in_use());
    assert(src);
    std::byte *data_ptr = src.data();
    Buffer dst = std::move(src);
    std::printf("  std::move(src) → src 变空，dst 接手\n");
    assert(!src);                   // 源被置空
    assert(dst);                    // 目标有效
    assert(dst.data() == data_ptr); // 指向同一块
    std::printf("  src 变空 ✅，dst 接手 ✅\n");
    std::printf("  现在有 src（空）和 dst（有效），dst 析构应该只归还一次\n");
  } // dst 析构归还；src 是空的，析构不做事
  std::printf("  全部离开作用域，应该不崩 ✅\n");

  // ── 测试 4：多池归属（Demo 2 做不到的事） ────────────────────
  std::printf(
      "\n[测试 4] 多池归属（Buffer 自带 Pool 指针，不会 release 错池）\n");
  FixedPool pool_a(128, 2);
  FixedPool pool_b(256, 2);

  Buffer from_a = pool_a.acquire();
  Buffer from_b = pool_b.acquire();
  assert(from_a);
  assert(from_b);
  std::printf("  pool_a 借的块 size=%zu（应该 128）\n", from_a.size());
  std::printf("  pool_b 借的块 size=%zu（应该 256）\n", from_b.size());
  assert(from_a.size() == 128);
  assert(from_b.size() == 256);
  std::printf("  ✅ Buffer 知道自己属于哪个 Pool（size 不同说明归属正确）\n");
  // from_a 析构时自动归还 pool_a，from_b 归还 pool_b，不会交叉

  // ── 测试 5：禁止拷贝（编译期验证，注释掉会编译不过） ─────────
  std::printf("\n[测试 5] 禁止拷贝（编译器保护）\n");
  Buffer b_check = pool_a.acquire();
  // Buffer b_copy = b_check;  // 取消注释 → 编译错误（拷贝构造已 delete）
  std::printf("  ✅ Buffer 禁止拷贝，防止双重归还（注释掉上面那行验证）\n");

  std::printf("\n🎉 Demo3 框架就绪，等你填实现\n");
  return 0;
}
