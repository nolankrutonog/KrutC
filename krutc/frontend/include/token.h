#ifndef TOKEN_H
#define TOKEN_H

#include <iostream>
#include <map>
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
  CHAR_CONST = 292,


  EMPTY = 468,
  ERROR = 469
};


extern const std::map<TokenType, std::string> TOKEN_TYPE_TO_STR; 
extern const std::set<TokenType> KEYWORDS_TTYPE; 
extern const std::map<std::string, TokenType> KEYWORDS_STR_TO_TTYPE;


class Token {
  int lineno;
  TokenType type;
  std::string str;
public:
  // Token();
  Token(TokenType type);
  Token(int lineno, TokenType type, std::string str);

  int get_lineno();
  TokenType get_type();
  std::string get_type_str();
  std::string get_str();

  void set_lineno(int n);
  void set_type(TokenType t);
  void set_str(std::string s);

  void dump();
};


#endif // TOKEN_H