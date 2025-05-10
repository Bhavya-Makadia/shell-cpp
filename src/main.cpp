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

    stringstream ss(input);
    string command, argument;
    ss >> command >> argument;


    if(command == "exit"){
      if(argument == "0"){
        exit(0);
      }
      break;
    }

    cout << input << ": command not found" << endl;
  }
}
