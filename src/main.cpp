#include <iostream>
#include <string>
#include <map>
#include <unistd.h>
#include <cstring>
#include <filesystem>
#include <fstream>

using namespace std;

string find_executable_path(string command);
string extractExecutable(string &input);
string handleQuote(string echoInput);

int main()
{
  // Flush after every std::cout / std:cerr
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
      {"wait", true}};

  cout << "$ ";

  while (getline(cin, input))
  {

    string exe = extractExecutable(input);

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
if (redirect_pos >= 2 && echoInput.substr(redirect_pos-1, 2) == "2>") {
        path = echoInput.substr(redirect_pos+1);
        raw = echoInput.substr(0, redirect_pos-2);
        // remove leading and trailing spaces from path
        while (path.size() > 0 && path[0] == ' ') {
            path = path.substr(1);
        }
        while (path.size() > 0 && path[path.size() - 1] == ' ') {
            path = path.substr(0, path.size() - 1);
        }
        string echoOutput = handleQuote(raw);
        ofstream file(path);
        file << echoOutput << endl;
        file.close();
    } else {
        if (redirect_pos >= 2 && echoInput.substr(redirect_pos-1, 2) == "1>") {
            path = echoInput.substr(redirect_pos+1);
            raw = echoInput.substr(0, redirect_pos-2);
        } else {
            path = echoInput.substr(redirect_pos+1);
            raw = echoInput.substr(0, redirect_pos);
        }
        // remove leading and trailing spaces from path
        while (path.size() > 0 && path[0] == ' ') {
            path = path.substr(1);
        }
        while (path.size() > 0 && path[path.size() - 1] == ' ') {
            path = path.substr(0, path.size() - 1);
        }
        string echoOutput = handleQuote(raw);
        ofstream file(path);
        file << echoOutput << endl;
        file.close();
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
    }
    else
    {
      cout << input << ": command not found" << endl;
    }

    cout << "$ ";
  }
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