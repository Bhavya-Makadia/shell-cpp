#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <vector>
#include <filesystem>
#include <cstdlib>
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
        char *pathEnv = getenv ("PATH");
        if( pathEnv != nullptr){
          string pathEnvStr(pathEnv);
          stringstream ss(pathEnvStr);

          string segment;
          vector<string> paths;

          while(getline(ss, segment, ':')){
            if(!segment.empty()){
              paths.push_back(segment);
            }
          }       

          for ( auto& dir : paths) {
            if (is_command_executable(dir, output)) {
              std::cout << output << " is " << dir << std::endl;
              is_Executable = true;
              break;
            }
          }

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
