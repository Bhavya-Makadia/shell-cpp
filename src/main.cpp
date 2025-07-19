#include <iostream>
#include <string>
#include <map>
#include <unistd.h>
#include <cstring>
#include <filesystem>

using namespace std;

string find_executable_path(std::string command);


int main() {
  // Flush after every std::cout / std:cerr
  cout << std::unitbuf;
  cerr << std::unitbuf;

  string input;

  map<std::string, bool> commands = {
    {"echo", true},
    {"exit", true},
    {"type", true}
  };

  cout << "$ ";

  while (getline(cin, input)) {
    if (input == "exit 0") {
      exit(0);
    } else if (input.compare(0, 4, "echo") == 0) {
      cout << input.substr(5) << endl;
    } else if (input.compare(0, 4, "type") == 0) {
      string command = input.substr(5);
      string executable_path = find_executable_path(command);
      
      if (commands.find(command) != commands.end()) {
        cout << command << " is a shell builtin" << endl;
      } else if(executable_path != "") {
        cout << command << " is " << executable_path << endl;
      } else {
        cout << command << ": not found" << endl;
      }
    } else if (find_executable_path(input.substr(0, input.find(" "))) != "") {
      system(input.c_str());
    } else if (input.compare(0, 3, "pwd") == 0) {
      cout << filesystem::current_path() << endl;
    } else {
      cout << input << ": command not found" << endl;
    }

    cout << "$ ";
  }
}

string find_executable_path(std::string command) {
  char *path = getenv("PATH");
  char *path_copy = strdup(path);
  char *token = strtok(path_copy, ":");
  while (token != NULL) {
    string curPath = token;
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