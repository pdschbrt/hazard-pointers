#include <atomic>
#include <cstddef>
#include <functional>
#include <iostream>
#include <mutex>
#include <numeric>
#include <thread>
#include <vector>

std::atomic<int> sumA(0);

int sumB;
std::mutex sumBMtx;

void partialSumImplA(const std::vector<int> &v, std::size_t startIdx,
                     std::size_t endIdx) {
  for (std::size_t i = startIdx; i < endIdx; ++i) {
    sumA += v[i];
  }
}

void partialSumImplB(const std::vector<int> &v, std::size_t startIdx,
                     std::size_t endIdx) {
  int partialSum = 0;
  for (std::size_t i = startIdx; i < endIdx; ++i) {
    partialSum += v[i];
  }
  std::lock_guard lock(sumBMtx);
  sumB += partialSum;
}

void computeSum(void (*partialSum)(const std::vector<int> &, std::size_t,
                                   std::size_t),
                const std::vector<int> &v, unsigned nThreads) {
  std::size_t chunks = v.size() / nThreads;
  std::vector<std::jthread> threads;
  threads.resize(nThreads);
  for (int i = 0; i < nThreads; ++i) {
    size_t startIdx = i * chunks;
    size_t endIdx = (i == nThreads - 1) ? v.size() : startIdx + chunks;
    threads.emplace_back(partialSum, std::cref(v), startIdx, endIdx);
  }
}

int main() {
  constexpr unsigned nThreads = 8;
  constexpr std::size_t size = 1'000;
  std::vector<int> elements(size);
  std::iota(elements.begin(), elements.end(), 1);
  computeSum(&partialSumImplA, elements, nThreads);
  computeSum(&partialSumImplB, elements, nThreads);
  std::cout << "sumA is: " << sumA << '\n';
  std::cout << "sumB is: " << sumB << '\n';
  return 0;
}
