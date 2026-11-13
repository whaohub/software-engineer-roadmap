#pragma once

#include <thread>

class Thread {
public:
  template <typename F,typename ...args>
  explicit Thread(F &&f,args && ...arg):t(std::forward<F>(f),std::forward<args>(arg)...){}
  ~Thread() noexcept{
    if(t.joinable()){
      t.join();
    }
  }
  Thread(Thread&) = delete;
  Thread& operator=(Thread&) = delete;
  Thread(Thread&&) noexcept = default;
  Thread& operator=(Thread&&) noexcept = default;

private:
  std::thread t;
};
