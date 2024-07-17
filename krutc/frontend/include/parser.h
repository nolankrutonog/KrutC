#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include <memory>
#include <set>
#include <string>
#include <unordered_map>

#include "constants.h"
#include "tokenbuffer.h"
#include "tree.h"

static int get_op_precedence(std::string op) { return BINOP_PRECEDENCE[op]; }

/*
  Class used for parsing. When parsing an expression, will push all expr tokens
  into seperate deque in order to properly parse expressions
*/
class ExprTQ {
 public:
  int lineno      = 0;
  int binop_index = -1;
  int binop_prec  = -1;
  // int paren_stack = 0;
  std::deque<Token> tq;

  void dump() {
    for (Token t : tq) {
      if (t.get_str() == "") {
        std::cout << t.get_type_str() + " ";
      } else {
        std::cout << t.get_str() + " ";
      }
    }
    std::cout << std::endl;
  }
};

class Parser {
 public:
  int parser_errors = 0;
  bool debug        = false;

  Parser(std::string filename, bool debug, bool token_dump)
      : tbuff(filename, token_dump), debug(debug), filename(filename) {}
  Program parse_program();
  void token_dump();
  bool check_lexer_errors();

 private:
  TokenBuffer tbuff;
  std::string filename;

  void panic_recover(std::set<std::string> ss);
  void debug_msg(std::string);

  Stmt *parse_stmt();

  Feature *parse_feature();
  AttrStmt *parse_attrstmt(Type_ *type);
  MethodStmt *parse_methodstmt(Type_ *type);
  Type_ *parse_typeexpr();

  FormalList parse_formallist();
  IfStmt *parse_if_stmt();
  WhileStmt *parse_while_stmt();
  ForStmt *parse_for_stmt();

  int parse_check_and_pop(std::string s);

  ClassStmt *parse_class_stmt();
  std::vector<std::string> get_parents();

  ExprTQ expr_tq;
  bool build_expr_tq(std::string s);
  void calibrate_expr_tq();
  ExprStmt *parse_exprstmt();
  BinopExpr *parse_binopexpr();

  ListConstExpr *parse_list_const_expr();
  ExprList parse_comma_separated_exprlist();
  DispatchExpr *parse_dispexpr();
  ListElemRef *parse_list_elem_ref_expr();
  SublistExpr *parse_sublist_expr(int, ExprStmt *);
  // SetConstExpr *parse_set_const_expr();
  int get_str_idx_expr_tq(std::string);

  IntConstExpr *parse_int_const_expr();
  BoolConstExpr *parse_bool_const_expr();
  StrConstExpr *parse_str_const_expr();
  CharConstExpr *parse_char_const_expr();
  ReturnExpr *parse_returnexpr();
  NewExpr *parse_newexpr();
  KillExpr *parse_killexpr();
  ObjectIdExpr *parse_objectid_expr();

  /* Print errors */
  void print_error(int lineno, const std::string &err_msg) {
    parser_errors++;
    std::cerr << filename << ":" << lineno << ": " << err_msg << std::endl;
  }
  void error_expected_token(int lineno, std::string tok) {
    std::string err_msg = "Error: Expected '" + tok + "'.";
    print_error(lineno, err_msg);
  }
  void error_unexpected_token(int lineno, std::string tok) {
    std::string err_msg = "Error: Unexpected '" + tok + "'.";
    print_error(lineno, err_msg);
  }

  void error_blank(int lineno, std::string err_msg) { print_error(lineno, err_msg); }

  void error_wrong_stmt_type(int lineno) {
    std::string err_msg = "Error: wrong stmt type.";
    print_error(lineno, err_msg);
  }
  void error_wrong_stmt_type_in_formal(int lineno) {
    std::string err_msg = "Error: wrong stmt type in formal.";
    print_error(lineno, err_msg);
  }
  void error_formal_init_not_null(int lineno) {
    std::string err_msg = "Error: Formal initialized to value.";
    print_error(lineno, err_msg);
  }
};

#endif  // PARSER_H
