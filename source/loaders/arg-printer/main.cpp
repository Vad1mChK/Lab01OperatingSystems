//
// Created by vadim on 13.10.2024.
//

#include <iostream>

int main(int argc, char** argv) {
  std::cout << "Args: " << std::endl;
  for (int i = 1; i < argc; ++i) {
    std::cout << "- " << argv[i] << std::endl;
  }
  return 0;
}