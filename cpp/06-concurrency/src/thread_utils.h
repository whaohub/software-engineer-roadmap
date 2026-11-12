#pragma once

#include <thread>

class Thread {
public:
  Thread();

private:
  std::thread t;
};
