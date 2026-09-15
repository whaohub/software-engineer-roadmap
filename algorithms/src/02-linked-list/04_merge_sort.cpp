// ============================================================
// 合并与排序 (Merge & Sort) — 题目框架
// ============================================================
// 链表是归并排序的天然舞台：
//   - 数组合并需要额外 O(n) 空间，链表只需改指针 → O(1) 空间
//   - 数组「找中点」靠下标，链表靠快慢指针
//   - 数组「插入」要搬移元素，链表只需改两条 next
//
// 合并模板（dummy + 尾指针）：
//   ListNode dummy(0);
//   ListNode *tail = &dummy;
//   while (l1 != nullptr && l2 != nullptr) {
//     if (l1->val <= l2->val) { tail->next = l1; l1 = l1->next; }
//     else                    { tail->next = l2; l2 = l2->next; }
//     tail = tail->next;
//   }
//   tail->next = (l1 != nullptr) ? l1 : l2;   // 剩余部分整体挂上，不必再遍历
//   return dummy.next;
//
// 易错点：收尾时把剩余子链「整体」接上，写成循环逐个搬运是多余的
// ============================================================

#include "list_utils.h"

// ============================================================
// 合并
// ============================================================

// [E] LC 21 — 合并两个有序链表 🟢
// 题目：将两个升序链表合并为一个新的升序链表并返回
// 要求：新链表是通过拼接给定的两个链表的所有节点组成的
// 约束：两链表结点数 [0, 50]
// 思路：dummy + 尾指针，每轮取两链头结点中较小者接到尾部
// 预期复杂度：时间 O(m + n) | 空间 O(1)
ListNode *mergeTwoLists(ListNode *list1, ListNode *list2) {
  (void)list1;
  (void)list2;
  // TODO: 实现
  return nullptr;
}

// ============================================================
// 分组 / 重排
// ============================================================

// [M] LC 86 — 分隔链表 🟠
// 题目：给一个链表的头节点 head 和一个特定值 x，对链表进行分隔，使得所有
//       小于 x 的节点都出现在大于或等于 x 的节点之前
// 要求：保留两个分区中每个节点的初始相对位置
// 约束：结点数 [0, 200]，-100 <= Node.val <= 100
// 思路：建两条子链 small / large（各自 dummy + 尾指针），遍历原链表按
//       val < x 分流；最后 smallTail->next = largeDummy.next，
//       并将 largeTail->next 置 nullptr
// 预期复杂度：时间 O(n) | 空间 O(1)
// 易错：必须给 large 链尾置 nullptr，否则原链表的后继残留会形成环
ListNode *partition(ListNode *head, int x) {
  (void)head;
  (void)x;
  // TODO: 实现
  return nullptr;
}

// [M] LC 328 — 奇偶链表 🟠
// 题目：给定单链表的头节点 head，将所有索引为奇数的节点和索引为偶数的
//       节点分别组合在一起（第 1 个节点为奇数，第 2 个为偶数，依此类推）
// 要求：返回重新排列后的链表的头节点；必须在 O(1) 额外空间、O(n) 时间内解决；
//       奇数节点与偶数节点的相对顺序应保持
// 约束：结点数 [0, 10^4]
// 思路：odd / even 两个指针各自成链，循环中交替接线：
//         odd->next  = even->next;  odd  = odd->next;
//         even->next = odd->next;   even = even->next;
//       循环结束后 odd->next = evenHead
// 预期复杂度：时间 O(n) | 空间 O(1)
// 易错：循环条件是 while (even && even->next)；偶数链表头 evenHead 需提前缓存
ListNode *oddEvenList(ListNode *head) {
  (void)head;
  // TODO: 实现
  return nullptr;
}

// [M] LC 61 — 旋转链表 🟠
// 题目：给一个链表的头节点 head，旋转链表，将链表每个节点向右移动 k 个位置
// 要求：返回旋转后的链表头节点
// 约束：结点数 [0, 500]，0 <= k <= 2×10^9
// 思路：① 遍历求长度 n，顺手把尾结点接到 head 上成环
//       ② k %= n（k 可能远大于 n）
//       ③ 从头走 n - k - 1 步得到新尾结点，断开其 next 即为新头
// 预期复杂度：时间 O(n) | 空间 O(1)
// 易错：n == 0 或 k % n == 0 时直接返回原 head，避免在环上多绕一圈
ListNode *rotateRight(ListNode *head, int k) {
  (void)head;
  (void)k;
  // TODO: 实现
  return nullptr;
}

// [M] LC 143 — 重排链表 🟠
// 题目：给定单链表 L 的头节点 head（L 表示为 L0 → L1 → … → Ln），
//       将其重排为 L0 → Ln → L1 → Ln-1 → L2 → Ln-2 → …
// 要求：不能只改变节点内部的值，需要实际的节点交换
// 约束：结点数 [1, 5×10^4]
// 思路（三步组合拳，综合了本专题全部基本功）：
//       ① 快慢指针找中点
//       ② 反转后半段（复用 LC 206）
//       ③ 前后两段交替合并（后半段可能比前半段短 1 个结点）
// 预期复杂度：时间 O(n) | 空间 O(1)
// 易错：切分前必须把前半段尾结点的 next 置 nullptr，否则两段仍连着会成环
void reorderList(ListNode *head) {
  (void)head;
  // TODO: 实现
}

// ============================================================
// 排序
// ============================================================

// [M] LC 148 — 排序链表 🟠
// 题目：给链表的头结点 head，请将其按升序排列并返回排序后的链表
// 要求：在 O(n log n)
// 时间复杂度和常数级空间复杂度下解决（递归栈不计入则需自底向上归并）
// 约束：结点数 [0, 5×10^4]，-10^5 <= Node.val <= 10^5
// 思路（自顶向下归并）：
//       ① 递归终止：head == nullptr || head->next == nullptr
//       ② 快慢指针找中点并「断开」（leftTail->next = nullptr）
//       ③ 递归排序两半
//       ④ 复用 mergeTwoLists 合并
// 预期复杂度：时间 O(n log n) | 空间 O(log n)（递归栈）
// 易错：找中点时若 slow 从 head 出发，偶数长度下前半段会比后半段短；
//       想两段均分可让 slow 从 dummy 出发，或统一按「前半段更短」处理
ListNode *sortList(ListNode *head) {
  (void)head;
  // TODO: 实现
  return nullptr;
}

// [M] LC 147 — 对链表进行插入排序 🟠
// 题目：给定单个链表的头 head，使用插入排序对链表进行排序，并返回排序后
//       链表的头节点
// 要求：保持「已排序部分 + 待插入部分」的双段结构
// 约束：结点数 [1, 5000]，-5000 <= Node.val <= 5000
// 思路：dummy + lastSorted（已排序部分尾结点）+ cur（当前待插入结点）
//       ① lastSorted->val <= cur->val → 天然有序，lastSorted 后移
//       ② 否则从 dummy 开始找第一个大于 cur->val 的前驱，摘链插入
// 预期复杂度：时间 O(n²) | 空间 O(1)
// 易错：插入前先 lastSorted->next = cur->next 断开 cur，再执行插入
ListNode *insertionSortList(ListNode *head) {
  (void)head;
  // TODO: 实现
  return nullptr;
}
