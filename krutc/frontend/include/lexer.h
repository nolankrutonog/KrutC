#include <iostream>
#include "filestreambuffer.h"
#include "token.h"

#define MAX_VERSE_LEN 1024


class Lexer {
public:
  Lexer(const std::string filepath) : buff(filepath), filepath(filepath) {}
  Token get_next_token();
  bool has_more();

private:
  FileStreamBuffer buff;
  std::string filepath;
  std::string verse_err_buff;

  char char_buff[MAX_VERSE_LEN];
  
  int curr_lineno = 1;
  
  Token ml_comment();
  int verse();

};
