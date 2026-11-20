// mini-pipeline · M1 实现（当前全部是桩，测试全红——这是设计好的起点）
// 完成标准见 TASK.md：make test / test-asan / test-tsan 三条全绿。
//
// 动手顺序建议（按"最快看到红转绿"排）：
//   1. 池的底层内存 + free list（先单线程跑通 AcquireBasic / ResetReturns）
//   2. 句柄的 copy/move 与 use_count（让 CopySharesBlock / MoveLeavesEmpty 绿）
//   3. 把计数改成原子、加多线程压测（ConcurrentStress，TSan 会挑毛病）
//   4. 句柄压到 <=16 字节（HandleSize），并保证 data() 对齐（Alignment）
#include "mini_pipeline/buffer_pool.hpp"
#include <atomic>
#include <cstddef>
#include <mutex>

namespace mp {

// ============================ Buffer ============================

Buffer::Buffer() noexcept = default;
Buffer::Buffer(const Buffer &other) noexcept
    : pool_(other.pool_), index_(other.index_) {
  if (other.pool_)
    pool_->ref_counts_[index_].fetch_add(1);
}
Buffer &Buffer::operator=(const Buffer &other) noexcept {
  if (this == &other)
    return *this;

  if (pool_ && pool_->ref_counts_[index_].fetch_sub(1) == 1) {
    pool_->release(index_);
  }

  pool_ = nullptr;
  index_ = 0;

  if (other.pool_) {
    pool_ = other.pool_;
    index_ = other.index_;
    pool_->ref_counts_[index_].fetch_add(1);
  }

  return *this;
}
Buffer::Buffer(Buffer &&other) noexcept
    : pool_(other.pool_), index_(other.index_) {
  other.pool_ = nullptr;
  other.index_ = 0;
}
Buffer &Buffer::operator=(Buffer &&other) noexcept {
  if (this == &other)
    return *this;
  // TODO(M1): 释放自己 -> 接管 other -> other 置空
  if (pool_ && pool_->ref_counts_[index_].fetch_sub(1) == 1) {
    pool_->release(index_);
  }

  pool_ = nullptr;
  index_ = 0;

  if (other.pool_) {
    pool_ = other.pool_;
    index_ = other.index_;
    other.pool_ = nullptr;
    other.index_ = 0;
  }
  return *this;
}
Buffer::~Buffer() {
  // TODO(M1): 引用计数 -1，减到 0 时归还块
  if (pool_ && (pool_->ref_counts_[index_].fetch_sub(1) == 1)) {
    pool_->release(index_);
  }
}

std::byte *Buffer::data() const noexcept {
  if (pool_)
    return pool_->memory_ + index_ * pool_->block_size_;
  return nullptr;
}
std::size_t Buffer::size() const noexcept {
  return pool_ ? pool_->block_size_ : 0;
}
std::uint32_t Buffer::use_count() const noexcept {

  return pool_ ? pool_->ref_counts_[index_].load() : 0;
}
void Buffer::reset() noexcept {
  if (pool_) {
    if (pool_->ref_counts_[index_].fetch_sub(1) == 1) {
      pool_->release(index_);
    }
    pool_ = nullptr;
    index_ = 0;
  }
}
Buffer::operator bool() const noexcept { return pool_ != nullptr; }
Buffer::Buffer(BufferPool *pool, std::uint32_t index)
    : pool_(pool), index_(index) {
  pool_->ref_counts_[index_].fetch_add(1);
}

// ========================== BufferPool ==========================

BufferPool::BufferPool(std::size_t block_size, std::uint32_t block_count)
    : block_size_(block_size), block_count_(block_count) {
  // TODO(M1): 一次性分配 block_size_ * block_count_ 的连续内存
  //   注意对齐：块起始地址要满足 alignof(std::max_align_t)
  //   注意 free list 用「索引」而不是「指针」，避免内存移动后失效
  memory_ = new std::byte[block_size_ * block_count_];
  ref_counts_ = new std::atomic<std::uint32_t>[block_count]();

  free_idx_.reserve(block_count_);
  for (std::uint32_t i = 0; i < block_count_; ++i) {
    free_idx_.push_back(i);
  }
}
BufferPool::~BufferPool() {
  if (memory_)
    delete[] memory_;
  if (ref_counts_) {
    delete[] ref_counts_;
  }
}

Buffer BufferPool::acquire() {
  std::lock_guard<std::mutex> lock_guard(mutex_);
  if (!free_idx_.empty()) {
    auto idx = free_idx_.back();
    free_idx_.pop_back();
    const std::uint32_t now =
        in_use_.fetch_add(1, std::memory_order_relaxed) + 1;

    peak_ = std::max(peak_.load(), now);

    return Buffer{this, idx};
  }
  return Buffer{};
}

void BufferPool::release(std::uint32_t index) noexcept {
  std::lock_guard<std::mutex> lock_guard(mutex_);
  free_idx_.push_back(index);
  in_use_.fetch_sub(1, std::memory_order_relaxed);
}

std::size_t BufferPool::block_size() const noexcept { return block_size_; }
std::uint32_t BufferPool::capacity() const noexcept { return block_count_; }
BufferPool::Stats BufferPool::stats() const noexcept {
  return Stats{in_use_.load(), peak_.load(), block_count_};
};

BufferPool &global_pool() {
  // TODO(M1): 返回进程级单例（M5 讨论静态初始化/析构顺序）
  static BufferPool pool(1024, 256);
  return pool;
}

} // namespace mp
