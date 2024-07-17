#ifndef TOKEN_H
#define TOKEN_H

#include <iostream>
#include <set>
#include <unordered_map>

#include "constants.h"

class Token {
  int lineno;
  TokenType type;
  std::string str;

 public:
  Token(TokenType type) : type(type) {}
  Token(int lineno, TokenType type, std::string str) : lineno(lineno), type(type), str(str) {}

  int get_lineno() { return lineno; }
  TokenType get_type() { return type; }
  std::string get_type_str() { return TOKEN_TYPE_TO_STR[type]; }
  std::string get_str() { return str; }

  void set_lineno(int n) { lineno = n; }
  void set_type(TokenType t) { type = t; }
  void set_str(std::string s) { str = s; }

  void dump() { std::cout << "#" << lineno << " " << TOKEN_TYPE_TO_STR[type] << " " << str << std::endl; }
};

#endif  // TOKEN_H