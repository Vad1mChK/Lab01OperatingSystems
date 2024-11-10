#include <iostream>

#include "../shell/Shell.hpp"

namespace monolith::app {
void Main() {
  auto shell = Shell();
  shell.Start();
}
}  // namespace monolith::app

int main() {
  monolith::app::Main();
}