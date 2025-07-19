#include <iostream>
#include <string>
#include <map>
#include <unistd.h>
#include <cstring>

std::string find_executable_path(std::string command);


int main() {
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  std::string input;

  std::map<std::string, bool> commands = {
    {"echo", true},
    {"exit", true},
    {"type", true}
  };

  std::cout << "$ ";

  while (std::getline(std::cin, input)) {
    if (input == "exit 0") {
      exit(0);
    } else if (input.compare(0, 4, "echo") == 0) {
      std::cout << input.substr(5) << std::endl;
    } else if (input.compare(0, 4, "type") == 0) {
      std::string command = input.substr(5);
      std::string executable_path = find_executable_path(command);
      
      if (commands.find(command) != commands.end()) {
        std::cout << command << " is a shell builtin" << std::endl;
      } else if(executable_path != "") {
        std::cout << command << " is " << executable_path << std::endl;
      } else {
        std::cout << command << ": not found" << std::endl;
      }
    } else if (find_executable_path(input.substr(0, input.find(" "))) != "") {
      std::system(input.c_str());
    } else {
      std::cout << input << ": command not found" << std::endl;
    }

    std::cout << "$ ";
  }
}

std::string find_executable_path(std::string command) {
  char *path = getenv("PATH");
  char *path_copy = strdup(path);
  char *token = strtok(path_copy, ":");
  while (token != NULL) {
    std::string curPath = token;
    curPath += "/" + command;
    if (access(curPath.c_str(), X_OK) == 0) {
      free(path_copy);
      return curPath;
    }
    token = strtok(NULL, ":");
  }
  free(path_copy);
  return "";
}