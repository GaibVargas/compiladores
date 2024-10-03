#include <iostream>
#include <string>
#include <fstream>
#include <cctype>
#include <memory>
#include <vector>

struct Token {
  std::string lexem;
  std::string id;
  int start;
  int end;

  Token(const std::string& lexem, const std::string& id, int start, int end)
    : lexem(lexem), id(id), start(start), end(end) {}
};

class TokenIdentifier {
  protected:
    std::string _id;

    void prepare_reading(std::ifstream& stream, int start) {
      if (start < 0) {
        std::cout << "[ALERT] Trying to position file cursor at invalid position.\nStart: " << start << "\nCurrent cursor position: " << stream.tellg() << std::endl;
        return;
      }
      if (stream.eof()) {
        stream.clear();
      }
      stream.seekg(start);
    }

  public:
    TokenIdentifier(const std::string& id): _id(id) {}
    virtual std::shared_ptr<Token> next_token(std::ifstream& stream, int start) = 0;
};

bool isAlpha(char ch) {
  return std::isalpha(static_cast<unsigned char>(ch));
}

bool isNumeric(char ch) {
  return std::isdigit(static_cast<unsigned char>(ch));
}

bool isWhitespace(char c) {
  return std::isspace(static_cast<unsigned char>(c));
}

class IdentTokenIdentifier: public TokenIdentifier {
  public:
    IdentTokenIdentifier(): TokenIdentifier(std::string("IDENT")) {}

    std::shared_ptr<Token> next_token(std::ifstream& stream, int start) {
      prepare_reading(stream, start);

      char c;
      if (!stream.get(c)) return nullptr;
      while (isWhitespace(c) && !stream.eof()) {
        if (!stream.get(c)) return nullptr;
      }

      if (!isAlpha(c)) {
        stream.seekg(-1, std::ios::cur);
        return nullptr;
      }

      int token_start = stream.tellg();
      std::string lexem{c};

      while (true) {
        if (!stream.get(c)) break;
        if (!isAlpha(c) && !isNumeric(c) && c != '_') break;
        lexem += c;
        if (stream.eof()) break;
      }
      if (!stream.eof()) {
        stream.seekg(-1, std::ios::cur);
      }

      return std::make_shared<Token>(lexem, _id, token_start, static_cast<int>(stream.tellg()) + 1);
    }
};

class NumberTokenIdentifier: public TokenIdentifier {
  public:
    NumberTokenIdentifier(): TokenIdentifier(std::string("NUM")) {}

    std::shared_ptr<Token> next_token(std::ifstream& stream, int start) {
      prepare_reading(stream, start);

      char c;
      bool has_read_point = false;
      if (!stream.get(c)) return nullptr;
      while (isWhitespace(c) && !stream.eof()) {
        if (!stream.get(c)) return nullptr;
      }

      if (!isNumeric(c) && c != '.') {
        stream.seekg(-1, std::ios::cur);
        return nullptr;
      }
      bool needs_to_read_another = false;

      if (c == '.'){
        has_read_point = true;
        needs_to_read_another = true;
      }

      int token_start = stream.tellg();
      std::string lexem{c};
      while (true) {
        if (!stream.get(c)) break;
        if (!isNumeric(c) && c != '.') break;
        if (c == '.' && has_read_point) break;
        if (c == '.') {
          has_read_point = true;
          needs_to_read_another = true;
        }

        lexem += c;

        if (isNumeric(c) && needs_to_read_another)
          needs_to_read_another = false;

        if (stream.eof()) break;
      }

      if (!stream.eof()) {
        stream.seekg(-1, std::ios::cur);
      }

      if (needs_to_read_another && lexem.size() > 1) {
        stream.seekg(-1, std::ios::cur);
        lexem.pop_back();
        return std::make_shared<Token>(lexem, "NI", token_start, static_cast<int>(stream.tellg()) + 1);
        return nullptr;
      }
      
      if (!has_read_point)
        return std::make_shared<Token>(lexem, "NI", token_start, static_cast<int>(stream.tellg()) + 1);

      return std::make_shared<Token>(lexem, "NPF", token_start, static_cast<int>(stream.tellg()) + 1);
    }
};

class OtherTokenIdentifier: public TokenIdentifier {
  public:
    OtherTokenIdentifier(): TokenIdentifier(std::string("OUTRO")) {}

    std::shared_ptr<Token> next_token(std::ifstream& stream, int start) {
      prepare_reading(stream, start);

      char c;
      if (!stream.get(c)) return nullptr;
      while (isWhitespace(c) && !stream.eof()) {
        if (!stream.get(c)) return nullptr;
      }

      int token_start = stream.tellg();

      if (!isAlpha(c) && !isNumeric(c) && !isWhitespace(c)) {
        return std::make_shared<Token>(std::string(1, c), _id, token_start, static_cast<int>(stream.tellg()) + 1);
      };
      return nullptr;
    }
};

std::shared_ptr<Token> get_longest_token(std::vector<std::shared_ptr<Token>>& tokens) {
  std::shared_ptr<Token> response;
  int longest = 0;
  int index = 0;
  for (const auto &token: tokens) {
    auto token_size = token->end - token->start;
    if (index == 0) {
      longest = token_size;
      response = token;
    } else if (token_size > longest) {
      longest = token_size;
      response = token;
    }
  }
  return response;
}

int main(int argc, char const *argv[]) {
  std::vector<std::shared_ptr<TokenIdentifier>> token_identifiers{
    std::make_shared<IdentTokenIdentifier>(),
    std::make_shared<NumberTokenIdentifier>(),
    std::make_shared<OtherTokenIdentifier>()
  };

  std::vector<std::shared_ptr<Token>> current_tokens;
  std::vector<std::shared_ptr<Token>> tokens;

  int start = 0;

  std::ifstream file("file.txt");

  if(file.is_open()) {
    while(true) {
      file.clear();
      file.seekg(start);
      while (isWhitespace(file.get()) && !file.eof());
      if (file.get() == EOF) break;
      else file.seekg(-1, std::ios::cur);

      current_tokens.clear();
      for (const auto &token_identifier: token_identifiers) {
        auto token = token_identifier->next_token(file, start);
        if (!token) continue;
        current_tokens.push_back(token);
      }

      if (!tokens.empty() && current_tokens.empty()) {
        std::cout << "Lexic error start on file cursor position " << start << std::endl;
        return -1;
      }

      auto token = get_longest_token(current_tokens);
      // std::cout << token->id << ' ' << token->lexem << " start " << token->start << " end " << token->end << std::endl;
      tokens.push_back(token);
      start = token->end - 1;
    }
  }
  for (auto & token: tokens) {
    std::cout << token->id << ' ';
  }
  std::cout << std::endl;
  return 0;
}