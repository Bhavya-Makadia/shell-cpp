#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
using namespace std;

string trimInitialWhitespace(string str) {
  str.erase(0, str.find_first_not_of(" \t\n\r"));
  return str;
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
      bool found = false;
      size_t num_builtins = sizeof(shell_builtins) / sizeof(shell_builtins[0]);
      for ( size_t i = 0; i < num_builtins; i++){
        if ( output == shell_builtins[i]){
          cout << output << " is a shell builtin"<< endl;
          found = true;
          break;
        } 
      }
      if (!found) {
        cout << output << ": not found" << endl;
      }
    } else {
      cout << input << ": command not found" << endl;
    }
    
  }
}
