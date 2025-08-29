#include <folly/executors/CPUThreadPoolExecutor.h>
#include <folly/synchronization/Hazptr-fwd.h>
#include <folly/synchronization/HazptrDomain.h>
#include <folly/synchronization/HazptrHolder.h>
#include <folly/synchronization/HazptrObj.h>

#include <atomic>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <utility>

struct Data : public folly::hazptr_obj_base<Data> {
  int id = 0;
  std::string name = "";

  Data() = default;
  Data(int id, std::string name) : id(id), name(std::move(name)) {}

  virtual ~Data() {
    std::cout << "data is destroyed: " << id << " '" << name << "'\n";
  }
};

class SharedValue {
public:
  SharedValue(const std::string &name) {
    auto *newData = new Data(nextId++, name);
    sharedPtr.store(newData, std::memory_order_relaxed);
  }

  ~SharedValue() {
    Data *currData = sharedPtr.load(std::memory_order_relaxed);
    if (currData) {
      currData->retire();
    }
  }

  // A thread-safe update operation.
  void update(std::string newName) {
    auto *newData = new Data(nextId++, newName);
    // Atomically swap the pointer.
    Data *oldData = sharedPtr.exchange(newData, std::memory_order_relaxed);
    // Retire the old pointer.
    if (oldData) {
      oldData->retire();
    }
  }

  // A thread-safe read operation.
  std::unique_ptr<Data> read() {
    using namespace std::chrono_literals;
    // Create a HazptrHolder to manage the hazard pointer.
    // The holder automatically retires the hazard pointer on destruction.
    folly::hazptr_holder hptr = folly::make_hazard_pointer();
    auto *data = hptr.protect(sharedPtr);

    // The following line shows how not to do it!
    // auto *data = sharedPtr.load(std::memory_order_relaxed);

    // Let's wait a bit to ensure we trigger the nasty use-after-free bug.
    std::this_thread::sleep_for(1s);
    if (data) {
      // This is the critical read operation!
      return std::make_unique<Data>(*data);
    }
    return nullptr;
  }

private:
  std::atomic<Data *> sharedPtr = nullptr;
  static inline std::atomic<int> nextId = 0;
};

int main() {
  std::cout << "Starting Folly Hazard Pointer example.\n\n";

  SharedValue value("initial_value");

  folly::CPUThreadPoolExecutor threadPool(/*numThreads=*/4);
  // Simulate concurrent reads and writes
  for (int i = 0; i < 20; ++i) {
    threadPool.add([&value, i] {
      auto data = value.read();
      if (data) {
        std::cout << "Thread " << i << " read data: " << data->name << '\n';
      } else {
        std::cout << "Thread " << i << " failed to read\n";
      }
      if (i % 5 == 0) {
        // A writer thread updates the value.
        std::string newName = "new Value " + std::to_string(i);
        value.update(newName);
        std::cout << "Thread " << i << " updated value to: " << newName << '\n';
      }
    });
  }
  threadPool.join();

  std::cout << "\nAll tasks completed. Main thread now exiting.\n";
  return 0;
}
