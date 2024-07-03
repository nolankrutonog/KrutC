#ifndef TOKEN_H
#define TOKEN_H

#include <iostream>
#include <unordered_map>
#include <set>



enum TokenType {
  CLASS = 256,
  INHERITS = 257,
  FOR = 258,
  IF = 262,
  ELSE = 263,
  WHILE = 264,
  RETURN = 265,
  // THIS = 266,
  INT_CONST = 267,
  BOOL_CONST = 268,
  STR_CONST = 269,
  SPECIAL_CHAR = 278,
  BINOP = 282,
  TYPEID = 285,
  OBJECTID = 286,
  CONTINUE = 287,
  BREAK = 288,
  NONE = 289,
  KILL = 290,
  NEW = 291,


  EMPTY = 468,
  ERROR = 469
};



static std::unordered_map<TokenType, std::string> TOKEN_TYPE_TO_STR = {
    {CLASS, "CLASS"},
    {INHERITS, "INHERITS"},
    {FOR, "FOR"},
    {IF, "IF"},
    {ELSE, "ELSE"},
    {WHILE, "WHILE"},
    {RETURN, "RETURN"},
    {INT_CONST, "INT_CONST"},
    {BOOL_CONST, "BOOL_CONST"},
    {STR_CONST, "STR_CONST"},
    {SPECIAL_CHAR, "SPECIAL_CHAR"},
    {BINOP, "BINOP"},
    {TYPEID, "TYPEID"},
    {OBJECTID, "OBJECTID"},
    {CONTINUE, "CONTINUE"},
    {BREAK, "BREAK"},
    {NONE, "NONE"},
    {KILL, "KILL"},
    {NEW, "NEW"},

    {EMPTY, "EMPTY"},
    {ERROR, "ERROR"}

};



class Token {
  int lineno;
  TokenType type;
  std::string str;
public:
  // Token();
  Token(TokenType type) : type(type) {}
  Token(int lineno, TokenType type, std::string str):
    lineno(lineno), type(type), str(str) {}

  int get_lineno() { return lineno; }
  TokenType get_type() { return type; }
  std::string get_type_str() {
    return TOKEN_TYPE_TO_STR[type];
  }
  std::string get_str() { return str; }

  void set_lineno(int n) { lineno = n; }
  void set_type(TokenType t) { type = t; }
  void set_str(std::string s) { str = s; }

  void dump() {
    std::cout << "#" << lineno << " " << TOKEN_TYPE_TO_STR[type] << " " << str << std::endl;
  }

};

static std::set<TokenType> KEYWORDS_TTYPE {
  CLASS, INHERITS, FOR, /* START, END, STEP,*/ IF, ELSE, WHILE, RETURN, // THIS
};

static std::unordered_map<std::string, TokenType> KEYWORDS_STR_TO_TTYPE {
  {"CLASS", CLASS},
  {"INHERITS", INHERITS},
  {"FOR", FOR},
  {"IF", IF},
  {"ELSE", ELSE},
  {"WHILE", WHILE},
  {"RETURN", RETURN},
  {"CONTINUE", CONTINUE},
  {"BREAK", BREAK},
  {"NONE", NONE},
  {"NEW", NEW},
  {"KILL", KILL}
};

#endif // TOKEN_H