#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include <memory>
#include <string>
#include <set>
#include <unordered_map>
#include "../include/tokenbuffer.h"
#include "../include/tree.h"


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

class Parser {
public:
    int parser_errors = 0;
    bool tdump = false;
    bool debug = false;

    Parser(std::string filename, bool tdump, bool debug) 
      : tbuff(filename), filename(filename), tdump(tdump), debug(debug) {}
    void parse_program();
    void token_dump();
    Program program;

private:
    TokenBuffer tbuff;
    std::string filename;
    bool doing_formals = false;

    // std::set<StmtType> forbidden_attr_init_stmt_types {
    //   BRO_STMT, FOR_STMT, WHILE_STMT, RETURN_STMT, ASSIGN_STMT, IF_STMT, TYPEID_STMT 
    // };

    void panic_recover(); 
    void debug_msg(std::string);

    Stmt *parse_stmt();

    Feature* parse_feature();
    AttrStmt* parse_attrstmt();
    MethodStmt* parse_methodstmt();
    FormalStmt* parse_formalstmt();
    IfStmt *parse_if_stmt();
    WhileStmt *parse_while_stmt();
    ForStmt *parse_for_stmt();

    int parse_check_and_pop(Token t, std::string s);
    int parse_check_and_pop_type(Token t, TokenType type);

    BroStmt *parse_bro_stmt();
    std::vector<std::string> get_vibers();

    ExprStmt *parse_exprstmt();

    BinopExpr *parse_binopexpr(std::vector<Token> &token_vec, int index);
    int find_highest_binop_index(std::vector<Token> &tvec, int start_iter, int end_iter);

    DispatchExpr *parse_dispexpr(std::vector<Token> &token_vec);
    std::vector<Token> disp_config_name_and_caller(std::vector<Token> &token_vec, std::string &calling_class, std::string &name);
    int disp_arg_check(std::vector<Token> &arglist_tvec);

    IntConstExpr* parse_int_const_expr(Token t);
    BoolConstExpr* parse_bool_const_expr(Token t);
    VerseConstExpr* parse_verse_const_expr(Token t);
    ReturnExpr *parse_returnexpr();
    ObjectIdExpr *parse_objectid_expr(Token t);

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
