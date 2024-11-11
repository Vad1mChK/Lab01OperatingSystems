#include <gtest/gtest.h>

#include <sstream>

#include "common/unistd_check.hpp"
#include "monolith/shell/Shell.hpp"

class ShellTestSuite : public ::testing::Test {
protected:
  std::stringstream mock_input;
  std::stringstream mock_output;
  Shell* shell;

  void SetUp() override {
    mock_input.str("");   // Clear input
    mock_output.str("");  // Clear output
    shell = new Shell(mock_input, mock_output);
    ASSERT_NE(shell, nullptr) << "Shell is null.";
  }

  void TearDown() override {
    delete shell;
    shell = nullptr;
    ASSERT_EQ(shell, nullptr) << "Shell was not deleted.";
  }
};

TEST_F(ShellTestSuite, ShellInitialization) {
  mock_input.str("exit\n");  // Simulate input to exit the shell
  EXPECT_NO_THROW(shell->Start());
  EXPECT_NE(mock_output.str().find("Starting up shell."), std::string::npos);
}

TEST_F(ShellTestSuite, CorrectlyDetectPlatform) {
  mock_input.str("exit\n");
  EXPECT_NO_THROW(shell->Start());
  std::string platformName = (
#ifdef UNISTD_AVAILABLE
      "Unix"
#elif defined(_WIN32)
      "Windows"
#else
      "unknown"
#endif
  );
  EXPECT_NE(mock_output.str().find(platformName), std::string::npos);
}

TEST_F(ShellTestSuite, ExecuteBasicCommand) {
  mock_input.str("pwd\nexit\n");  // Simulate user input
  EXPECT_NO_THROW(shell->Start());
  EXPECT_NE(mock_output.str().find("Current working directory is:"), std::string::npos);
}

TEST_F(ShellTestSuite, ExecuteUnknownCommand) {
  mock_input.str("unknown_command\nexit\n");
  EXPECT_NO_THROW(shell->Start());
  EXPECT_NE(mock_output.str().find("Unknown command: `unknown_command`"), std::string::npos);
}

TEST_F(ShellTestSuite, VerifyCommandHistory) {
  mock_input.str("ls\npwd\nexit\n");
  EXPECT_NO_THROW(shell->Start());
  auto& history = shell->GetHistory();
  EXPECT_EQ(history.size(), 3);  // Includes 'ls', 'pwd', and 'exit'
  EXPECT_EQ(history.top(), "exit");
}