#include <atomic>
#include <iostream>
#include <ostream>
#include <thread>

std::atomic<int> count;
// atomic basic
// atomic 只保证线程间访问原子性
void worker() {

  for (int i = 0; i < 100000; ++i) {
    count.fetch_add(1);
  }
}

void basic_atomic() {
  std::thread t1(worker);
  std::thread t2(worker);

  t1.join();
  t2.join();
  std::cout << "count : " << count << std::endl;
}

// memory_order
//
// memory_order_relaxed

int data;
std::atomic<bool> ready;
// data race
void producer() {
  data = 100;
  ready.store(true, std::memory_order_release);
}

void consumer() {
  while (!ready.load(std::memory_order_relaxed)) {
  }

  std::cout << "data: " << data << std::endl;
}

void order_relaxed() {
  std::thread t1(producer);
  std::thread t2(consumer);

  t1.join();
  t2.join();
}

std::atomic<int> x{0};
std::atomic<int> y{0};

int r1 = 0;
int r2 = 0;

// Thread A
void threadA() {
  x.store(1, std::memory_order_relaxed);

  r1 = y.load(std::memory_order_relaxed);
  std::cout << "x: " << x << " r1: " << r1 << std::endl;
}

// Thread B
void threadB() {
  y.store(1, std::memory_order_relaxed);

  r2 = x.load(std::memory_order_relaxed);
  std::cout << "y: " << x << " r2: " << r2 << std::endl;
}

void store_buffing() {
  std::thread t1(threadA);
  std::thread t2(threadB);

  t1.join();
  t2.join();
}

int main() {
  store_buffing();
  return 0;
}