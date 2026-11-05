// mini-pipeline · M1 实现（当前全部是桩，测试全红——这是设计好的起点）
// 完成标准见 TASK.md：make test / test-asan / test-tsan 三条全绿。
//
// 动手顺序建议（按"最快看到红转绿"排）：
//   1. 池的底层内存 + free list（先单线程跑通 AcquireBasic / ResetReturns）
//   2. 句柄的 copy/move 与 use_count（让 CopySharesBlock / MoveLeavesEmpty 绿）
//   3. 把计数改成原子、加多线程压测（ConcurrentStress，TSan 会挑毛病）
//   4. 句柄压到 <=16 字节（HandleSize），并保证 data() 对齐（Alignment）
#include "mini_pipeline/buffer_pool.hpp"

namespace mp {

// ============================ Buffer ============================

Buffer::Buffer() noexcept = default;
Buffer::Buffer(const Buffer& other) noexcept : pool_or_block_(other.pool_or_block_), index_(other.index_) {
  // TODO(M1): 引用计数 +1（线程安全）
}
Buffer& Buffer::operator=(const Buffer& other) noexcept {
  if (this == &other) return *this;
  // TODO(M1): 先释放自己持有的块，再接管 other 的块并 +1
  pool_or_block_ = other.pool_or_block_;
  index_ = other.index_;
  return *this;
}
Buffer::Buffer(Buffer&& other) noexcept : pool_or_block_(other.pool_or_block_), index_(other.index_) {
  // TODO(M1): 转移所有权，把 other 置空（不改变引用计数）
  other.pool_or_block_ = nullptr;
  other.index_ = 0;
}
Buffer& Buffer::operator=(Buffer&& other) noexcept {
  if (this == &other) return *this;
  // TODO(M1): 释放自己 -> 接管 other -> other 置空
  pool_or_block_ = other.pool_or_block_;
  index_ = other.index_;
  other.pool_or_block_ = nullptr;
  other.index_ = 0;
  return *this;
}
Buffer::~Buffer() {
  // TODO(M1): 引用计数 -1，减到 0 时归还块
}

std::byte* Buffer::data() const noexcept {
  return nullptr;  // TODO(M1)
}
std::size_t Buffer::size() const noexcept {
  return 0;  // TODO(M1)
}
std::uint32_t Buffer::use_count() const noexcept {
  return 0;  // TODO(M1)
}
void Buffer::reset() noexcept {
  // TODO(M1): 与析构同样的释放逻辑（可复用）
}
Buffer::operator bool() const noexcept { return pool_or_block_ != nullptr; }

// ========================== BufferPool ==========================

BufferPool::BufferPool(std::size_t block_size, std::uint32_t block_count)
    : block_size_(block_size), block_count_(block_count) {
  // TODO(M1): 一次性分配 block_size_ * block_count_ 的连续内存
  //   注意对齐：块起始地址要满足 alignof(std::max_align_t)
  //   注意 free list 用「索引」而不是「指针」，避免内存移动后失效
}
BufferPool::~BufferPool() {
  // TODO(M1): 释放底层内存（先确认没有在途句柄，或接受泄漏并断言）
}

Buffer BufferPool::acquire() {
  // TODO(M1): 从 free list 弹出一块 -> 初始化控制块/清零 -> 返回句柄
  return Buffer{};
}

void BufferPool::release(std::uint32_t index) noexcept {
  // TODO(M1): 把块索引压回 free list（多线程下要保证结构不被破坏）
  (void)index;
}

std::size_t BufferPool::block_size() const noexcept { return block_size_; }
std::uint32_t BufferPool::capacity() const noexcept { return block_count_; }
BufferPool::Stats BufferPool::stats() const noexcept {
  // TODO(M1): 返回真实统计（in_use / peak / capacity）
  return Stats{0, 0, block_count_};
}

BufferPool& global_pool() {
  // TODO(M1): 返回进程级单例（M5 讨论静态初始化/析构顺序）
  static BufferPool pool(1024, 256);
  return pool;
}

}  // namespace mp
