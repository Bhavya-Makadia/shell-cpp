#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <unistd.h>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <set>
#include <readline/readline.h>
#include <readline/history.h>

using namespace std;

string find_executable_path(string command);
string extractExecutable(string &input);
string handleQuote(string echoInput);
char* command_generator(const char* text, int state);
char** command_completion(const char* text, int start, int end);

set<string> commands = {"exit", "echo", "type", "pwd", "cd"};

int main()
{
  rl_attempted_completion_function = command_completion;
  // Flush after every std::cout / std:cerr

  vector<string> commandHistory;

  while(1){
  cout << unitbuf;
  cerr << unitbuf;

  string input;

  map<string, bool> commands = {
      {"alias", true},
      {"bg", true},
      {"bind", true},
      {"break", true},
      {"builtin", true},
      {"caller", true},
      {"cd", true},
      {"command", true},
      {"compgen", true},
      {"complete", true},
      {"compopt", true},
      {"continue", true},
      {"declare", true},
      {"dirs", true},
      {"disown", true},
      {"echo", true},
      {"enable", true},
      {"eval", true},
      {"exec", true},
      {"exit", true},
      {"export", true},
      {"false", true},
      {"fc", true},
      {"std::string", true},
      {"fg", true},
      {"getopts", true},
      {"hash", true},
      {"help", true},
      {"history", true},
      {"jobs", true},
      {"kill", true},
      {"let", true},
      {"local", true},
      {"logout", true},
      {"mapfile", true},
      {"popd", true},
      {"printf", true},
      {"pushd", true},
      {"pwd", true},
      {"read", true},
      {"readarray", true},
      {"readonly", true},
      {"return", true},
      {"set", true},
      {"shift", true},
      {"shopt", true},
      {"source", true},
      {"suspend", true},
      {"test", true},
      {"times", true},
      {"trap", true},
      {"true", true},
      {"type", true},
      {"typeset", true},
      {"ulimit", true},
      {"umask", true},
      {"unalias", true},
      {"unset", true},
      {"wait", true}
    };

    char* input_cstr = readline("$ ");
    if (!input_cstr) { // Check for EOF or error
      break;
    }
    input = input_cstr;
    free(input_cstr);
  
    if (input.empty()) { // Check for empty input
      continue;
    }

    string exe = extractExecutable(input);
    commandHistory.push_back(input);
    if (input == "exit 0")
    {
      exit(0);
    }
    else if (input.compare(0, 4, "echo") == 0)
    {
      string echoInput = input.substr(5);
      size_t redirect_pos = echoInput.find('>');
      if (redirect_pos != string::npos)
      {
        string path;
        string raw;
        
        if (redirect_pos >= 2 && echoInput.substr(redirect_pos, 2) == ">>")
        {
          if (redirect_pos >= 2 && echoInput.substr(redirect_pos - 1, 3) == "2>>")
          {
            path = echoInput.substr(redirect_pos + 2);
            raw = echoInput.substr(0, redirect_pos - 2);
            // remove leading and trailing spaces from path
            while (path.size() > 0 && path[0] == ' ')
            {
              path = path.substr(1);
            }
            while (path.size() > 0 && path[path.size() - 1] == ' ')
            {
              path = path.substr(0, path.size() - 1);
            }
            string echoOutput = handleQuote(raw);
            ofstream err(path);
            cout << echoOutput << endl;
            if (err.is_open())
            {
              err.close();
            }
          }
          else
          {
            if (redirect_pos >= 2 && echoInput.substr(redirect_pos - 1, 3) == "1>>")
            {
              path = echoInput.substr(redirect_pos + 2);
              raw = echoInput.substr(0, redirect_pos - 2);
            }
            else
            {
              path = echoInput.substr(redirect_pos + 2);
              raw = echoInput.substr(0, redirect_pos - 1);
            }
            // remove leading and trailing spaces from path
            while (path.size() > 0 && path[0] == ' ')
            {
              path = path.substr(1);
            }
            while (path.size() > 0 && path[path.size() - 1] == ' ')
            {
              path = path.substr(0, path.size() - 1);
            }
            
            string echoOutput = handleQuote(raw);
            ofstream file(path, ios::app);
            file << echoOutput << endl;
            file.close();
          }
        }
        else
        {
          if (redirect_pos >= 2 && echoInput.substr(redirect_pos - 1, 2) == "2>")
          {
            path = echoInput.substr(redirect_pos + 1);
            raw = echoInput.substr(0, redirect_pos - 2);
            // remove leading and trailing spaces from path
            while (path.size() > 0 && path[0] == ' ')
            {
              path = path.substr(1);
            }
            while (path.size() > 0 && path[path.size() - 1] == ' ')
            {
              path = path.substr(0, path.size() - 1);
            }
            string echoOutput = handleQuote(raw);
            ofstream err(path);
            cout << echoOutput << endl;
            if (err.is_open())
            {
              err.close();
            }
          }
          else
          {
            if (redirect_pos >= 2 && echoInput.substr(redirect_pos - 1, 2) == "1>")
            {
              path = echoInput.substr(redirect_pos + 1);
              raw = echoInput.substr(0, redirect_pos - 2);
            }
            else
            {
              path = echoInput.substr(redirect_pos + 1);
              raw = echoInput.substr(0, redirect_pos - 1);
            }
            // remove leading and trailing spaces from path
            while (path.size() > 0 && path[0] == ' ')
            {
              path = path.substr(1);
            }
            while (path.size() > 0 && path[path.size() - 1] == ' ')
            {
              path = path.substr(0, path.size() - 1);
            }
            string echoOutput = handleQuote(raw);
            ofstream file(path);
            file << echoOutput << endl;
            file.close();
          }
        }
      }
      else
      {
        string echoOutput = handleQuote(echoInput);
        cout << echoOutput << endl;
      }
    }
    else if (input.compare(0, 4, "type") == 0)
    {
      string command = input.substr(5);
      string executable_path = find_executable_path(command);

      if (commands.find(command) != commands.end())
      {
        cout << command << " is a shell builtin" << endl;
      }
      else if (executable_path != "")
      {
        cout << command << " is " << executable_path << endl;
      }
      else
      {
        cout << command << ": not found" << endl;
      }
    }
    else if (find_executable_path(input.substr(0, input.find(" "))) != "")
    {
      system(input.c_str());
    }
    else if (find_executable_path(exe) != "")
    {
      system(input.c_str());
    }
    else if (input.compare(0, 3, "pwd") == 0)
    {
      filesystem::path curr_dir = filesystem::current_path();
      cout << curr_dir.string() << endl;
    }
    else if (input.compare(0, 2, "cd") == 0)
    {
      string path = input.substr(3);
      if (path[0] == '~')
      {
        path = string(getenv("HOME"));
      }
      if (chdir(path.c_str()) != 0)
      {
        cout << "cd: " << path << ": No such file or directory" << endl;
      }
    } else if (input.compare(0, 7, "history") == 0){
      size_t space = input.find(' ');
      string numStr = (space == string::npos) ? "" : input.substr(space + 1);
      int num = numStr.empty() ? commandHistory.size() : stoi(numStr);
      for (int i = commandHistory.size() - num; i < commandHistory.size(); i++){
        cout << i + 1 << " " << commandHistory[i] << endl;
      }
    } else
    {
      cout << input << ": command not found" << endl;
    }


  }
}

