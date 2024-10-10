#include "symbols_table.h"

void SymbolsTable::add_symbol(std::shared_ptr<Token> &token)
{
  if (_table.find(token->lexeme) != _table.end())
  {
    if (std::find(_table[token->lexeme].begin(), _table[token->lexeme].end(), token->line) == _table[token->lexeme].end())
      _table[token->lexeme].push_back(token->line);
  }
  else
  {
    _table[token->lexeme] = std::vector{token->line};
  }
}

void SymbolsTable::print(std::ostream &out)
{
  for (const auto &pair : _table)
  {
    out << pair.first << ": [";
    for (uint i = 0; i < pair.second.size(); i++)
    {
      if (i == pair.second.size() - 1)
      {
        out << pair.second[i] << "]" << std::endl;
      }
      else
      {
        out << pair.second[i] << ",";
      }
    }
  }
}