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
#include <sys/types.h>   // pid_t
#include <sys/wait.h>    // waitpid
#include <unistd.h>
#include <sstream>
#include <iterator>

using namespace std;
void run_command(const string &cmd);
vector<std::string> split_by_pipe(const std::string& str);
vector<char*> parse_args(const std::string& cmd);
void execute_pipeline(const std::string& input);
int run_builtin_or_exec(const string &cmd, int output_fd, int input_fd);
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
  int last_appended_index = 0;
  using_history();
  char* histfile = getenv("HISTFILE");
    if (histfile != NULL) {
        read_history(histfile);
        // Populate commandHistory vector
        HIST_ENTRY** history_list_ptr = history_list();
        int hist_length = history_length;
        for (int i = 0; i < hist_length; i++) {
            commandHistory.push_back(history_list_ptr[i]->line);
        }
        last_appended_index = commandHistory.size(); // Update last_appended_index
    }
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
    add_history(input.c_str());

    if (input.find('|') != string::npos) {
    execute_pipeline(input); // run pipeline
    continue; // skip rest of loop since it's handled
    }

    string exe = extractExecutable(input);
    commandHistory.push_back(input);
    if (input == "exit 0")
    {
      if (histfile != NULL) {
        ofstream file(histfile, ios_base::app);
        if (file.is_open()) {
            for (int i = last_appended_index; i < commandHistory.size(); i++) {
                file << commandHistory[i] << endl;
            }
            file.close();
        }
    }
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
      if (space != string::npos && input.substr(space + 1, 2) == "-w") {
        size_t file_space = input.find(' ', space + 1);
        if (file_space != string::npos) {
            string history_file = input.substr(file_space + 1);
            ofstream file(history_file);
            if (file.is_open()) {
                for (const auto& command : commandHistory) {
                    file << command << endl;
                }
                file.close();
            } else {
                cout << "Unable to open history file" << endl;
            }
        } else {
            cout << "history: -w requires a file name" << endl;
        }
    } else if (space != string::npos && input.substr(space + 1, 2) == "-a") {
        size_t file_space = input.find(' ', space + 1);
        if (file_space != string::npos) {
            string history_file = input.substr(file_space + 1);
            ofstream file(history_file, ios_base::app);
            if (file.is_open()) {
                for (int i = last_appended_index; i < commandHistory.size(); i++) {
                    file << commandHistory[i] << endl;
                }
                last_appended_index = commandHistory.size();
                file.close();
            } else {
                cout << "Unable to open history file" << endl;
            }
        } else {
            cout << "history: -a requires a file name" << endl;
        }
        if (commandHistory.empty() || commandHistory.back().find("history -a") != 0) {
            add_history(input.c_str());
            commandHistory.push_back(input);
        }
    } else if (space != string::npos && input.substr(space + 1, 2) == "-r") {
        size_t file_space = input.find(' ', space + 1);
        if (file_space != string::npos) {
            string history_file = input.substr(file_space + 1);
            ifstream file(history_file);
            string line;
            while (getline(file, line)) {
                add_history(line.c_str());
                commandHistory.push_back(line);
            }
            file.close();
        } else {
            cout << "history: -r requires a file name" << endl;
        }
    }  else {
      string numStr = (space == string::npos) ? "" : input.substr(space + 1);
      int num = numStr.empty() ? commandHistory.size() : stoi(numStr);
      for (int i = commandHistory.size() - num; i < commandHistory.size(); i++){
        cout << i + 1 << " " << commandHistory[i] << endl;
      }
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

void run_command(const string &cmd) {
    // Built-in handling as before...
    if (cmd.rfind("echo", 0) == 0) {
        string echoOutput = handleQuote(cmd.substr(5));
        cout << echoOutput << endl;
        exit(0);
    }
    else if (cmd == "pwd") {
        cout << filesystem::current_path().string() << endl;
        exit(0);
    }
    else if (cmd.rfind("type", 0) == 0) {
        string command = cmd.substr(5);
        if (commands.find(command) != commands.end())
            cout << command << " is a shell builtin" << endl;
        else {
            string executable_path = find_executable_path(command);
            if (!executable_path.empty())
                cout << command << " is " << executable_path << endl;
            else
                cout << command << ": not found" << endl;
        }
        exit(0);
    }

    // Tokenize command
    istringstream iss(cmd);
    vector<string> tokens{istream_iterator<string>{iss}, istream_iterator<string>{}};
    if (tokens.empty()) exit(0);

    // Convert to char* array for execvp
    vector<char*> argv;
    for (auto& token : tokens) argv.push_back(const_cast<char*>(token.c_str()));
    argv.push_back(nullptr);

    // Print arguments for debugging (optional)
    // cerr << "execvp: [";
    // for (size_t i = 0; i < argv.size() - 1; ++i) {
    //     if (i > 0) cerr << ", ";
    //     cerr << argv[i];
    // }
    // cerr << "]" << endl;

    // Proper execvp call
    execvp(argv[0], argv.data());

    // If exec fails
    perror("execvp");
    exit(1);
}

std::vector<std::string> split_by_pipe(const std::string& str) {
    std::vector<std::string> result;
    std::stringstream ss(str);
    std::string segment;
    while (std::getline(ss, segment, '|')) {
        // Remove leading/trailing spaces
        size_t first = segment.find_first_not_of(" \t");
        size_t last = segment.find_last_not_of(" \t");
        if (first != std::string::npos)
            segment = segment.substr(first, last-first+1);
        result.push_back(segment);
    }
    return result;
}

// Helper: parse command string into argv
std::vector<char*> parse_args(const std::string& cmd) {
    std::istringstream iss(cmd);
    std::vector<std::string> tokens;
    std::string token;
    while (iss >> token)
        tokens.push_back(token);
    std::vector<char*> args;
    for (auto& t : tokens) args.push_back(&t[0]);
    args.push_back(nullptr); // exec needs a nullptr-terminated array
    return args;
}

void execute_pipeline(const std::string& input) {
    std::vector<std::string> commands = split_by_pipe(input);
    int n = commands.size();
    int prev_fd = -1;    // For read end of previous pipe
    std::vector<int> pids;
  int pipe_fd{0};
    for (int i = 0; i < n; ++i) {
        
        if (i != n - 1) {
            if (pipe(pipe_fd) == -1) {
                perror("pipe");
                return;
            }
        }

        pid_t pid = fork();
        if (pid == -1) {
            perror("fork");
            return;
        } else if (pid == 0) {
            // Child process
            // If not the first command, redirect stdin to prev_fd
            if (i > 0) {
                dup2(prev_fd, 0);
                close(prev_fd);
            }
            // If not the last command, redirect stdout to pipe_fd[1]
            if (i != n - 1) {
                close(pipe_fd);  // Close unused read end
                dup2(pipe_fd, 1);
                close(pipe_fd);
            }

            // Parse args and exec
            auto args = parse_args(commands[i]);
            execvp(args, args.data());
            perror("execvp");
            exit(1);
        } else {
            // Parent process
            if (prev_fd != -1)
                close(prev_fd);     // Close old input
            if (i != n - 1) {
                close(pipe_fd); // Parent doesn't write
                prev_fd = pipe_fd; // Save read end for next child
            }
            pids.push_back(pid);
        }
    }
    // Wait for all children
    int status;
    for (int pid : pids)
        waitpid(pid, &status, 0);
}

int run_builtin_or_exec(const string &cmd, int output_fd, int input_fd) {
    // Apply input redirection if needed
    if (input_fd != STDIN_FILENO) {
        if (dup2(input_fd, STDIN_FILENO) == -1) {
            perror("dup2");
            exit(1);
        }
        close(input_fd);
    }
    // Apply output redirection if needed
    if (output_fd != STDOUT_FILENO) {
        if (dup2(output_fd, STDOUT_FILENO) == -1) {
            perror("dup2");
            exit(1);
        }
        close(output_fd);
    }

    // Built-ins
    if (cmd.rfind("echo", 0) == 0) {
        string echoOutput = handleQuote(cmd.substr(5));
        echoOutput += "\n";
        write(STDOUT_FILENO, echoOutput.c_str(), echoOutput.size());
        return 0;
    }
    else if (cmd == "pwd") {
        string path = filesystem::current_path().string() + "\n";
        write(STDOUT_FILENO, path.c_str(), path.size());
        return 0;
    }
    else if (cmd.rfind("type", 0) == 0) {
        string command = cmd.substr(5);
        string output;
        if (commands.find(command) != commands.end()) {
            output = command + " is a shell builtin\n";
        } else {
            string executable_path = find_executable_path(command);
            if (!executable_path.empty())
                output = command + " is " + executable_path + "\n";
            else
                output = command + ": not found\n";
        }
        write(STDOUT_FILENO, output.c_str(), output.size());
        return 0;
    }

    // External command
    vector<char*> argv;
    istringstream iss(cmd);
    string token;
    while (iss >> token) argv.push_back(strdup(token.c_str()));
    argv.push_back(nullptr);
    execvp(argv[0], argv.data());
    perror("execvp");
    exit(1);
}