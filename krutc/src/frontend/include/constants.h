#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <iostream>
#include <string>
#include <unordered_set>

namespace basic_classes {
const std::string Void = "void";
const std::string Object = "object";
const std::string Int = "int";
const std::string Bool = "bool";
const std::string Char = "char";
const std::string String = "string";
const std::string List = "list";

// TODO:
const std::string Set = "set";
const std::string Deci = "deci";
const std::string Dict = "dict";

inline bool is_basic_class(std::string &cname) {
  return cname == Void || cname == Object || cname == Int || cname == Bool ||
         cname == String || cname == Char || cname == List || cname == Set ||
         cname == Deci || cname == Dict;
}
}  // namespace basic_classes

namespace lexing {
const std::string Class = "CLASS";
const std::string Inherits = "INHERITS";
const std::string For = "FOR";
const std::string If = "IF";
const std::string Else = "ELSE";
const std::string While = "WHILE";
const std::string Return = "RETURN";
const std::string Continue = "CONTINUE";
const std::string Break = "BREAK";
const std::string New = "NEW";

inline bool is_uppercase_keyword(std::string &k) {
  return k == Class || k == Inherits || k == For || k == If || k == Else ||
         k == While || k == Return || k == Continue || k == Break || k == New;
}

const std::string OpenP = "(";
const std::string CloseP = ")";
const std::string SemiColon = ";";
const std::string OpenBrack = "[";
const std::string CloseBrack = "]";
const std::string OpenBrace = "{";
const std::string CloseBrace = "}";
const std::string Period = ".";
const std::string Comma = ",";
const std::string Colon = ":";

inline bool is_special_char(std::string &c) {
  return c == OpenP || c == CloseP || c == SemiColon || c == OpenBrack ||
         c == CloseBrack || c == OpenBrace || c == CloseBrace || c == Period ||
         c == Comma || c == Colon;
}

const std::string Plus = "+";
const std::string Minus = "-";
const std::string Times = "*";
const std::string Divide = "/";

const std::string LessThan = "<";
const std::string GreaterThan = ">";

const std::string LEQ = "<=";
const std::string GEQ = ">=";
const std::string Equal = "==";
const std::string NotEqual = "!=";
const std::string And = "&&";
const std::string Or = "||";

const std::string PlusEquals = "+=";
const std::string MinusEquals = "-=";
const std::string TimesEquals = "*=";
const std::string DivideEquals = "/=";

const std::string Define = "=";

inline bool is_binop(std::string &b) {
  return b == Plus || b == Minus || b == Times || b == Divide ||
         b == LessThan || b == GreaterThan || b == LEQ || b == GEQ ||
         b == Equal || b == NotEqual || b == And || b == Or ||
         b == PlusEquals || b == MinusEquals || b == TimesEquals ||
         b == DivideEquals || b == Define;
}
}  // namespace lexing

namespace typechecking {
/* basic class methods */
const std::string Constructor = "constructor";
const std::string Length = "length";
const std::string Clear = "clear";
const std::string Is_Empty = "is_empty";
const std::string Insert = "insert";
const std::string Remove = "remove";
const std::string Front = "front";
const std::string Back = "back";
const std::string Push_Back = "push_back";
const std::string Pop_Back = "pop_back";
const std::string Push_Front = "push_front";
const std::string Pop_Front = "pop_front";
const std::string Contains = "contains";

/* global builtin methods */
const std::string Main = "main";
const std::string Print = "print";
const std::string To_String = "to_string";
const std::string Type_Of = "type_of";
const std::string Abs = "abs";
const std::string Sum = "sum";
const std::string Min = "min";
const std::string Max = "max";
const std::string Input = "input";
const std::string Kill = "kill";
}  // namespace typechecking

enum TokenType {
  CLASS = 256,
  INHERITS = 257,
  FOR = 258,
  IF = 262,
  ELSE = 263,
  WHILE = 264,
  RETURN = 265,
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
  NEW = 291,
  CHAR_CONST = 292,
  DECI_CONST = 293,

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
    {NEW, "NEW"},

    {EMPTY, "EMPTY"},
    {ERROR, "ERROR"}

};

static std::unordered_map<std::string, TokenType> KEYWORDS_STR_TO_TTYPE{
    {"CLASS", CLASS},   {"INHERITS", INHERITS},
    {"FOR", FOR},       {"IF", IF},
    {"ELSE", ELSE},     {"WHILE", WHILE},
    {"RETURN", RETURN}, {"CONTINUE", CONTINUE},
    {"BREAK", BREAK},   {"NONE", NONE},
    {"NEW", NEW}};

static std::unordered_map<std::string, int> BINOP_PRECEDENCE = {
    {"+", 0},  {"-", 0},  {"*", 1},  {"/", 1},

    {"<", 2},  {">", 2},

    {"<=", 3}, {">=", 3}, {"==", 3}, {"!=", 3}, {"&&", 3}, {"||", 3},

    {"+=", 4}, {"-=", 4}, {"/=", 5}, {"*=", 5},

    {"=", 6},
};

#endif  // CONSTANTS_H