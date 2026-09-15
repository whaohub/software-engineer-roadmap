// ============================================================
// 虚拟头结点 (Dummy Head) — 题目框架
// ============================================================
// 核心思想：在真实头结点前挂一个「哨兵」，把所有位置的操作统一成
//           「修改某个结点的 next」，从而消除下面的特判分支。
//
// 什么时候必须用 dummy：
//   1. 头结点本身可能被删除（LC 203 / LC 82）
//   2. 头结点可能被替换（LC 19 删头）
//
// 固定套路：
//   ListNode dummy(0);          // 栈对象，不要 new
//   dummy.next = head;
//   ListNode *prev = &dummy;    // 前驱指针始终从 dummy 出发
//   ... // 全程只操作 prev->next
//   return dummy.next;          // 返回 dummy 的后继，而非 head
//
// 易错点：dummy 用栈对象，别 new（否则要么漏 delete，要么得配合 ListPtr）
// ============================================================

#include "list_utils.h"

// ============================================================
// 删除类
// ============================================================

// [E] LC 203 — 移除链表元素 🟢
// 题目：给一个链表的头节点 head 和一个整数 val，删除链表中所有满足
//       Node.val == val 的节点
// 要求：返回新的头节点
// 约束：结点数 [0, 10^4]，1 <= Node.val <= 50，0 <= val <= 50
// 思路：dummy 指向 head，prev 指向待删结点的前驱；命中则
//       prev->next = cur->next 并 delete cur（prev 不前移），否则 prev 前移
// 预期复杂度：时间 O(n) | 空间 O(1)
ListNode *removeElements(ListNode *head, int val) {
  (void)head;
  (void)val;
  // TODO: 实现
  return nullptr;
}

// [E] LC 83 — 删除排序链表中的重复元素 🟢
// 题目：给定已排序链表的头 head，删除所有重复的元素，使每个元素只出现一次
// 要求：返回已排序的链表
// 约束：结点数 [0, 300]，链表已按升序排列
// 思路：单指针 cur。若 cur->next 与 cur 值相同则删除 cur->next（cur 不前移，
//       以便处理连续三个以上同值的情况），否则 cur 前移
// 预期复杂度：时间 O(n) | 空间 O(1)
ListNode *deleteDuplicates(ListNode *head) {
  (void)head;
  // TODO: 实现
  return nullptr;
}

// [M] LC 82 — 删除排序链表中的重复元素 II 🟠
// 题目：给定已排序链表的头 head，删除链表中所有重复数字的节点，
//       只留下不同的数字
// 要求：返回已排序的链表
// 约束：结点数 [0, 300]，链表已按升序排列
// 思路：dummy 作 prev，cur 探测「连续同值区间」[cur, last]：
//         若 last != cur（区间长度 > 1）→ 整段删除，prev->next = last->next，
//            prev 保持不动（新区间可能仍与前驱同值）
//         否则 → prev = cur，cur 前移
// 预期复杂度：时间 O(n) | 空间 O(1)
// 易错：头结点也可能整段被删，所以 dummy 是必需的
// 命名：LC 82 与 LC 83 在 LeetCode 上的函数名都叫 deleteDuplicates，
//       这里把 LC 82 的实现重命名为 deleteDuplicatesII 以避免冲突
ListNode *deleteDuplicatesII(ListNode *head) {
  (void)head;
  // TODO: 实现
  return nullptr;
}

// [M] LC 19 — 删除链表的倒数第 N 个结点 🟠
// 题目：给一个链表，删除链表的倒数第 n 个结点
// 要求：返回链表的头结点
// 约束：结点数 sz ∈ [1, 30]，1 <= n <= sz
// 思路：dummy + 快慢指针。fast 先走 n + 1 步，随后 fast/slow 同步前进，
//       当 fast 走到 nullptr 时 slow 恰好指向待删结点的前驱
// 预期复杂度：时间 O(n) | 空间 O(1)
// 易错：fast 先走 n + 1（而非 n）步，差值才是「前驱」而不是「目标本身」
ListNode *removeNthFromEnd(ListNode *head, int n) {
  (void)head;
  (void)n;
  // TODO: 实现
  return nullptr;
}
