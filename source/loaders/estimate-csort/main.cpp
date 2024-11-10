#include <algorithm>
#include <chrono>
#include <iostream>
#include <vector>

int main() {
  size_t N = 1'000'000;  // 1 million elements
  std::vector<uint32_t> data(N);
  // Initialize data with random numbers
  for (auto& val : data) {
    val = rand();
  }

  auto start = std::chrono::high_resolution_clock::now();
  std::sort(data.begin(), data.end());
  auto end = std::chrono::high_resolution_clock::now();

  std::chrono::duration<double> elapsed = end - start;
  std::cout << "Time taken to sort " << N << " elements: " << elapsed.count() << " seconds\n";

  return 0;
}
