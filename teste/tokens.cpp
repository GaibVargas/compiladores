#include <string>
#include <memory>
#include <fstream>
#include <iostream>
#include "fsm.cpp"

bool isAlpha(char ch) {
  return std::isalpha(static_cast<unsigned char>(ch));
}

bool isNumeric(char ch) {
  return std::isdigit(static_cast<unsigned char>(ch));
}

bool isWhitespace(char c) {
  return std::isspace(static_cast<unsigned char>(c));
}

struct Token
{
  std::string lexeme;
  std::string id;
  int line;
  int column;

  Token(const std::string &lexeme, const std::string &id, int line, int column)
      : lexeme(lexeme), id(id), line(line), column(column) {}
};

class TokenIdentifier
{
protected:
  std::string _id;
  std::unique_ptr<FiniteStateMachine> _identifier_machine;
  int _start;

public:
  TokenIdentifier(const std::string &id) : _id(id) {}
  std::string get_id() {
    return _id;
  }
  int get_start() {
    return _start;
  }
  void reset()
  {
    _identifier_machine->reset();
  }
  FSMStatus analyze(char c, int cursor_position)
  {
    auto prev_status = _identifier_machine->get_status();
    if (prev_status == FSMStatus::ERROR || prev_status == FSMStatus::SUCCESS)
      return prev_status;
    auto current_status = _identifier_machine->transition(c);
    if (prev_status == FSMStatus::IDLE && (current_status == FSMStatus::RUNNING || current_status == FSMStatus::SUCCESS))
      _start = cursor_position;
    return current_status;
  };
  std::string get_lexeme()
  {
    return _identifier_machine->get_lexeme();
  }
  bool has_already_succeeded() {
    return _identifier_machine->get_status() == FSMStatus::SUCCESS;
  }
  bool has_to_move_cursor_back()
  {
    return _identifier_machine->has_to_move_cursor_back();
  }
};

class IdentTokenIdentifier : public TokenIdentifier
{
  public:
    IdentTokenIdentifier(): TokenIdentifier("IDENT") {
      _identifier_machine = std::make_unique<FiniteStateMachine>(
        std::unordered_map<std::string, std::function<std::string(char)>>{
          {"0", [](char c) {
            if (isAlpha(c)) return "1";
            return "dead";
          }},
          {"1", [](char c) {
            if (isAlpha(c) || isNumeric(c)) return "1";
            return "final";
          }},
        },
        "0",
        std::set<std::string>{"final"},
        std::set<std::string>{"final"}
      );
    }
};

class IntegerTokenIdentifier : public TokenIdentifier
{
  public:
    IntegerTokenIdentifier(): TokenIdentifier("NI") {
      _identifier_machine = std::make_unique<FiniteStateMachine>(
        std::unordered_map<std::string, std::function<std::string(char)>>{
          {"0", [](char c) {
            if (isNumeric(c)) return "1";
            return "dead";
          }},
          {"1", [](char c) {
            if (isNumeric(c)) return "1";
            if (c == 'e') return "2";
            return "final";
          }},
          {"2", [](char c) {
            if (isNumeric(c)) return "3";
            if (c == '+' || c == '-') return "4";
            return "dead";
          }},
          {"3", [](char c) {
            if (isNumeric(c)) return "3";
            return "final";
          }},
          {"4", [](char c) {
            if (isNumeric(c)) return "3";
            return "dead";
          }},
        },
        "0",
        std::set<std::string>{"final"},
        std::set<std::string>{"final"}
      );
    }
};

class OtherTokenIdentifier : public TokenIdentifier
{
  public:
    OtherTokenIdentifier(): TokenIdentifier("NI") {
      _identifier_machine = std::make_unique<FiniteStateMachine>(
        std::unordered_map<std::string, std::function<std::string(char)>>{
          {"0", [](char c) {
            if (!isAlpha(c) && !isNumeric(c) && !isWhitespace(c)) return "final";
            return "dead";
          }},
        },
        "0",
        std::set<std::string>{"final"}
      );
    }
};
