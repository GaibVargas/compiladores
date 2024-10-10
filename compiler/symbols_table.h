#ifndef __symbol_table_h
#define __symbol_table_h

#include <unordered_map>
#include <string>
#include <vector>
#include <iostream>
#include "tokens.h"

class SymbolsTable
{
private:
  std::unordered_map<std::string, std::vector<int>> _table;
public:
  void add_symbol(std::shared_ptr<Token>& token);
  void print(std::ostream& out);
};

#endif