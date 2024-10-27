#ifndef COMMAND_FACTORY_HPP
#define COMMAND_FACTORY_HPP

#include <memory>
#include <string>
#include <vector>

#include "Command.hpp"

// Factory function declaration
std::unique_ptr<Command> CommandFactory(
    const std::string& command_name, const std::vector<std::string>& args
);

std::vector<std::string> SplitString(const std::string& inp);

bool IsBlank(const std::string& inp);

std::string StringToLower(const std::string& inp);

#endif  // COMMAND_FACTORY_HPP