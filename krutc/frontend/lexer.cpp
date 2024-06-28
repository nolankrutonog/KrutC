/* Definition of a lexer for the Krutcpp Programming Language (KPL) */
#include <iostream>
#include <vector>
#include <unordered_set>

#include "include/lexer.h"

using namespace std;

static unordered_set<string> KEYWORDS_STR = {
  "CLASS",
  "INHERITS",
  "FOR",
  "IF",
  "ELSE",
  "WHILE",
  "RETURN",
  "THIS",
  "CONTINUE",
  "BREAK",
  "NEW",
  "NONE",
  "KILL"
};

unordered_set<string> BASIC_TYPES = {
  "int", /* currently represented as long */
  /* TODO: double, float, long, etc */
  "string",
  "bool",
  "void",
  // "none",
  "list",
  "stack",
  "object",
  "char"
};

static const unordered_set<string> SPECIAL_CHARS = {
  "(",
  ")",
  ";",
  "[",
  "]",
  "{",
  "}",
  ".",
  ",",
  // TODO: list[i:j]
  // ":"
};

static const unordered_set<string> BINOPM {
  "+",
  "-",
  "*",
  "/",

  "<",
  ">",

  "<=",
  ">=",
  "==",
  "!=",
  "&&",
  "||",

  "+=",
  "-=",
  "*=",
  "/=",

  "="
};

/* remove all characters that are part of comments */
Token Lexer::ml_comment() {
  Token t = Token(EMPTY);

  int comment_stack = 1;
  int lineno = curr_lineno;

  while (comment_stack) {
    char curr = buff.get_next();

    if (curr == '*') {
      char next = buff.lookahead(0);
      if (next == '/') {
        /* pop comment_end off buff */
        buff.get_next();
        comment_stack--;
      }
    } else if (curr == '/') {
      char next = buff.lookahead(0);
      if (next == '*') {
        buff.get_next(); /* pop * off buff */
        comment_stack++;
        lineno = curr_lineno;
      }
    } else if (curr == '\n') {
      curr_lineno++;
    } else if (curr == '\0') {
      string err_str = filepath + ":" + to_string(lineno) + " Lexer Error: EOF in comment. ";
      t = Token(lineno, ERROR, err_str);
      return t;
    } 
  }
  return t;
}

/* find every string */
int Lexer::get_string() {
  string_err_buff = "";
  int string_start_lineno = curr_lineno;
  int char_buff_size = 0;
  while (true) {
    char curr = buff.get_next();
    
    if (curr == '\0') {
      char_buff_size = -1;
      string_err_buff = filepath + ":" + to_string(string_start_lineno) + ": " + "Lexer Error: EOF in string.";
    } else if (curr == '\n') {
      curr_lineno++;
      char_buff[char_buff_size++] = curr;
      continue;
    } else if (char_buff_size >= MAX_STRING_LEN) {
      char_buff_size = -1;
      string_err_buff = filepath + ":" + to_string(string_start_lineno) + ": " + "Lexer Error. Verse too long. Maximum string length is " + to_string(MAX_STRING_LEN);
    } else if (curr == '\"') {
      break;
    } else {
      char_buff[char_buff_size++] = curr;
    }
  }
  return char_buff_size;
}



/* Called from the parser. On each call, returns the next token. */
Token Lexer::get_next_token() {
  Token t = Token(EMPTY);

  while (buff.has_next()) {
    string curr;
    curr += buff.get_next();

    // Multiline comment check
    if (curr == "/" && buff.lookahead(0) == '*') {
      buff.get_next(); // consume '*'
      Token et = ml_comment();
      if (et.get_type() == ERROR) return et;
      continue;
    } else if (curr == "-" && buff.lookahead(0) == '-') {
      /* single line comments */
      buff.get_next(); // pop trailing '-'
      while (buff.lookahead(0) != '\n' && buff.has_next()) {
        buff.get_next();
      }
      continue;
    }

    if (curr == "*" && buff.lookahead(0) == '/') {
      string err_msg = filepath + ":" + to_string(curr_lineno) + ": Lexer Error: Unrecognized token " + curr + buff.lookahead(0);
      t = Token(curr_lineno, ERROR, err_msg);
      break;
    }

    if (curr == "\n") {
      curr_lineno++;
      continue;
    }

    if (curr == "\t" || curr == " ") 
      continue;

    // Special characters
    if (SPECIAL_CHARS.find(curr) != SPECIAL_CHARS.end()) {
      t = Token(curr_lineno, SPECIAL_CHAR, curr);
      break;
    }

    /* binary operators */
    string possible = curr + buff.lookahead(0);
    if (BINOPM.find(possible) != BINOPM.end()) {
      curr += buff.get_next();
      t = Token(curr_lineno, BINOP, curr);
      break;
    } else if (BINOPM.find(curr) != BINOPM.end()) {
      t = Token(curr_lineno, BINOP, curr);
      break;
    }

    // Handle STRING_CONST
    if (curr == "\"") {
      int num_chars = get_string();
      if (num_chars >= 0) {
        string str(char_buff, num_chars);
        t = Token(curr_lineno, STR_CONST, "\"" + str + "\"");
      } else {
        t = Token(curr_lineno, ERROR, string_err_buff);
      }
      break;
    }

    // Handle INT_CONST
    if (isnumber(curr[0])) {
      while (isnumber(buff.lookahead(0))) {
        curr += buff.get_next();
      }
      t = Token(curr_lineno, INT_CONST, curr);
      break;
    }

    // Handle keywords, typeids, objectids
    if (isalpha(curr[0])) {
      while (isalnum(buff.lookahead(0)) || buff.lookahead(0) == '_') {
        curr += buff.get_next();
      }

      string curr_upper = curr;
      transform(curr_upper.begin(), curr_upper.end(), curr_upper.begin(), ::toupper);

      if (KEYWORDS_STR.find(curr_upper) != KEYWORDS_STR.end()) {
        t = Token(curr_lineno, KEYWORDS_STR_TO_TTYPE[curr_upper], "");
      } else if (isupper(curr[0]) || BASIC_TYPES.find(curr) != BASIC_TYPES.end()) {
        t = Token(curr_lineno, TYPEID, curr);
      } else if (curr == "true" || curr == "false") {
        t = Token(curr_lineno, BOOL_CONST, curr);
      } else {
        t = Token(curr_lineno, OBJECTID, curr);
      }
      break;
    }

    // Handle unrecognized tokens
    string err_msg = filepath + ":" + to_string(curr_lineno) + ": Lexer Error: Unrecognized token " + curr;
    t = Token(curr_lineno, ERROR, err_msg);
    break;
  }
  return t;
}

bool Lexer::has_more() {
  return buff.has_next();
}

