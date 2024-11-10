#include <algorithm>
#include <chrono>
#include <iostream>
#include <vector>
#include <random>

const size_t Million = 1'000'000;

int main() {
  size_t data_size = Million;  // 1 million elements
  std::vector<uint32_t> data(data_size);
  // Initialize data with random numbers
  for (auto& val : data) {
    val = random();
  }

  auto start = std::chrono::high_resolution_clock::now();
  std::sort(data.begin(), data.end());
  auto end = std::chrono::high_resolution_clock::now();

  std::chrono::duration<double> elapsed = end - start;
  std::cout << "Time taken to sort " << data_size << " elements: " << elapsed.count() << " seconds\n";

  return 0;
}
