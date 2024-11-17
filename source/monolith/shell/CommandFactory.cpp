#include "CommandFactory.hpp"

#include <memory>
#include <regex>

#include "Command.hpp"
#include "commands/ChangeDirectoryCommand.hpp"
#include "commands/ExitCommand.hpp"
#include "commands/HelpCommand.hpp"
#include "commands/HistoryCommand.hpp"
#include "commands/ListFilesCommand.hpp"
#include "commands/PathToWorkingDirectoryCommand.hpp"
#include "commands/RunCommand.hpp"
#include "commands/RunNonBlockingCommand.hpp"

using CommandFactoryFunction =
    std::function<std::unique_ptr<Command>(const std::vector<std::string>&)>;

std::map<std::string, CommandFactoryFunction> CommandMap() {
  return {
      {   "help",
       [](const std::vector<std::string>& args) { return std::make_unique<HelpCommand>(args); }     },
      {   "exit",
       [](const std::vector<std::string>& args) { return std::make_unique<ExitCommand>(args); }     },
      {"history",
       [](const std::vector<std::string>& args) { return std::make_unique<HistoryCommand>(args); }  },
      {    "pwd",
       [](const std::vector<std::string>& args) {
       return std::make_unique<PathToWorkingDirectoryCommand>(args);
       }                                                                                            },
      {     "cd",
       [](const std::vector<std::string>& args) {
       return std::make_unique<ChangeDirectoryCommand>(args);
       }                                                                                            },
      {     "ls",
       [](const std::vector<std::string>& args) { return std::make_unique<ListFilesCommand>(args); }},
      {    "run",
       [](const std::vector<std::string>& args) { return std::make_unique<RunCommand>(args); }      },
      { "runnb",
       [](const std::vector<std::string>& args) { return std::make_unique<RunNonBlockingCommand>(args);}}
  };
}

std::unique_ptr<Command> CommandFactory(
    const std::string& command_name, const std::vector<std::string>& args
) {
  auto cmd_map = CommandMap();
  // Convert the command name to lowercase
  auto command_name_lowercase = StringToLower(command_name);

  // Try to find the command in the map
  auto entry = cmd_map.find(command_name_lowercase);
  if (entry != cmd_map.end()) {
    // Call the factory function to create the command
    return entry->second(args);
  }

  // Return nullptr if the command is unknown
  return nullptr;
}

std::vector<std::string> SplitString(const std::string& inp) {
  std::vector<std::string> result;

  // Regular expression to match words or quoted phrases
  const std::regex rgx(R"((\"[^\"]*\"|\S+))");  // Matches either words or phrases inside quotes
  const std::sregex_iterator begin(inp.begin(), inp.end(), rgx);
  const std::sregex_iterator end;

  // Iterate over all matches
  for (auto it = begin; it != end; ++it) {
    std::string match = it->str();

    // If the match starts and ends with a quote, remove the quotes
    if (match.front() == '"' && match.back() == '"') {
      match = match.substr(1, match.size() - 2);  // Remove surrounding quotes
    }

    result.push_back(match);  // Add to the result
  }

  return result;
}

bool IsBlank(const std::string& inp) {
  return std::all_of(inp.cbegin(), inp.cend(), [](char chr) {
    return std::isspace(static_cast<unsigned char>(chr)) != 0;
  });
}

std::string StringToLower(const std::string& inp) {
  std::string res = inp;
  std::transform(res.begin(), res.end(), res.begin(), [](char chr) {
    return static_cast<char>(tolower(chr));
  });
  return res;
}
