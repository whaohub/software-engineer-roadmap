// ============================================================
// 反转类 (Reverse) — 题目框架
// ============================================================
// 反转是链表题的分水岭：写不顺反转，LC 92 / 143 / 234 / 25 全都写不动。
//
// 两种基本实现：
//   迭代 —— 三指针 prev / cur / next，逐个把 cur->next 掉头指向 prev
//   递归 —— 先反转 head->next，再让 head->next->next = head、head->next =
//   nullptr
//
// 迭代模板（背下来）：
//   ListNode *prev = nullptr, *cur = head;
//   while (cur != nullptr) {
//     ListNode *next = cur->next;   // ① 先存后继（顺序错就断链！）
//     cur->next = prev;             // ② 掉头
//     prev = cur;                   // ③ 前驱后移
//     cur = next;                   // ④ 当前后移
//   }
//   return prev;                    // cur 为空时 prev 才是新头
//
// 易错点：① 必须第一步执行；返回值必须是 prev 而不是 cur（cur 已经是 nullptr）
// ============================================================

#include "list_utils.h"

// ============================================================
// 整条反转
// ============================================================

// [E] LC 206 — 反转链表 🟢
// 题目：给单链表的头节点 head，反转链表
// 要求：返回反转后的链表
// 约束：结点数 [0, 5000]
// 思路一（迭代）：三指针原地掉头，见文件头模板
// 思路二（递归）：newHead = reverseList(head->next)，
//                 head->next->next = head，head->next = nullptr，返回 newHead
// 预期复杂度：迭代 时间 O(n) / 空间 O(1)；递归 时间 O(n) / 空间 O(n)（调用栈）
ListNode *reverseList(ListNode *head) {
  (void)head;
  // TODO: 实现（迭代）
  return nullptr;
}

// 递归版本 —— 与上面同一题，仅实现方式不同
ListNode *reverseListRecursive(ListNode *head) {
  (void)head;
  // TODO: 实现（递归）
  return nullptr;
}

// ============================================================
// 部分反转
// ============================================================

// [M] LC 92 — 反转链表 II 🟠
// 题目：给单链表的头指针 head 和两个整数 left、right（left <= right），
//       反转从位置 left 到位置 right 的链表节点
// 要求：返回反转后的链表
// 约束：结点数 [1, 500]，1 <= left <= right <= n
// 思路（头插法，一次遍历）：dummy 出发走 left - 1 步得到 prev，cur =
// prev->next。
//       重复 right - left 次：把 cur 的后继结点摘下，插到 prev 之后
//         ListNode *moved = cur->next;
//         cur->next = moved->next;
//         moved->next = prev->next;
//         prev->next = moved;
// 预期复杂度：时间 O(n) | 空间 O(1)
// 易错：定位走 left - 1 步（走到左边界的前驱）；头插全程 cur 自身不动
ListNode *reverseBetween(ListNode *head, int left, int right) {
  (void)head;
  (void)left;
  (void)right;
  // TODO: 实现
  return nullptr;
}

// [M] LC 24 — 两两交换链表中的节点 🟠
// 题目：给一个链表，两两交换其中相邻的节点
// 要求：返回交换后链表的头节点；不能只是单纯改变节点内部的值，
//       需要实际进行节点交换
// 约束：结点数 [0, 100]
// 思路：dummy + 三指针迭代。每轮持有 prev / first / second：
//         second = first->next;
//         first->next = second->next;
//         second->next = first;
//         prev->next = second;
//         prev = first;      // first 交换后成为本轮的尾巴
// 预期复杂度：时间 O(n) | 空间 O(1)（递归写法同样简洁，栈空间 O(n)）
// 易错：循环条件 prev->next && prev->next->next
// 缺一不可（奇数个结点时最后一轮不换）
ListNode *swapPairs(ListNode *head) {
  (void)head;
  // TODO: 实现
  return nullptr;
}

// ============================================================
// 反转的综合应用
// ============================================================

// [E] LC 234 — 回文链表 🟢
// 题目：给一个单链表的头节点 head，判断该链表是否为回文链表
// 要求：返回 true / false
// 约束：结点数 [1, 10^5]
// 思路（三步组合拳）：① 快慢指针找中点 ② 反转后半段 ③ 前后双指针向中间比较
// 预期复杂度：时间 O(n) | 空间 O(1)
// 易错：① 奇数长度时前半段比后半段多一个结点，循环只需走完后半段
//       ② 生产代码里比较结束后应把后半段再反转回去，保持调用方链表结构不变
bool isPalindrome(ListNode *head) {
  (void)head;
  // TODO: 实现
  return false;
}