std::set<std::string> get_executables_from_path() {
    std::set<std::string> executables;
    char* path = getenv("PATH");
    char* path_copy = strdup(path);
    char* token = strtok(path_copy, ":");
    while (token != NULL) {
        std::string cur_path = token;
        for (const auto& entry : std::filesystem::directory_iterator(cur_path)) {
            if (entry.is_regular_file() && (entry.status().permissions() & std::filesystem::perms::owner_exec) != std::filesystem::perms::none) {
                executables.insert(entry.path().filename().string());
            }
        }
        token = strtok(NULL, ":");
    }
    free(path_copy);
    return executables;
}

std::set<std::string> executables = get_executables_from_path();
char* command_generator(const char* text, int state) {
   static std::set<std::string>::iterator it_builtin;
    static std::set<std::string>::iterator it_executable;
    static std::string prefix;

    if (state == 0) {
        it_builtin = commands.begin();
        it_executable = executables.lower_bound(text);
        prefix = text;
    }

    // Check built-in commands
    while (it_builtin != commands.end()) {
        const std::string& cmd = *it_builtin;
        ++it_builtin;

        if (cmd.find(prefix) == 0) {
            return strdup(cmd.c_str());
        }
    }

    // Check executables
    while (it_executable != executables.end()) {
        const std::string& cmd = *it_executable;
        ++it_executable;

        if (cmd.find(prefix) == 0) {
            return strdup(cmd.c_str());
        } else {
            break;  // Since executables are sorted, we can stop once we find a command that doesn't match the prefix
        }
    }

    return nullptr;
}

char** command_completion(const char* text, int start, int end) {
  rl_attempted_completion_over = 1; // prevent readline from filename completion
  return rl_completion_matches(text, command_generator);
}

string handleQuote(string echoInput)
{
  string output;
  bool isSingleQoute = false;
  bool isDoubleQoute = false;
  bool prevSpace = false;
  bool isbackSlash = false;

  for (char c : echoInput)
  {
    if (isbackSlash)
    {
      output += c;
      isbackSlash = false;
      prevSpace = (c == ' ');
    }
    else if (c == '\\' && !isSingleQoute)
    {
      isbackSlash = true;
    }
    else if (c == '\'' && !isDoubleQoute)
    {
      isSingleQoute = !isSingleQoute;
    }
    else if (c == '"' && !isSingleQoute)
    {
      isDoubleQoute = !isDoubleQoute;
    }
    else if (!isSingleQoute && !isDoubleQoute && (c == ' ' && prevSpace))
    {
      continue;
    }
    else if (!isSingleQoute && !isDoubleQoute && c == ' ')
    {
      output += c;
      prevSpace = true;
    }
    else
    {
      prevSpace = false;
      output += c;
    }
  }
  return output;
}

string extractExecutable(string &input)
{
  size_t i = 0;
  while (i < input.size() && isspace(input[i]))
  {
    i++;
  }

  if (i == input.size())
  {
    return "";
  }

  char quoteChar = '\0';
  string exe;
  if (input[i] == '\'' || input[i] == '"')
  {
    quoteChar = input[i++];
    while (i < input.size() && input[i] != quoteChar)
    {
      exe += input[i++];
    }
    if (i < input.size() && input[i] == quoteChar)
      i++;
  }
  else
  {
    while (i < input.size() && !isspace(input[i]))
    {
      exe += input[i++];
    }
  }
  return exe;
}

string find_executable_path(string command)
{
  char *path = getenv("PATH");
  char *path_copy = strdup(path);
  char *token = strtok(path_copy, ":");
  while (token != NULL)
  {
    string curPath = token;
    curPath += "/" + command;
    if (access(curPath.c_str(), X_OK) == 0)
    {
      free(path_copy);
      return curPath;
    }
    token = strtok(NULL, ":");
  }
  free(path_copy);
  return "";
}