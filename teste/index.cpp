#include <memory>
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
public:
  int cursor_start;
  int start_line;
  int start_column;
protected:
  std::string _id;
  std::unique_ptr<FiniteStateMachine> _identifier_machine;

public:
  TokenIdentifier(const std::string &id) : _id(id) {}
  std::string get_id() {
    return _id;
  }
  void reset()
  {
    _identifier_machine->reset();
  }
  FSMStatus analyze(char c, int cursor_position, int line, int column)
  {
    auto prev_status = _identifier_machine->get_status();
    if (prev_status == FSMStatus::ERROR || prev_status == FSMStatus::SUCCESS)
      return prev_status;
    auto current_status = _identifier_machine->transition(c);
    if (prev_status == FSMStatus::IDLE && (current_status == FSMStatus::RUNNING || current_status == FSMStatus::SUCCESS)) {
      cursor_start = cursor_position;
      start_line = line;
      start_column = column;
    }
    return current_status;
  };
  std::string get_lexeme()
  {
    return _identifier_machine->get_lexeme();
  }
  bool has_already_succeeded() {
    return _identifier_machine->get_status() == FSMStatus::SUCCESS;
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
    OtherTokenIdentifier(): TokenIdentifier("OTHER") {
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


struct PossibleToken
{
  std::shared_ptr<Token> token;
  int start;

  PossibleToken(std::shared_ptr<Token> token, int start): token(token), start(start) {}
};

std::shared_ptr<PossibleToken> get_longest_token(std::vector<std::shared_ptr<PossibleToken>>& possible_tokens) {
  std::shared_ptr<PossibleToken> response;
  int longest = 0;
  int index = 0;
  for (const auto &possible_token: possible_tokens) {
    auto token_size = possible_token->token->lexeme.size();
    if (index == 0) {
      longest = token_size;
      response = possible_token;
    } else if (token_size > longest) {
      longest = token_size;
      response = possible_token;
    }
  }
  return response;
}

int main() {
  std::vector<std::string> reserved{
    "def",
    "if",
    "else",
    "for",
    "while",
    "int",
    "float",
    "string",
    "return",
    "new",
    "read",
    "print"
  };
  std::vector<std::shared_ptr<TokenIdentifier>> token_identifiers{
    std::make_shared<IdentTokenIdentifier>(),
    std::make_shared<IntegerTokenIdentifier>(),
    std::make_shared<OtherTokenIdentifier>()
  };
  std::vector<std::shared_ptr<Token>> tokens;
  std::vector<std::shared_ptr<PossibleToken>> possible_tokens;

  auto filename = "file.txt";
  std::ifstream file(filename);

  char c;
  int line = 1;
  int column = 0;
  bool reading_token = false;

  if(file.is_open()) {
    while(file.good()) {
      file.get(c);
      if (c == EOF && !reading_token) break;
      column++;
      if (c == '\n') {
        line++;
        column = 0;
      }

      if (isWhitespace(c) && !reading_token) continue;

      int on_running = 0;
      for (const auto &token_identifier: token_identifiers) {
        if (token_identifier->has_already_succeeded()) continue;
        auto result = token_identifier->analyze(c, static_cast<int>(file.tellg()) - 1, line, column);
        if (result == FSMStatus::RUNNING) {
          on_running++;
          continue;
        }
        if (result == FSMStatus::SUCCESS) {
          auto token = std::make_shared<Token>(token_identifier->get_lexeme(), token_identifier->get_id(), token_identifier->start_line, token_identifier->start_column);
          possible_tokens.push_back(std::make_shared<PossibleToken>(token, token_identifier->cursor_start));
        }
      }

      reading_token = on_running > 0;

      if (on_running > 0) continue;

      if (possible_tokens.size() == 0) {
        std::cout << "Lexic error starting on " << line << ':' << column << std::endl;
        return -1;
      }

      if (possible_tokens.size()) {
        auto choosen_token = get_longest_token(possible_tokens);
        auto token = choosen_token->token;
        tokens.push_back(token);
        possible_tokens.clear();

        std::cout << token->id << ' ' << token->lexeme << ' ' << token->line << ":" << token->column << std::endl;

        int next_cursor_position = choosen_token->start + token->lexeme.size();
        // if (isWhitespace(c)) next_cursor_position++; Isso tem problema no caso do token identificado não ser o último autômato a rodar

        file.clear();
        file.seekg(next_cursor_position);

        for (const auto &token_identifier: token_identifiers) {
          token_identifier->reset();
        }
      }
    }
  }
}