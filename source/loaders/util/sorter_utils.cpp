#include "sorter_utils.hpp"

#include <random>

uint32_t RandomUint32() {
  static std::random_device random_device;
  static std::mt19937 engine(random_device());
  static std::uniform_int_distribution<uint32_t> dist(0, std::numeric_limits<uint32_t>::max());
  return dist(engine);
}

std::string SanitizeInputFilename(const std::string& input_filename) {
  std::string sanitized = input_filename;
  std::replace(sanitized.begin(), sanitized.end(), '/', '_'); // Replace '/' with '_'
  return sanitized;
}