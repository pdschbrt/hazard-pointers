#include <atomic>
#include <iostream>
#include <thread>
#include <vector>

using namespace std::chrono_literals;

struct Node {
  explicit Node(int v) : v(v) {}
  int v = 0;
};

class DataStructure {
public:
  ~DataStructure() { delete head.load(std::memory_order_relaxed); }

  void read() {
    auto *n = head.load(std::memory_order_acquire);
    std::this_thread::sleep_for(300ms);
    std::cout << "v is: '" << n->v << "'\n";
  }

  void write(int i) {
    auto *newN = new Node(i);
    auto *oldN = head.exchange(newN, std::memory_order_acq_rel);
    delete oldN;
  }

private:
  std::atomic<Node *> head{new Node(9001)};
};

int main() {
  DataStructure d;
  std::vector<std::jthread> threads;
  for (int i = 0; i < 20; ++i) {
    threads.emplace_back([&d]() { d.read(); });
    if (i % 5 == 0) {
      threads.emplace_back([&d, i]() { d.write(i); });
    }
  }
  return 0;
}
