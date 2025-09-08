#include <folly/synchronization/Hazptr.h>

#include <atomic>
#include <iostream>
#include <thread>
#include <vector>

using namespace std::chrono_literals;

struct Node : folly::hazptr_obj_base<Node> {
  explicit Node(int v) : v(v) {}
  int v = 0;
};

class DataStructure {
public:
  ~DataStructure() { shared.load(std::memory_order_relaxed)->retire(); }

  void read() {
    folly::hazptr_holder hptr = folly::make_hazard_pointer();
    auto *n = hptr.protect(shared);
    std::this_thread::sleep_for(300ms);
    std::cout << "v is: '" << n->v << "'\n";
  }

  void update(int i) {
    auto *newN = new Node(i);
    auto *oldN = shared.exchange(newN, std::memory_order_relaxed);
    oldN->retire();
  }

private:
  std::atomic<Node *> shared = new Node(9001);
};

int main() {
  DataStructure d;
  std::vector<std::jthread> threads;
  for (int i = 0; i < 20; ++i) {
    threads.emplace_back([&d]() { d.read(); });
    if (i % 5 == 0) {
      threads.emplace_back([&d, i]() { d.update(i); });
    }
  }
  return 0;
}
