#pragma once

#include <atomic>

class spinlock {
 public:
  spinlock() = default;
  ~spinlock() = default;
  void lock();
  bool try_lock();
  void unlock();

 private:
  std::atomic<bool> locked_{false};
};
