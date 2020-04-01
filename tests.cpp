#include <chrono>
#include <functional>
#include <iostream>
#include <random>
#include <thread>
#include <vector>

#include "spinlock.h"

using std::operator""s;
using std::operator""ns;

class Tester {
 private:
#define tester_current_time std::chrono::steady_clock::now

  using tester_duration_t = std::chrono::seconds;
  using tester_time_t = std::chrono::time_point<std::chrono::steady_clock>;

 public:
  void operator()() {
    UnitTest();
    StressTest();
  }

 private:
  tester_duration_t time_limit_;
  tester_time_t launch_time_;
  bool done_;
  bool error_flag_;

  void ConditionalHandleError(bool statement, const std::string& message) {
    if (statement) {
      std::cerr << message;
      error_flag_ = true;
      exit(EXIT_FAILURE);
    }
  }

  void InitTester(tester_duration_t duration) {
    time_limit_ = duration;
    launch_time_ = tester_current_time();
    done_ = false;
    error_flag_ = false;
  }

  void WaitAndCheckForTL() {
    int status = 0;
    while (!done_) {
      if (error_flag_) {
        exit(EXIT_FAILURE);
      }
      ConditionalHandleError(tester_current_time() > launch_time_ + time_limit_,
                             "Tester time limit exceeded");
    }
  }

  void UnitTest() {
    InitTester(1s);

    std::thread unit_test_executor([this]() {
      LockUnlock();
      LockUnlockSequence();
      TryLock();

      done_ = true;
    });
    std::thread unit_test_checker([this]() { WaitAndCheckForTL(); });

    unit_test_executor.join();
    unit_test_checker.join();

    std::cout << "Unit tests passed /ᐠ.ꞈ.ᐟ\\" << std::endl;
  }

  void StressTest() {
    InitTester(5s);

    std::thread stress_test_executor([this]() {
      StressLockUnlock();

      done_ = true;
    });
    std::thread stress_test_checker([this]() { WaitAndCheckForTL(); });

    stress_test_executor.join();
    stress_test_checker.join();

    std::cout << "Stress tests passed (=^ ◡ ^=)" << std::endl;
  }

  static void LockUnlock() {
    spinlock spinlock;
    spinlock.lock();
    spinlock.unlock();
  }

  static void LockUnlockSequence() {
    spinlock spinlock;
    spinlock.lock();
    spinlock.unlock();
    spinlock.lock();
    spinlock.unlock();
  }

  void TryLock() {
    spinlock spinlock;
    ConditionalHandleError(!spinlock.try_lock(), "spinlock try_lock failure");
    ConditionalHandleError(spinlock.try_lock(), "spinlock try_lock failure");
    spinlock.unlock();
    ConditionalHandleError(!spinlock.try_lock(), "spinlock try_lock failure");
    spinlock.unlock();
    spinlock.lock();
    ConditionalHandleError(spinlock.try_lock(), "spinlock try_lock failure");
  }

  void StressLockUnlock() {
    const int NUM_THREADS = 1000;
    std::vector<std::thread> thread_pool;
    thread_pool.reserve(NUM_THREADS);

    const int NUM_SPINLOCKS = 10;
    std::vector<spinlock> spinlocks(NUM_SPINLOCKS);

    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> index_dist(
        0, NUM_SPINLOCKS - 1);
    std::uniform_int_distribution<std::mt19937::result_type> scenario_dist(0,
                                                                           1);
    for (int i = 0; i < NUM_THREADS; ++i) {
      thread_pool.emplace_back(std::thread(std::bind([&]() {
        int scenario = scenario_dist(dev);
        int index = index_dist(dev);
        switch (scenario) {
          case 0:
            while (!spinlocks[index].try_lock())
              ;
            break;
          case 1:spinlocks[index].lock();
            break;
          default:
            ConditionalHandleError(true, "scenario generation failure");
        }
        std::this_thread::sleep_for(100ns);
        spinlocks[index].unlock();
      })));
    }

    for (auto&& thread_object : thread_pool) {
      thread_object.join();
    }
  }
};

int main() {
  Tester tester;
  tester();
  return 0;
}
