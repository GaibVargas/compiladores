#include <iostream>
#include <unordered_map>
#include <string>
#include <functional>
#include <vector>
#include <set>
#include "fsm.cpp"

int main() {
  auto fsm = FiniteStateMachine(
    std::unordered_map<std::string, std::function<std::string(char)>> {
      {"0", [](char c) {
        if (c == 'a') return std::string("1");
        return std::string("dead");
      }},
      {"1", [](char c) {
        if (c == 'b') return std::string("2");
        return std::string("dead");
      }},
      {"2", [](char c) {
        if (c == 'c') return std::string("final");
        return std::string("dead");
      }},
    },
    std::string("0"),
    std::set<std::string>{"final"}
  );
  std::cout << "Estado inicial " << fsm.get_status() << std::endl;
  fsm.transition('x');
  fsm.transition('b');
  fsm.transition('c');
  fsm.transition('c');
  fsm.transition('d');
  std::cout << "Estado final " << fsm.get_status() << std::endl;
  std::cout << "Lexema " << fsm.get_lexeme() << std::endl;
  fsm.reset();
  std::cout << "Estado depois do reset " << fsm.get_status() << std::endl;
  fsm.transition('a');
  fsm.transition('b');
  fsm.transition('c');
  std::cout << "Estado final 2 " << fsm.get_status() << std::endl;
  std::cout << "Lexema " << fsm.get_lexeme() << std::endl;
  return 0;
}