#include <iostream>
#include <string>
#include <sstream>
using namespace std;

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


    if(input == "exit 0"){
      return 0;
    }

    istringstream iss(input);
    string command;
    iss >> command;
    if( command == "echo"){
      string output;
      getline(iss, output);
      cout << output.substr(1) << endl;
    } else {
      cout << input << ": command not found" << endl;
    }
    
  }
}
