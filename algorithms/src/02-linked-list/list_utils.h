// ============================================================
// 链表公共定义与测试工具 —— list_utils.h
// ============================================================
// 本文件为 header-only：所有函数与类均声明为 inline 或类内定义，
// 因此可以安全地被多个 .cpp 以及测试源文件重复 include（不违反 ODR）。
//
// 命名空间划分：
//   ListNode / DListNode        —— 全局，与 LeetCode 官方签名保持一致
//   list_utils::*               —— 测试辅助函数（构造 / 序列化 / 释放）
//   ListPtr                     —— RAII 包装，出作用域自动释放整条链表
// ============================================================

#ifndef ALGORITHMS_LINKED_LIST_UTILS_H
#define ALGORITHMS_LINKED_LIST_UTILS_H

#include <cstddef>
#include <initializer_list>
#include <utility>
#include <vector>

// ============================================================
// 单链表结点 —— 与 LeetCode 官方定义一致
// ============================================================
struct ListNode {
  int val;
  ListNode *next;
  ListNode() : val(0), next(nullptr) {}
  ListNode(int x) : val(x), next(nullptr) {}
  ListNode(int x, ListNode *next) : val(x), next(next) {}
};

// ============================================================
// 双向链表结点 —— LC 146 LRU 缓存 / LC 707 设计链表 使用
// ============================================================
struct DListNode {
  int key;
  int val;
  DListNode *prev;
  DListNode *next;
  DListNode() : key(0), val(0), prev(nullptr), next(nullptr) {}
  DListNode(int k, int v) : key(k), val(v), prev(nullptr), next(nullptr) {}
};

namespace list_utils {

// ------------------------------------------------------------
// 防死循环上限：单条链表超过该结点数视为存在环，遍历提前终止
// （用 static constexpr 而非 inline 变量，兼容 C++11/14 的解析器）
// ------------------------------------------------------------
static constexpr std::size_t kMaxNodes = 100000;

// ============================================================
// 构造 —— 由数组/初始化列表生成链表（空输入返回 nullptr）
// ============================================================
inline ListNode *buildList(const std::vector<int> &vals) {
  ListNode dummy(0);
  ListNode *tail = &dummy;
  for (int v : vals) {
    tail->next = new ListNode(v);
    tail = tail->next;
  }
  return dummy.next;
}

inline ListNode *buildList(std::initializer_list<int> vals) {
  return buildList(std::vector<int>(vals));
}

// ============================================================
// 序列化 —— 链表 → vector，便于测试中直接 EXPECT_EQ 比较
// 存在环时最多读取 kMaxNodes 个结点后终止（不会死循环）
// ============================================================
inline std::vector<int> toVector(ListNode *head) {
  std::vector<int> out;
  for (ListNode *cur = head; cur != nullptr && out.size() < kMaxNodes;
       cur = cur->next) {
    out.push_back(cur->val);
  }
  return out;
}

// ============================================================
// 长度统计 —— 同样受 kMaxNodes 保护
// ============================================================
inline std::size_t length(ListNode *head) {
  std::size_t n = 0;
  for (ListNode *cur = head; cur != nullptr && n < kMaxNodes; cur = cur->next) {
    ++n;
  }
  return n;
}

// ============================================================
// 定位 —— 返回从 0 开始计数的第 index 个结点，越界返回 nullptr
// LC 142（环入口）、LC 160（相交点）构造测试数据时使用
// ============================================================
inline ListNode *nodeAt(ListNode *head, std::size_t index) {
  ListNode *cur = head;
  for (std::size_t i = 0; i < index && cur != nullptr; ++i) {
    cur = cur->next;
  }
  return cur;
}

// ============================================================
// 建环 / 破环 —— 供 LC 141 / LC 142 使用
// connectTailTo(head, pos): 尾结点 next 指向第 pos 个结点
//                           pos < 0 表示不建环（保持 nullptr）
// ============================================================
inline void connectTailTo(ListNode *head, int pos) {
  if (head == nullptr || pos < 0) {
    return;
  }
  ListNode *tail = head;
  while (tail->next != nullptr) {
    tail = tail->next;
  }
  tail->next = nodeAt(head, static_cast<std::size_t>(pos));
}

// 断开环：把链表中第一个被重复访问的结点的 next 置空，恢复成单链表
inline void breakCycle(ListNode *head) {
  ListNode *slow = head;
  ListNode *fast = head;
  while (fast != nullptr && fast->next != nullptr) {
    slow = slow->next;
    fast = fast->next->next;
    if (slow == fast) {
      ListNode *entry = head;
      while (entry != slow) {
        entry = entry->next;
        slow = slow->next;
      }
      ListNode *tail = entry;
      while (tail->next != entry) {
        tail = tail->next;
      }
      tail->next = nullptr;
      return;
    }
  }
}

// ============================================================
// 释放 —— 逐个 delete，避免测试中出现内存泄漏
// ⚠️ 带环链表请先调用 breakCycle()，否则本函数会在环中死循环
// ============================================================
inline void freeList(ListNode *head) {
  while (head != nullptr) {
    ListNode *next = head->next;
    delete head;
    head = next;
  }
}

} // namespace list_utils

// ============================================================
// ListPtr —— 链表所有权的 RAII 包装
// ============================================================
// 用途：测试中 buildList() 返回的裸指针由本类托管，函数返回时自动
//      freeList()，无需在每个 TEST 里手写释放逻辑（对齐仓库的 RAII 主题）。
//
// 用法：
//   ListPtr in = ListPtr(list_utils::buildList({1, 2, 3}));
//   ListPtr out(list_utils::buildList({3, 2, 1}));
//   EXPECT_EQ(list_utils::toVector(out.get()), ...);
//
// 注意：不可拷贝（独占所有权），可移动。
// ============================================================
class ListPtr {
public:
  ListPtr() = default;
  explicit ListPtr(ListNode *head) : head_(head) {}

  ~ListPtr() { list_utils::freeList(head_); }

  ListPtr(const ListPtr &) = delete;
  ListPtr &operator=(const ListPtr &) = delete;

  ListPtr(ListPtr &&other) noexcept : head_(other.release()) {}

  ListPtr &operator=(ListPtr &&other) noexcept {
    if (this != &other) {
      list_utils::freeList(head_);
      head_ = other.release();
    }
    return *this;
  }

  // 放弃所有权并返回裸指针（用于把链表交给被测函数）
  ListNode *release() {
    ListNode *tmp = head_;
    head_ = nullptr;
    return tmp;
  }

  // 交出裸指针但保留所有权（用于调用只读的被测函数）
  ListNode *get() const { return head_; }

  ListNode *operator->() const { return head_; }

  explicit operator bool() const { return head_ != nullptr; }

  std::vector<int> toVector() const { return list_utils::toVector(head_); }

private:
  ListNode *head_ = nullptr;
};

#endif // ALGORITHMS_LINKED_LIST_UTILS_H
