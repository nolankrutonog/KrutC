#include <iostream>
#include <unordered_map>
#include <set>


enum TokenType {
  BRO = 256,
  VIBESWITH = 257,
  FOR = 258,
  // START = 259,
  // END = 260,
  // STEP = 261,
  IF = 262,
  ELSE = 263,
  WHILE = 264,
  RETURN = 265,
  THIS = 266,
  INT_CONST = 267,
  BOOL_CONST = 268,
  VERSE_CONST = 269,
  // DISPATCH = 270,
  // ASSIGN = 270, // =, +=, -=, *=, /=
  // LEQ = 271,
  // GEQ = 272,
  // EQ = 273,
  // ASSIGN_PLUS = 274, 
  // ASSIGN_SUB = 275,
  // ASSIGN_MUL = 276,
  // ASSIGN_DIV = 277,
  // ASSIGN_OP = 277,
  SPECIAL_CHAR = 278,
  // PLUS = 279,
  // SUB = 280,
  // MUL = 281,
  // DIV = 282,
  BINOP = 282,
  // LESS_THAN = 283,
  // GREATER_THAN = 284,
  TYPEID = 285,
  OBJECTID = 286,
  CONTINUE = 287,
  BREAK = 288,


  EMPTY = 468,
  ERROR = 469
};

static std::unordered_map<TokenType, std::string> TOKEN_TYPE_TO_STRING = {
    {BRO, "BRO"},
    {VIBESWITH, "VIBESWITH"},
    {FOR, "FOR"},
    // {START, "START"},
    // {END, "END"},
    // {STEP, "STEP"},
    {IF, "IF"},
    {ELSE, "ELSE"},
    {WHILE, "WHILE"},
    {RETURN, "RETURN"},
    {THIS, "THIS"},
    {INT_CONST, "INT_CONST"},
    {BOOL_CONST, "BOOL_CONST"},
    {VERSE_CONST, "VERSE_CONST"},
    // {DISPATCH, "DISPATCH"},
    // {ASSIGN, "ASSIGN"},
    // {LEQ, "LEQ"},
    // {GEQ, "GEQ"},
    // {EQ, "EQ"},
    // {ASSIGN_PLUS, "ASSIGN_PLUS"},
    // {ASSIGN_SUB, "ASSIGN_SUB"},
    // {ASSIGN_MUL, "ASSIGN_MUL"},
    // {ASSIGN_DIV, "ASSIGN_DIV"},
    // {ASSIGN_OP, "ASSIGN_OP"},
    {SPECIAL_CHAR, "SPECIAL_CHAR"},
    // {PLUS, "PLUS"},
    // {SUB, "SUB"},
    // {MUL, "MUL"},
    // {DIV, "DIV"},
    // {LESS_THAN, "LESS_THAN"},
    // {GREATER_THAN, "GREATER_THAN"},
    {BINOP, "BINOP"},
    {TYPEID, "TYPEID"},
    {OBJECTID, "OBJECTID"},
    {CONTINUE, "CONTINUE"},
    {BREAK, "BREAK"},
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
    return TOKEN_TYPE_TO_STRING[type];
  }
  std::string get_str() { return str; }

  void set_lineno(int n) { lineno = n; }
  void set_type(TokenType t) { type = t; }
  void set_str(std::string s) { str = s; }

  void dump() {
    std::cout << "#" << lineno << " " << TOKEN_TYPE_TO_STRING[type] << " " << str << std::endl;
  }

};

static std::set<TokenType> KEYWORDS_TTYPE {
  BRO, VIBESWITH, FOR, /* START, END, STEP,*/ IF, ELSE, WHILE, RETURN, // THIS
};

static std::unordered_map<std::string, TokenType> KEYWORDS_STR_TO_TTYPE {
  {"BRO", BRO},
  {"VIBESWITH", VIBESWITH},
  {"FOR", FOR},
  // {"START", START},
  // {"END", END},
  // {"STEP", STEP},
  {"IF", IF},
  {"ELSE", ELSE},
  {"WHILE", WHILE},
  {"RETURN", RETURN},
  {"THIS", THIS},
  {"CONTINUE", CONTINUE},
  {"BREAK", BREAK},
  /*
  {"INT_CONST", INT_CONST},
  {"BOOL_CONST", BOOL_CONST},
  {"VERSE_CONST", VERSE_CONST},
  {"ASSIGN", ASSIGN},
  {"LEQ", LEQ},
  {"GEQ", GEQ},
  {"EQ", EQ},
  {"ASSIGN_PLUS", ASSIGN_PLUS},
  {"ASSIGN_SUB", ASSIGN_SUB},
  {"ASSIGN_MUL", ASSIGN_MUL},
  {"ASSIGN_DIV", ASSIGN_DIV},
  */
};
