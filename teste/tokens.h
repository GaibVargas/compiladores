#ifndef __tokens_h
#define __tokens_h

#include <string>
#include <memory>
#include <fstream>
#include <iostream>
#include "fsm.h"

bool isAlpha(char ch);

bool isNumeric(char ch);

bool isWhitespace(char c);

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
public:
  int cursor_start;
  int start_line;
  int start_column;
protected:
  std::string _id;
  std::unique_ptr<FiniteStateMachine> _identifier_machine;

public:
  TokenIdentifier(const std::string &id) : _id(id) {}
  std::string get_id()
  {
    return _id;
  }
  void reset()
  {
    _identifier_machine->reset();
  }
  std::string get_lexeme()
  {
    return _identifier_machine->get_lexeme();
  }
  bool has_already_succeeded()
  {
    return _identifier_machine->get_status() == FSMStatus::SUCCESS;
  }
  bool has_to_move_cursor_back()
  {
    return _identifier_machine->has_to_move_cursor_back();
  }
  FSMStatus analyze(char c, int cursor_position, int line, int column);
};

class IdentTokenIdentifier : public TokenIdentifier
{
  public:
    IdentTokenIdentifier();
};

class IntegerTokenIdentifier : public TokenIdentifier
{
  public:
    IntegerTokenIdentifier();
};

class OtherTokenIdentifier : public TokenIdentifier
{
  public:
    OtherTokenIdentifier();
};

#endif