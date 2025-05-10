#include <iostream>
using namespace std;

int main() {
  // Flush after every std::cout / std:cerr
  cout << std::unitbuf;
  cerr << std::unitbuf;

  cout << "$ ";

  string input;
  getline(cin, input);

  cout<< input << ": command not found" << endl;
}
