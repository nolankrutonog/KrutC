#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include <memory>
#include <string>
#include <set>
#include <unordered_map>
#include "tokenbuffer.h"
#include "tree.h"


static std::unordered_map<std::string, int> OP_PRECEDENCE = {
  {"+", 0},
  {"-", 0},
  {"*", 1},
  {"/", 1},

  {"<", 2},
  {">", 2},

  {"<=", 3},
  {">=", 3},
  {"==", 3},
  {"!=", 3},
  {"&&", 3},
  {"||", 3},

  {"+=", 4},
  {"-=", 4},
  {"/=", 5},
  {"*=", 5},

  {"=", 6},
};

static int get_op_precedence(std::string op) {
  return OP_PRECEDENCE[op];
}

class ExprTQ {
public:
  int lineno = 0;
  int binop_index = -1;
  int binop_prec = -1;
  int paren_stack = 0;
  std::deque<Token> tq;

  void dump() {
    for (Token t: tq) {
      std::cout << t.get_str() + " ";
    }
    std::cout << std::endl;
  }
};

class Parser {
public:
    int parser_errors = 0;
    // bool tdump = false;
    bool debug = false;
    // bool tree = false;

    Parser(std::string filename, bool debug) 
      : tbuff(filename), debug(debug), filename(filename) {}
    void parse_program();
    void token_dump();
    Program program;

private:
    TokenBuffer tbuff;
    std::string filename;
    bool doing_formals = false;

    void panic_recover(std::string s); 
    void debug_msg(std::string);


    Stmt *parse_stmt();

    Feature* parse_feature();
    AttrStmt* parse_attrstmt();
    MethodStmt* parse_methodstmt();
    FormalList parse_formallist();
    IfStmt *parse_if_stmt();
    WhileStmt *parse_while_stmt();
    ForStmt *parse_for_stmt();

    int parse_check_and_pop(std::string s);
    // int parse_check_and_pop_type(TokenType type);

    BroStmt *parse_bro_stmt();
    std::vector<std::string> get_vibers();

    ExprTQ expr_tq;
    bool build_expr_tq(std::string s);
    int calibrate_expr_tq();
    ExprStmt *parse_exprstmt();

    BinopExpr *parse_binopexpr();

    DispatchExpr *parse_dispexpr();
    // bool expr_tvec_size_check(int n);

    IntConstExpr* parse_int_const_expr();
    BoolConstExpr* parse_bool_const_expr();
    VerseConstExpr* parse_verse_const_expr();
    ReturnExpr *parse_returnexpr();
    NewExpr *parse_newexpr();
    KillExpr *parse_killexpr();
    ObjectIdExpr *parse_objectid_expr();

    /* Print errors */
    void print_error(int lineno, const std::string& err_msg) {
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

    void error_blank(int lineno, std::string err_msg) {
      print_error(lineno, err_msg);
    }

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

#endif // PARSER_H
