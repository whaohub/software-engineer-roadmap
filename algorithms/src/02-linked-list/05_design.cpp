// ============================================================
// 设计类 (Design) — 题目框架
// ============================================================
// 这类题不考单一技巧，考的是「选择正确的数据结构组合」。
// 面试高频度极高，且答案有强套路，属于必须背下来的部分。
//
// 三个标志性组合：
//   LC 707 双向链表 + 虚拟头尾结点      — 索引访问 O(min(k, n-k))
//   LC 146 哈希表 + 双向链表            — get / put 平均 O(1)
//   LC 2   虚拟头结点 + 进位变量        — 大数加法
//
// 为什么 LRU 一定要用「双向」链表：
//   淘汰最旧结点时需要在 O(1) 时间内拿到它的前驱并改 prev 指针，
//   单向链表只能从头遍历（O(n)），双向链表靠结点自带的 prev 直接完成。
// ============================================================

#include "list_utils.h"

#include <unordered_map>

// ============================================================
// [M] LC 707 — 设计链表 🟠
// ============================================================
// 题目：设计链表的实现。可以选择使用单链表或双链表。单链表中的节点应该具有
//       两个属性：val 和 next。val 是当前节点的值，next 是指向下一个节点的
//       指针/引用。实现 MyLinkedList 类：
//         get(index)              获取链表中第 index 个节点的值；无效则返回 -1
//         addAtHead(val)          在链表第一个元素之前添加一个值为 val 的节点
//         addAtTail(val)          将值为 val 的节点追加到链表的最后一个元素之后
//         addAtIndex(index, val)  在链表中的第 index 个节点之前添加值为 val
//         的节点；
//                                 index == 链表长度时追加到末尾；index
//                                 不合法则不插入
//         deleteAtIndex(index)    如果下标有效，则删除链表中下标为 index 的节点
// 要求：索引均从 0 开始
// 约束：结点数 [0, 2000]，0 <= index, val <= 1000，最多调用 2000 次
// 思路：双向链表 + 虚拟头尾结点（sentinel），size_ 记录有效结点个数。
//       定位时走「离哪端近就从哪端走」以减少移动次数
// 预期复杂度：addAtHead / addAtTail O(1)；get / addAtIndex / deleteAtIndex
//             O(min(k, n - k))；空间 O(n)
// 易错：① get / deleteAtIndex 的越界条件用 index >= size_（而非 index > size_）
//       ② addAtIndex 允许 index == size_（等价于 addAtTail）
//       ③ 虚拟头尾结点让「在头部插入」和「在尾部插入」无需特判
class MyLinkedList {
public:
  MyLinkedList() {
    // TODO: 创建虚拟头尾结点并双向相连
  }

  ~MyLinkedList() {
    // TODO: 释放全部结点（含两个虚拟结点）
  }

  int get(int index) {
    (void)index;
    // TODO: 实现
    return -1;
  }

  void addAtHead(int val) {
    (void)val;
    // TODO: 实现
  }

  void addAtTail(int val) {
    (void)val;
    // TODO: 实现
  }

  void addAtIndex(int index, int val) {
    (void)index;
    (void)val;
    // TODO: 实现
  }

  void deleteAtIndex(int index) {
    (void)index;
    // TODO: 实现
  }

private:
  // 虚拟头尾结点：永远存在，使头部/尾部插入逻辑与中间插入完全一致
  DListNode *dummy_head_ = nullptr;
  DListNode *dummy_tail_ = nullptr;
  int size_ = 0;
};

// ============================================================
// [M] LC 146 — LRU 缓存 🟠
// ============================================================
// 题目：请你设计并实现一个满足 LRU (最近最少使用) 缓存约束的数据结构。
//       实现 LRUCache 类：
//         LRUCache(int capacity)  以正整数容量 capacity 初始化 LRU 缓存
//         int get(int key)        存在则返回其值并标记为「最近使用」，否则返回
//         -1 void put(int key, int value)
//                                 存在则更新值并标记为「最近使用」；不存在则插入；
//                                 超出容量则逐出「最久未使用」的键值
// 要求：get 和 put 必须以 O(1) 平均时间复杂度运行
// 约束：1 <= capacity <= 3000，0 <= key <= 10^4，0 <= value <= 10^5，
//       最多调用 2 × 10^5 次 get 和 put
// 思路：哈希表（key → 结点指针）+ 双向链表（头部最新、尾部最旧）
//         get / put 命中 → 把结点移到头部
//         put 未命中   → 新建结点插到头部、写入哈希表；
//                        若 size > capacity 则删掉尾结点并从哈希表 erase
// 预期复杂度：get / put 平均 O(1) | 空间 O(capacity)
// 易错：① 删尾结点前必须先取出它的 key 才能 erase 哈希表（所以结点要存 key）
//       ② put 命中已存在的 key 时只更新 val + 移到头部，不要新建结点
//       ③ 用虚拟头尾结点可省去「链表为空」「只有一个结点」的特判
class LRUCache {
public:
  explicit LRUCache(int capacity) {
    (void)capacity;
    // TODO: 记录容量，创建虚拟头尾结点并双向相连
  }

  ~LRUCache() {
    // TODO: 释放全部缓存结点（含两个虚拟结点）
  }

  int get(int key) {
    (void)key;
    // TODO: 实现
    return -1;
  }

  void put(int key, int value) {
    (void)key;
    (void)value;
    // TODO: 实现
  }

private:
  // 把结点从原位置摘下并插到 dummy_head_ 之后（标记为最近使用）
  void moveToFront(DListNode *node) {
    (void)node;
    // TODO: 实现
  }

  // 在 dummy_head_ 之后插入新结点（key = -1 可用于虚拟结点）
  void insertAtFront(DListNode *node) {
    (void)node;
    // TODO: 实现
  }

  // 从链表中摘除结点（不释放内存）
  void unlink(DListNode *node) {
    (void)node;
    // TODO: 实现
  }

  int capacity_ = 0;
  int size_ = 0;
  std::unordered_map<int, DListNode *> cache_;
  DListNode *dummy_head_ = nullptr; // 最新使用端
  DListNode *dummy_tail_ = nullptr; // 最久未使用端
};

// ============================================================
// [M] LC 2 — 两数相加 🟠
// ============================================================
// 题目：给你两个非空的链表，表示两个非负的整数。它们每位数字都是按照逆序的
//       方式存储的，并且每个节点只能存储一位数字。请将两个数相加，并以相同
//       形式返回一个表示和的链表
// 要求：除数字 0 之外，这两个数都不会以 0 开头
// 约束：每个链表的结点数 [1, 100]，0 <= Node.val <= 9
// 思路：dummy + 尾指针 + 进位变量 carry。同时遍历两条链，逐位求和：
//         int sum = a + b + carry;
//         tail->next = new ListNode(sum % 10);
//         carry = sum / 10;
// 预期复杂度：时间 O(max(m, n)) | 空间 O(1)（不把返回结果算作额外空间）
// 易错：① 循环条件必须包含 carry != 0（如 5 + 5 = 10 会多产生一位）
//       ② 某条链先走完时，该位按 0 计算，不要提前退出循环
ListNode *addTwoNumbers(ListNode *l1, ListNode *l2) {
  (void)l1;
  (void)l2;
  // TODO: 实现
  return nullptr;
}
