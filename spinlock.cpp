#include "spinlock.h"

void spinlock::lock() {
  while (locked_.exchange(true)) {
    while (locked_.load()) {
      // Burns CPU time... there should be std::this::thread_yield or sleep
      // can't include <thread> because of genmc ABI issues
    }
  }
}

bool spinlock::try_lock() { return locked_.exchange(true) == 0; }

void spinlock::unlock() { locked_.store(false); }
