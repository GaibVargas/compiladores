#include <memory>
#include <string>
#include <memory>
#include <fstream>
#include <iostream>
#include "tokens.h"
#include "symbols_table.h"

struct PossibleToken
{
  std::shared_ptr<Token> token;
  int start;

  PossibleToken(std::shared_ptr<Token> token, int start) : token(token), start(start) {}
};

std::shared_ptr<PossibleToken> get_longest_token(std::vector<std::shared_ptr<PossibleToken>> &possible_tokens)
{
  std::shared_ptr<PossibleToken> response;
  int longest = 0;
  int index = 0;
  for (const auto &possible_token : possible_tokens)
  {
    auto token_size = possible_token->token->lexeme.size();
    if (index == 0)
    {
      longest = token_size;
      response = possible_token;
    }
    else if (token_size > longest)
    {
      longest = token_size;
      response = possible_token;
    }
  }
  return response;
}

int main()
{
  std::vector<std::string> reserved{
      "def",
      "if",
      "else",
      "for",
      "while",
      "int",
      "float",
      "string",
      "tint",
      "tfloat",
      "tstring",
      "return",
      "new",
      "read",
      "print",
      "and",
      "or",
      "not"};
  std::vector<std::shared_ptr<TokenIdentifier>> token_identifiers{
      std::make_shared<IdentTokenIdentifier>(),
      std::make_shared<IntegerTokenIdentifier>(),
      std::make_shared<FloatTokenIdentifier>(),
      std::make_shared<OtherTokenIdentifier>()};
  std::vector<std::shared_ptr<Token>> tokens;
  std::vector<std::shared_ptr<PossibleToken>> possible_tokens;

  auto symbol_table = std::make_unique<SymbolsTable>();

  auto filename = "file.txt";
  std::ifstream file(filename);

  char c;
  int line = 1;
  int column = 0;
  int line_between_current_token_reading = 0;
  bool reading_token = false;

  if (file.is_open())
  {
    while (file.good())
    {
      file.get(c);
      if (c == EOF && !reading_token)
        break;
      column++;
      if (c == '\n')
      {
        line++;
        column = 0;
        if (reading_token)
          line_between_current_token_reading++;
      }

      if (isWhitespace(c) && !reading_token)
        continue;

      int on_running = 0;
      for (const auto &token_identifier : token_identifiers)
      {
        if (token_identifier->has_already_succeeded())
          continue;
        auto result = token_identifier->analyze(c, static_cast<int>(file.tellg()) - 1, line, column);
        if (result == FSMStatus::RUNNING)
        {
          on_running++;
          continue;
        }
        if (result == FSMStatus::SUCCESS)
        {
          auto token = std::make_shared<Token>(token_identifier->get_lexeme(), token_identifier->get_id(), token_identifier->start_line, token_identifier->start_column);
          possible_tokens.push_back(std::make_shared<PossibleToken>(token, token_identifier->cursor_start));
        }
      }

      reading_token = on_running > 0;

      if (on_running > 0)
        continue;

      if (possible_tokens.size() == 0)
      {
        std::cout << "Lexic error starting on " << line << ':' << column << std::endl;
        return -1;
      }

      if (possible_tokens.size())
      {
        auto choosen_token = get_longest_token(possible_tokens);
        auto token = choosen_token->token;
        tokens.push_back(token);

        if (token->id == "IDENT" && std::find(reserved.begin(), reserved.end(), token->lexeme) == reserved.end())
          symbol_table->add_symbol(token);
        // std::cout << token->id << ' ' << token->lexeme << ' ' << token->line << ":" << token->column << std::endl;

        column = token->column + token->lexeme.size() - 1;
        if (line_between_current_token_reading)
        {
          line -= line_between_current_token_reading;
          line_between_current_token_reading = 0;
        }

        file.clear();
        int next_cursor_position = choosen_token->start + token->lexeme.size();
        file.seekg(next_cursor_position);

        possible_tokens.clear();
        for (const auto &token_identifier : token_identifiers)
        {
          token_identifier->reset();
        }
      }
    }
  }
  symbol_table->print(std::cout);
}