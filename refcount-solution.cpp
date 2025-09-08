#include <atomic>
#include <iostream>
#include <memory>
#include <thread>
#include <vector>

using namespace std::chrono_literals;

struct Node {
  explicit Node(int v) : v(v) {}
  int v = 0;
};

class DataStructure {
public:
  void read() {
    auto n = shared.load();
    std::this_thread::sleep_for(300ms);
    std::cout << "v is: '" << n->v << "'\n";
  }

  void write(int i) {
    auto newN = std::make_shared<Node>(i);
    shared.exchange(newN);
  }

private:
  std::atomic<std::shared_ptr<Node>> shared{std::make_shared<Node>(9001)};
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
