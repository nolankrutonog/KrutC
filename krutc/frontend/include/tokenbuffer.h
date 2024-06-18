#ifndef TOKEN_BUFFER_H
#define TOKEN_BUFFER_H

#include <iostream>
#include <deque>

#include "lexer.h"

class TokenBuffer {
public:
  int has_errors = 0;

  TokenBuffer(const std::string filepath, bool token_dump) 
    : lexer(filepath), token_dump(token_dump) { fill_buffer(); }

  bool has_next() {
    return buffer.size();
  }

  Token get_next() { 
    if (buffer.empty()) {
      return Token(EMPTY);
    }
    Token t = buffer.front();
    if (token_dump)
      t.dump();
    buffer.pop_front();
    curr_lineno = t.get_lineno();
    return t;
  }

  Token lookahead(int n) {
    if (n >= buffer.size()) {
      return Token(EMPTY);
    }
    return buffer[n];
  }

  int get_lineno() {
    return curr_lineno;
  }

private:
  Lexer lexer;
  std::deque<Token> buffer;
  bool token_dump;
  int curr_lineno = 0;

  void fill_buffer() {
    while (lexer.has_more()) {
      Token t = lexer.get_next_token();
      if (t.get_type() == ERROR) {
        std::cout << t.get_str() << std::endl;
        has_errors++;
        continue;
      }
      if (t.get_type() == EMPTY) {
        continue;
      }
      // t.dump();
      buffer.push_back(t);
    }
  }
};

#endif // TOKEN_BUFFER_H