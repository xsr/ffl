#include <iostream>
#include <sstream>
#include <fstream>

#include "lib.hpp"

std::string func_src = "() recv_internal(int msg_value, cell in_msg_full, slice in_msg_body) impure { \
  throw(0xffff); \
}";

int main() {
  // std::stringstream stringstream;
  // stringstream << std::ifstream("./test.fc").rdbuf();

  std::cout << "test func source:\n===================\n";
  // std::cout << stringstream.str() << std::endl;
  std::cout << func_src << std::endl;

  // auto generated = generate(stringstream.str());
  auto generated = generate(func_src);

  std::cout << "\ngenerated fift:\n==================\n";
  std::cout << generated << std::endl;

  std::string script = generated + "2 boc+>B dup Bx.\n";
  auto interpreted = interpret(script);

  std::cout << "interpreted to boc:\n===============\n";

  std::cout << interpreted << std::endl;

  std::cout << "\nbye\n";

  return 0;
}
