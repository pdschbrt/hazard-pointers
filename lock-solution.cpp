#include <iostream>
#include <mutex>
#include <shared_mutex>
#include <thread>
#include <utility>
#include <vector>

using namespace std::chrono_literals;

struct Node {
  explicit Node(int v) : v(v) {}
  int v = 0;
};

class DataStructure {
public:
  ~DataStructure() { delete shared; }

  void read() {
    std::shared_lock lock(mtx);
    std::this_thread::sleep_for(300ms);
    std::cout << "v is: '" << shared->v << "'\n";
  }

  void update(int i) {
    Node *oldN;
    auto *newN = new Node(i);
    {
      std::unique_lock lock(mtx);
      oldN = std::exchange(shared, newN);
    }
    delete oldN;
  }

private:
  Node *shared = new Node(9001);
  std::shared_mutex mtx;
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
