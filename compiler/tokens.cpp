#include <string>
#include <memory>
#include <fstream>
#include <iostream>
#include "fsm.h"
#include "tokens.h"

bool isAlpha(char ch)
{
  return std::isalpha(static_cast<unsigned char>(ch));
}

bool isNumeric(char ch)
{
  return std::isdigit(static_cast<unsigned char>(ch));
}

bool isWhitespace(char c)
{
  return std::isspace(static_cast<unsigned char>(c));
}

FSMStatus TokenIdentifier::analyze(char c, int cursor_position, int line, int column)
{
  auto prev_status = _identifier_machine->get_status();
  if (prev_status == FSMStatus::ERROR || prev_status == FSMStatus::SUCCESS)
    return prev_status;
  auto current_status = _identifier_machine->transition(c);
  if (prev_status == FSMStatus::IDLE && (current_status == FSMStatus::RUNNING || current_status == FSMStatus::SUCCESS))
  {
    cursor_start = cursor_position;
    start_line = line;
    start_column = column;
  }
  return current_status;
}

IdentTokenIdentifier::IdentTokenIdentifier() : TokenIdentifier("IDENT")
{
  _identifier_machine = std::make_unique<FiniteStateMachine>(
      std::unordered_map<std::string, std::function<std::string(char)>>{
          {"0", [](char c)
           {
             if (isAlpha(c))
               return "1";
             return "dead";
           }},
          {"1", [](char c)
           {
             if (isAlpha(c) || isNumeric(c))
               return "1";
             return "final";
           }},
      },
      "0", std::set<std::string>{"final"}, std::set<std::string>{"final"});
}

IntegerTokenIdentifier::IntegerTokenIdentifier() : TokenIdentifier("NI")
{
  _identifier_machine = std::make_unique<FiniteStateMachine>(
      std::unordered_map<std::string, std::function<std::string(char)>>{
          {"0", [](char c)
           {
             if (isNumeric(c))
               return "1";
             return "dead";
           }},
          {"1", [](char c)
           {
             if (isNumeric(c))
               return "1";
             if (c == 'e')
               return "2";
             return "final";
           }},
          {"2", [](char c)
           {
             if (isNumeric(c))
               return "3";
             if (c == '+')
               return "4";
             return "dead";
           }},
          {"3", [](char c)
           {
             if (isNumeric(c))
               return "3";
             return "final";
           }},
          {"4", [](char c)
           {
             if (isNumeric(c))
               return "3";
             return "dead";
           }},
      },
      "0", std::set<std::string>{"final"}, std::set<std::string>{"final"});
}

FloatTokenIdentifier::FloatTokenIdentifier() : TokenIdentifier("NPF")
{
  _identifier_machine = std::make_unique<FiniteStateMachine>(
      std::unordered_map<std::string, std::function<std::string(char)>>{
          {"0", [](char c)
           {
             if (c == '.')
               return "1";
             if (isNumeric(c))
               return "6";
             return "dead";
           }},
          {"1", [](char c)
           {
             if (isNumeric(c))
               return "2";
             return "dead";
           }},
          {"2", [](char c)
           {
             if (isNumeric(c))
               return "2";
             if (c == 'e')
               return "3";
             return "final";
           }},
          {"3", [](char c)
           {
             if (isNumeric(c))
               return "4";
             if (c == '+' || c == '-')
               return "5";
             return "dead";
           }},
          {"4", [](char c)
           {
             if (isNumeric(c))
               return "4";
             return "final";
           }},
          {"5", [](char c)
           {
             if (isNumeric(c))
               return "4";
             return "dead";
           }},
          {"6", [](char c)
           {
             if (isNumeric(c))
               return "6";
             if (c == '.')
               return "2";
             if (c == 'e')
               return "7";
             return "dead";
           }},
          {"7", [](char c)
           {
             if (c == '-')
               return "5";
             return "final";
           }},
      },
      "0", std::set<std::string>{"final"}, std::set<std::string>{"final"});
}

OtherTokenIdentifier::OtherTokenIdentifier() : TokenIdentifier("OUTRO")
{
  _identifier_machine = std::make_unique<FiniteStateMachine>(
      std::unordered_map<std::string, std::function<std::string(char)>>{
          {"0", [](char c)
           {
             if (!isAlpha(c) && !isNumeric(c) && !isWhitespace(c))
               return "final";
             return "dead";
           }},
      },
      "0", std::set<std::string>{"final"});
}
