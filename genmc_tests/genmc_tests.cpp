#include "../spinlock.h"

void ConditionalHandleError(bool statement, int& exit_code) {
  if (!exit_code && statement) {
    exit_code = 1;
  }
}

int main() {
  int exit_code = 0;

  spinlock spinlock;
  spinlock.lock();
  spinlock.unlock();
  spinlock.lock();
  spinlock.unlock();

  ConditionalHandleError(!spinlock.try_lock(), exit_code);
  ConditionalHandleError(spinlock.try_lock(), exit_code);
  spinlock.unlock();
  ConditionalHandleError(!spinlock.try_lock(), exit_code);
  spinlock.unlock();
  spinlock.lock();
  ConditionalHandleError(spinlock.try_lock(), exit_code);

  return exit_code;
}
