#include "spinlock.h"

void SpinLock::Lock() {
  while (locked_.exchange(true)) {
    while (locked_.load()) {
      std::this_thread::yield();
    }
  }
}

bool SpinLock::TryLock() { return locked_.exchange(true) == 0; }

void SpinLock::Unlock() { locked_.store(false); }
