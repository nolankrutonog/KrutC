#include "include/token.h"
#include <map>
#include <set>

using namespace std;

static const map<TokenType, string> TOKEN_TYPE_TO_STR = {
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

static const map<string, TokenType> KEYWORDS_STR_TO_TTYPE = {
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

static const set<TokenType> KEYWORDS_TTYPE = {
  CLASS, INHERITS, FOR, IF, ELSE, WHILE, RETURN, // THIS
};

///
/// Token methods 
///
Token::Token(TokenType type) : type(type) {}

Token::Token(int lineno, TokenType type, string str) 
  : lineno(lineno), type(type), str(str) {}

int Token::get_lineno() { 
  return lineno; 
}

TokenType Token::get_type() { 
  return type; 
}

string Token::get_type_str() { 
  return TOKEN_TYPE_TO_STR.at(type);
}

string Token::get_str() { 
  return str; 
}

void Token::set_lineno(int n) { 
  lineno = n; 
}

void Token::set_type(TokenType t) {
  type = t;
}

void Token::set_str(string s) {
  str = s;
}

void Token::dump() {
  cout << "#" << lineno << " " << TOKEN_TYPE_TO_STR.at(type) << " " << str << endl;
}

