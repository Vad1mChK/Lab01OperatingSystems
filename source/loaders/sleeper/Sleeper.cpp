//
// Created by vadim on 13.10.2024.
//
#include "Sleeper.hpp"

#include <iostream>
#include <thread>

void sleeper(uint32_t milliseconds) {
  std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cout << "Wrong arguments count (1 expected). ";
    std::cout << "Usage: " << argv[0] << " <milliseconds>" << '\n';
    return 1;
  }

  int duration = 0;
  try {
    duration = std::stoi(argv[1]);
    if (duration < 0) {
      std::cout << "Cannot sleep for a negative duration (time travels are not invented yet!)"
                << '\n';
      return 2;
    }
  } catch (const std::invalid_argument& e) {
    std::cout << "The argument could not be converted to a number." << '\n';
    return 3;
  } catch (const std::out_of_range& e) {
    std::cout << "The argument could not be converted to a number." << '\n';
    return 3;
  } catch (const std::exception& e) {
    std::cout << "An unknown exception has occurred while running." << '\n';
    return 4;
  }

  std::cout << "Sleeping for " << duration << "ms... " << '\n';
  sleeper(duration);
  std::cout << "Waking up." << '\n';
  return 0;
}