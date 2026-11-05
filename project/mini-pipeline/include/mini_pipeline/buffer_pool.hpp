// mini-pipeline · M1：定长块池 + 引用计数句柄
// 目标见 TASK.md。此文件只给接口契约，实现留给你（是的，现在全是红的）。
//
// 约束（面试会追问，写代码时就要想清楚）：
//   1) sizeof(mp::Buffer) <= 16  —— 句柄必须能塞进通道槽位
//   2) Buffer 的拷贝/析构是引用计数操作，必须线程安全
//   3) 池耗尽的行为要明确定义（返回空句柄 或 抛 std::bad_alloc，二选一并写进文档）
#pragma once

#include <cstddef>
#include <cstdint>

namespace mp {

class BufferPool;

/// 定长块句柄：拷贝增引用，最后一个句柄析构时把块归还池。
class Buffer {
 public:
  /// 空句柄（不持有任何块）
  Buffer() noexcept;
  Buffer(const Buffer& other) noexcept;
  Buffer& operator=(const Buffer& other) noexcept;
  Buffer(Buffer&& other) noexcept;
  Buffer& operator=(Buffer&& other) noexcept;
  ~Buffer();

  /// 块起始地址；空句柄返回 nullptr
  std::byte* data() const noexcept;
  std::size_t size() const noexcept;
  /// 当前引用数；空句柄为 0
  std::uint32_t use_count() const noexcept;
  /// 手动归还（等价于置空）
  void reset() noexcept;
  explicit operator bool() const noexcept;

 private:
  friend class BufferPool;
  // TODO(M1): 你的内部表示。提示——两个字段就能做到 16 字节以内：
  //   一个指向池（或块头），一个放块索引/引用计数。想清楚控制块放哪，
  //   放堆上会让 sizeof 超标（见 TASK.md「句柄膨胀」那个坑）。
  void* pool_or_block_ = nullptr;
  std::uint32_t index_ = 0;
};

/// 定长块池：构造时一次性分配 block_size * block_count 连续内存。
class BufferPool {
 public:
  struct Stats {
    std::uint32_t in_use = 0;   ///< 当前被借出的块数
    std::uint32_t peak = 0;     ///< 历史峰值
    std::uint32_t capacity = 0; ///< 总块数
  };

  BufferPool(std::size_t block_size, std::uint32_t block_count);
  ~BufferPool();

  BufferPool(const BufferPool&) = delete;
  BufferPool& operator=(const BufferPool&) = delete;

  /// 借一个块；池耗尽时返回空句柄（若你选择抛 std::bad_alloc，改这里并更新文档）
  Buffer acquire();

  std::size_t block_size() const noexcept;
  std::uint32_t capacity() const noexcept;
  Stats stats() const noexcept;

 private:
  friend class Buffer;
  /// Buffer 析构/重置时调用：归还一个块
  void release(std::uint32_t index) noexcept;

  // TODO(M1): 底层内存、free list、原子计数、统计
  std::size_t block_size_ = 0;
  std::uint32_t block_count_ = 0;
};

/// 进程级全局池（M5 会讨论这里的静态初始化顺序问题）
BufferPool& global_pool();

}  // namespace mp
