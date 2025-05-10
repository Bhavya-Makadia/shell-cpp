#include <iostream>
#include <string>
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

    if(input == "exit"){
      break;
    }

    cout << input << ": command not found" << endl;
  }
}
