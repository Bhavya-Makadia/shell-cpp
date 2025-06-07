#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <vector>
#include <filesystem>
#include <cstdlib>
#include <fstream>
using namespace std;

string trimInitialWhitespace(string str) {
  str.erase(0, str.find_first_not_of(" \t\n\r"));
  return str;
}

bool is_command_executable(string& dir, string& output){
  if(dir.length() < output.length()) return false;

   size_t last_slash = dir.find_last_of("/\\");
   string executable_file = dir.substr(last_slash + 1);


   return executable_file == output;
}

vector<string> split_path(const string& path) {
    vector<string> directories;
    stringstream ss(path);
    string dir;
    while (getline(ss, dir, ';')) {
        directories.push_back(dir);
    }
    return directories;
}

bool is_executable(const string& path) {
    ifstream file(path);
    return file.good();
}

string find_command_in_path(const string& command) {
    const char* path_env = std::getenv("PATH");
    if (!path_env) return "";

    vector<string> path_dirs = split_path(path_env);
    for (const auto& dir : path_dirs) {
        string full_path = dir + "/" + command;
        if (is_executable(full_path)) {
            return full_path;
        }
    }
    return "";
}

int main()
{
  // Flush after every std::cout / std:cerr
  cout << std::unitbuf;
  cerr << std::unitbuf;
  string input;

  while (true)
  {
    cout << "$ "; 
    getline(cin, input);

    istringstream iss(input);
    string command;
    iss >> command;

    string shell_builtins[] = {
    "alias", "bg", "bind", "break", "builtin", "caller", "cd", "command",
    "compgen", "complete", "compopt", "continue", "declare", "dirs",
    "disown", "echo", "enable", "eval", "exec", "exit", "export", "false",
    "fc", "fg", "getopts", "hash", "help", "history", "jobs", "kill", "let",
    "local", "logout", "mapfile", "popd", "printf", "pushd", "pwd", "read",
    "readarray", "readonly", "return", "set", "shift", "shopt", "source",
    "suspend", "test", "times", "trap", "true", "type", "typeset", "ulimit",
    "umask", " nalias", "unset", "wait"
    };

    if(input == "exit 0"){
      return 0;
    } else if( command == "echo"){
      string output;
      getline(iss, output);
      cout << output.substr(1) << endl;
    } else if(  command == "type" ) {
      string output;
      getline(iss, output);

      output = trimInitialWhitespace(output);
      bool is_builtin = false;
      bool is_Executable = false;

      size_t num_builtins = sizeof(shell_builtins) / sizeof(shell_builtins[0]);
      for ( size_t i = 0; i < num_builtins; i++){
        if ( output == shell_builtins[i]){
          cout << output << " is a shell builtin"<< endl;
          is_builtin = true;
          break;
        } 
      }
      if (!is_builtin) {
       string path = find_command_in_path(output);
        if (!path.empty()) {
          std::cout << command << " is " << path << std::endl;
          is_Executable = true;
        }
      }

      if (!is_builtin && !is_Executable) {
        cout << output << ": not found" << endl;
      }
    } else {
      cout << input << ": command not found" << endl;
    }
    
  }
}

