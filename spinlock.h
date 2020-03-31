#pragma once

#include <atomic>
#include <thread>

class SpinLock {
 public:
  SpinLock() = default;
  ~SpinLock() = default;
  void Lock();
  bool TryLock();
  void Unlock();

 private:
  std::atomic<bool> locked_{false};
};
