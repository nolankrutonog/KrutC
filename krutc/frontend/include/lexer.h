#ifndef LEXER_H
#define LEXER_H

#include <iostream>

#include "filestreambuffer.h"
#include "token.h"

#define MAX_STRING_LEN 1024

class Lexer {
 public:
  Lexer(const std::string filepath) : buff(filepath), filepath(filepath) {}
  Token get_next_token();
  bool has_more();

 private:
  FileStreamBuffer buff;
  std::string filepath;
  std::string string_err_buff;

  char char_buff[MAX_STRING_LEN];

  int curr_lineno = 1;

  Token ml_comment();
  int get_string();
};

#endif  // LEXER_H
