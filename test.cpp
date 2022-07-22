#include <iostream>
#include <sstream>
#include <fstream>

#include "lib.hpp"

int main() {
  std::stringstream stringstream;
  stringstream << std::ifstream("./test.fc").rdbuf();

  std::cout << "test func source:\n===================\n";
  std::cout << stringstream.str() << std::endl;

  auto generated = generate(stringstream.str());

  std::cout << "\ngenerated fift:\n==================\n";
  std::cout << generated << std::endl;

  std::string script = generated + "2 boc+>B dup Bx.\n";
  auto interpreted = interpret(script);

  std::cout << "interpreted to boc:\n===============\n";

  std::cout << interpreted << std::endl;

  std::cout << "\nbye\n";

  return 0;
}
