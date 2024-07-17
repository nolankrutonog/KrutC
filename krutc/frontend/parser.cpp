#include "include/parser.h"

#include <cassert>
#include <cctype>
#include <iostream>
#include <set>
#include <stack>

using namespace std;

void Parser::panic_recover(set<string> ss) {
  while (true) {
    Token t = tbuff.lookahead(0);
    if ((ss.find(t.get_str()) != ss.end()) || t.get_type() == EMPTY) {
      break;
    }
    tbuff.get_next();
  }
}

////////////////////////////////////////////////////////////
//
//
// Statements
//
//
////////////////////////////////////////////////////////////
FormalList Parser::parse_formallist() {
  debug_msg("BEGIN parse_formal_stmt()");

  FormalList formal_list;

  while (tbuff.lookahead(0).get_str() != ")") {
    Type_ *type;
    string name;
    Token next = tbuff.lookahead(0);
    int lineno = next.get_lineno();
    if (next.get_type() != TYPEID) {
      error_blank(lineno, "Error: Expected type in formal definition.");
      if (next.get_str() == ")") {
        break;
      }
    } else {
      type = parse_typeexpr();
    }

    next = tbuff.lookahead(0);
    if (next.get_type() != OBJECTID) {
      error_blank(lineno, "Error: Variable name required for formal parameter definition.");
      if (next.get_str() == ")") {
        break;
      }
    } else {
      name = next.get_str();
    }
    tbuff.get_next();

    FormalStmt *f = new FormalStmt(type, name);
    f->lineno     = lineno;
    formal_list.push_back(f);

    if (tbuff.lookahead(0).get_str() == ",") {
      tbuff.get_next();
      if (tbuff.lookahead(0).get_str() == ")") {
        error_blank(lineno, "Error: Expected another formal definition.");
        break;
      }
    } else if (tbuff.lookahead(0).get_str() == ")") {
      break;
    } else {
      error_blank(lineno, "Error: Expected ',' between formal definitions or ')' ");
      break;
    }
  }

  debug_msg("END parse_formal_stmt()");
  return formal_list;
}

/* type name(FormalList) {StmtList};*/
MethodStmt *Parser::parse_methodstmt(Type_ *type) {
  debug_msg("BEGIN parse_method_stmt()");
  int methodstmt_lineno = tbuff.lookahead(0).get_lineno();
  string name;
  FormalList formal_list;
  StmtList stmt_list;

  Token next = tbuff.lookahead(0);
  if (next.get_type() != OBJECTID) {
    error_blank(next.get_lineno(), "Error: expected a name for the method.");
    name = "";
  } else {
    name = tbuff.get_next().get_str();
  }

  parse_check_and_pop("("); // pop leading "("

  formal_list = parse_formallist();

  parse_check_and_pop(")"); // pop closing ')'

  parse_check_and_pop("{"); // parse opening {

  while (tbuff.lookahead(0).get_str() != "}" && tbuff.has_next()) {
    Stmt *stmt = parse_stmt();
    if (dynamic_cast<ClassStmt *>(stmt)) {
      string err_msg = "Error: Class can only be defined in outer scope.";
      error_blank(stmt->lineno, err_msg);
    }
    stmt_list.push_back(stmt);
  }

  parse_check_and_pop("}");

  MethodStmt *methodstmt = new MethodStmt(type, name, formal_list, stmt_list);
  methodstmt->lineno     = methodstmt_lineno;
  debug_msg("END parse_method_stmt()");

  return methodstmt;
}

AttrStmt *Parser::parse_attrstmt(Type_ *type) {
  debug_msg("BEGIN parse_attrstmt()");
  int attrstmt_lineno = tbuff.lookahead(0).get_lineno();
  string name;
  ExprStmt *init;

  Token name_tok = tbuff.lookahead(0);
  if (name_tok.get_type() != OBJECTID) {
    error_blank(name_tok.get_lineno(), "Error: expected OBJECTID after type declaration.");
  } else {
    name = name_tok.get_str();
    tbuff.get_next();
  }

  Token decider_tok = tbuff.lookahead(0);
  if (decider_tok.get_str() == ";") {
    tbuff.get_next(); // pop ';'
    init = NULL;
  } else if (decider_tok.get_str() == "=") {
    tbuff.get_next(); // pop "="
    if (tbuff.lookahead(0).get_str() == ";") {
      error_blank(tbuff.lookahead(0).get_lineno(), "Error: expected expression after attribute definition.");
      init = NULL;
    } else if (!build_expr_tq(";"))
      init = NULL;
    else
      init = parse_exprstmt();
    parse_check_and_pop(";");
  } else {
    /* this should never hit because its checked in parse_features() */
    error_blank(decider_tok.get_lineno(), "Error: Expected either ';' or expression following attribute "
                                          "declaration.");
  }

  AttrStmt *attrstmt = new AttrStmt(type, name, init);
  attrstmt->lineno   = attrstmt_lineno;
  debug_msg("END parse_attrstmt()");
  return attrstmt;
}

Type_ *Parser::parse_typeexpr() {
  debug_msg("BEGIN parse_typeexpr()");

  string name;
  Type_ *nested;
  int lineno = tbuff.lookahead(0).get_lineno();

  Token name_tok = tbuff.lookahead(0);
  if (name_tok.get_type() != TYPEID) {
    error_blank(lineno, "Error: Expected typeid in type declaration. Instead got `" + name_tok.get_str() + "`");
    name = "";
  } else {
    name = name_tok.get_str();
  }
  tbuff.get_next(); // pop outer Type_

  if (tbuff.lookahead(0).get_str() == "<") {
    tbuff.get_next(); // pop opening '<'
    nested = parse_typeexpr();
    if (tbuff.lookahead(0).get_str() == ">") {
      tbuff.get_next(); // pop closing '>'
    } else {
      error_blank(tbuff.lookahead(0).get_lineno(), "Error: Expected closing carrot '>'.");
    }
  } else {
    nested = NULL;
  }

  Type_ *expr  = new Type_(name, nested);
  expr->lineno = lineno;
  debug_msg("END parse_typeexpr()");
  return expr;
}

Feature *Parser::parse_feature() {
  debug_msg("BEGIN parse_feature()");
  assert(tbuff.lookahead(0).get_type() == TYPEID);

  if (tbuff.lookahead(1).get_str() == "(") {}

  Type_ *type = parse_typeexpr();

  if (tbuff.lookahead(0).get_str() == ">") {
    error_blank(tbuff.lookahead(0).get_lineno(), "Error: unbalanced '<' '>' in type declaration.");
    tbuff.get_next();

    while (tbuff.lookahead(0).get_str() == ">") {
      tbuff.get_next();
    }
  }

  Feature *f;
  if (tbuff.lookahead(0).get_str() == "(" || tbuff.lookahead(1).get_str() == "(") {
    f = parse_methodstmt(type);
  } else if (tbuff.lookahead(1).get_str() == ";" || tbuff.lookahead(1).get_str() == "=") {
    f = parse_attrstmt(type);
  } else {
    string err_msg = "Syntax error at token " + tbuff.lookahead(2).get_str() + ". Expected one of ';', '=', '('";
    error_blank(tbuff.lookahead(2).get_lineno(), err_msg);
    return NULL;
  }
  debug_msg("END parse_feature()");

  return f;
}

int Parser::parse_check_and_pop(string s) {
  if (!tbuff.has_next()) {
    error_blank(0, "Error: Expected '" + s + "', instead got EOF");
    return -1;
  }
  Token t = tbuff.lookahead(0);
  if (t.get_str() != s) {
    error_blank(t.get_lineno(), "Error: Expected '" + s + "', instead got '" + t.get_str() + "'.");
    panic_recover({s});
    tbuff.get_next(); // pop t
    return -1;
  }
  tbuff.get_next(); // pop t
  return 1;
}

ForStmt *Parser::parse_for_stmt() {
  debug_msg("BEGIN parse_for_stmt()");
  int forstmt_lineno = tbuff.lookahead(0).get_lineno();
  Stmt *stmt;
  ExprStmt *cond;
  ExprStmt *repeat;
  StmtList stmt_list;

  Token for_tok = tbuff.lookahead(0);
  assert(for_tok.get_type() == FOR);
  tbuff.get_next(); // pop 'FOR'

  parse_check_and_pop("(");

  Token next = tbuff.lookahead(0);
  if (next.get_str() == ";") {
    stmt = NULL;
    parse_check_and_pop(";"); // pop ';'
  } else {
    stmt = parse_stmt();

    if (dynamic_cast<ClassStmt *>(stmt)) {
      cout << "You found a lil easter egg hehehe congrats! email me "
              "nolankrutonog@gmail.com and you'll get a prize"
           << endl;
    }

    if (tbuff.lookahead(0).get_str() == ";") {
      /* if stmt is of type Stmt */
      parse_check_and_pop(";");
    }
  }

  if (tbuff.lookahead(0).get_str() != ";") {
    if (!build_expr_tq(";"))
      stmt = NULL;
    else
      cond = parse_exprstmt();
  } else {
    cond = NULL;
  }
  parse_check_and_pop(";"); // pop ';'

  if (tbuff.lookahead(0).get_str() != ")") {
    if (!build_expr_tq(")"))
      repeat = NULL;
    else
      repeat = parse_exprstmt();
  } else {
    repeat = NULL;
  }
  parse_check_and_pop(")"); // pop ')'

  parse_check_and_pop("{");

  while (tbuff.has_next() && tbuff.lookahead(0).get_str() != "}") {
    Stmt *stmt = parse_stmt();
    if (dynamic_cast<ClassStmt *>(stmt)) {
      string err_msg = "Error: Class can only be defined in outer scope";
      error_blank(stmt->lineno, err_msg);
    }
    stmt_list.push_back(stmt);
  }

  parse_check_and_pop("}");

  ForStmt *for_stmt = new ForStmt(stmt, cond, repeat, stmt_list);
  for_stmt->lineno  = forstmt_lineno;
  debug_msg("END parse_for_stmt()");
  return for_stmt;
}

vector<string> Parser::get_parents() {
  tbuff.get_next(); // pop keyword INHERITS

  vector<string> parents;
  while (tbuff.has_next() && tbuff.lookahead(0).get_str() != "{") {
    Token parent = tbuff.lookahead(0);
    if (parent.get_type() != TYPEID) {
      string err_msg = "Error: INHERITEE cannot be of type " + parent.get_type_str();
      error_blank(parent.get_lineno(), err_msg);
      /* dont need to return now because we may be able to recover */
    } else {
      // if (parents.find(parent.get_str()) != parents.end()) {
      if (find(parents.begin(), parents.end(), parent.get_str()) != parents.end()) {
        string err_msg = "Error: INHERITEE " + parent.get_str() + " cannot be defined twice.";
        error_blank(parent.get_lineno(), err_msg);
      } else {
        parents.push_back(parent.get_str());
      }
    }
    tbuff.get_next(); // pop parent token

    Token next = tbuff.lookahead(0);
    if (next.get_str() == "{") // dont pop '{'
      break;
    if (next.get_str() == ",") { // pop ','
      tbuff.get_next();
      continue;
    } else {
      string err_msg = "Error: Expected ',' or '{' after INHERITEE.";
      error_blank(next.get_lineno(), err_msg);
      return parents;
    }
  }
  return parents;
}

/* CLASS TYPEID [INHERITS [OBJECTID[, OBJECTID]*]]? { STMTLIST }; */
ClassStmt *Parser::parse_class_stmt() {
  debug_msg("BEGIN parse_class_stmt()");
  int classstmt_lineno = tbuff.lookahead(0).get_lineno();
  string name;
  vector<string> parents;
  FeatureList feature_list;

  Token class_tok = tbuff.lookahead(0);
  assert(class_tok.get_type() == CLASS);
  tbuff.get_next(); // pop CLASS

  Token next = tbuff.lookahead(0);
  if (next.get_type() != TYPEID) {
    string err_msg = "Error: CLASS name must start with a capital letter.";
    error_blank(next.get_lineno(), err_msg);
  }

  name = tbuff.get_next().get_str();

  next = tbuff.lookahead(0);
  if (next.get_type() == INHERITS) {
    parents = get_parents();
    if (parents.size() == 0) {
      string err_msg = "Error: Every CLASS must have one or more parent if "
                       "keyword INHERITS is present";
      error_blank(next.get_lineno(), err_msg);
    }
  }

  parse_check_and_pop("{");

  while (tbuff.has_next() && tbuff.lookahead(0).get_str() != "}") {
    if (tbuff.lookahead(0).get_type() != TYPEID) {
      string err_msg = "Error: Every CLASS must be defined by only METHODS or ATTRIBUTES.";
      error_blank(tbuff.lookahead(0).get_lineno(), err_msg);
      // recover to next typeid
      panic_recover({"}", ";"});
      tbuff.get_next();
      continue;
    }
    Feature *f = parse_feature();
    feature_list.push_back(f);
  }

  parse_check_and_pop("}");

  ClassStmt *class_stmt = new ClassStmt(name, parents, feature_list);
  class_stmt->lineno    = classstmt_lineno;
  debug_msg("END parse_class_stmt()");
  return class_stmt;
}

IfStmt *Parser::parse_if_stmt() {
  debug_msg("BEGIN parse_if_stmt()");
  int lineno = tbuff.lookahead(0).get_lineno();
  ExprStmt *pred;
  StmtList then_branch;
  StmtList else_branch;
  tbuff.get_next(); // pop if

  /* evaluate expression */
  parse_check_and_pop("(");
  if (!build_expr_tq(")"))
    pred = NULL;
  else
    pred = parse_exprstmt();
  parse_check_and_pop(")");

  parse_check_and_pop("{");

  while (true) {
    if (tbuff.lookahead(0).get_str() == "}" || tbuff.lookahead(0).get_type() == EMPTY) {
      break;
    }
    Stmt *stmt = parse_stmt();
    if (stmt) {
      if (dynamic_cast<ClassStmt *>(stmt)) {
        string err_msg = "Error: Class can only be defined in outer scope";
        error_blank(stmt->lineno, err_msg);
      }

      then_branch.push_back(stmt);
    }
  }

  parse_check_and_pop("}");

  if (tbuff.lookahead(0).get_type() == ELSE) {
    tbuff.get_next(); // pop else
    parse_check_and_pop("{");

    while (true) {
      if (tbuff.lookahead(0).get_str() == "}" || tbuff.lookahead(0).get_type() == EMPTY) {
        break;
      }
      Stmt *stmt = parse_stmt();
      if (stmt) {
        if (dynamic_cast<ClassStmt *>(stmt)) {
          string err_msg = "Error: Class can only be defined in outer scope";
          error_blank(stmt->lineno, err_msg);
        }
        else_branch.push_back(stmt);
      }
    }

    parse_check_and_pop("}");
  }

  IfStmt *if_stmt = new IfStmt(pred, then_branch, else_branch);
  if_stmt->lineno = lineno;
  debug_msg("END parse_if_stmt()");
  return if_stmt;
}

WhileStmt *Parser::parse_while_stmt() {
  debug_msg("BEGIN parse_while_stmt()");
  int lineno = tbuff.lookahead(0).get_lineno();
  ExprStmt *pred;
  StmtList stmt_list;

  tbuff.get_next(); // pop 'while'

  /* evaluate expression */
  parse_check_and_pop("(");
  if (!build_expr_tq(")"))
    pred = NULL;
  else
    pred = parse_exprstmt();
  parse_check_and_pop(")");

  parse_check_and_pop("{");

  while (true) {
    if (tbuff.lookahead(0).get_str() == "}" || tbuff.lookahead(0).get_type() == EMPTY) {
      break;
    }
    Stmt *stmt = parse_stmt();
    if (stmt) {
      if (dynamic_cast<ClassStmt *>(stmt)) {
        string err_msg = "Error: Class can only be defined in outer scope";
        error_blank(stmt->lineno, err_msg);
      }
      stmt_list.push_back(stmt);
    }
  }

  parse_check_and_pop("}");

  WhileStmt *whilestmt = new WhileStmt(pred, stmt_list);
  whilestmt->lineno    = lineno;
  debug_msg("END parse_while_stmt()");
  return whilestmt;
}

////////////////////////////////////////////////////////////
//
//
// Expressions
//
//
////////////////////////////////////////////////////////////
void Parser::calibrate_expr_tq() {
  int lineno      = expr_tq.tq.front().get_lineno();
  int binop_index = -1;
  int binop_prec  = -1;
  stack<string> stk;

  for (int i = 0; i < (int)expr_tq.tq.size(); i++) {
    Token t = expr_tq.tq[i];
    if (t.get_str() == "(" || t.get_str() == "[")
      stk.push(t.get_str());
    else if (t.get_str() == ")" || t.get_str() == "]")
      stk.pop();
    else if (t.get_type() == BINOP && stk.empty()) {
      int curr_precedence = get_op_precedence(t.get_str());
      if (curr_precedence > binop_prec) {
        binop_prec  = curr_precedence;
        binop_index = i;
      }
    }
  }
  expr_tq.lineno      = lineno;
  expr_tq.binop_index = binop_index;
  expr_tq.binop_prec  = binop_prec;
}

/* builds up an expression token deque which stops after finding 'S'*/
bool Parser::build_expr_tq(string s) {
  // deque<Token> tvec;
  int lineno        = tbuff.lookahead(0).get_lineno();
  int binop_index   = -1;
  int binop_prec    = -1;
  int paren_stack   = 0;
  int bracket_stack = 0;

  stack<string> stk;
  bool stk_error = false;

  int i = -1;
  while (true) {
    i++;
    Token t = tbuff.lookahead(0);
    /* regardless of S, expr cannot include ';' */
    if (t.get_type() == EMPTY || t.get_str() == ";") {
      break;
    }

    /* sometimes the expression ends with a ')' and expression contains parens
     */
    if (t.get_str() == s) {
      if (s != ")")
        break;
      else if (stk.empty())
        break;
    }

    if (t.get_str() == "[" || t.get_str() == "(")
      stk.push(t.get_str());
    else if (t.get_str() == ")") {
      if (!stk.empty()) {
        string top = stk.top();
        if (top == "(") {
          stk.pop();
        } else {
          stk_error = true;
        }
      } else {
        stk_error = true;
      }
    } else if (t.get_str() == "]") {
      if (!stk.empty()) {
        string top = stk.top();
        if (top == "[") {
          stk.pop();
        } else {
          stk_error = true;
        }
      } else {
        stk_error = true;
      }
    }

    else if (stk.empty() && t.get_type() == BINOP) {
      int curr_prec = get_op_precedence(t.get_str());
      if (curr_prec > binop_prec) {
        binop_index = i;
        binop_prec  = curr_prec;
      }
    }

    expr_tq.tq.push_back(t);
    tbuff.get_next();
  }

  expr_tq.lineno      = lineno;
  expr_tq.binop_index = binop_index;
  expr_tq.binop_prec  = binop_prec;

  if (!stk.empty() || stk_error) {
    error_blank(lineno, "Error: unbalanced '[' ']' or '(' ')'.");
    expr_tq.tq.clear();
    return false;
  }

  return true;
}

ExprStmt *Parser::parse_exprstmt() {
  debug_msg("BEGIN: parse_exprstmt()");

  if (expr_tq.tq.empty())
    return NULL;

  calibrate_expr_tq();

  ExprStmt *expr;
  int lineno = expr_tq.tq.front().get_lineno();

  /* check to see expr type */
  Token t = expr_tq.tq.front();
  if (expr_tq.binop_index != -1) {
    expr = parse_binopexpr();
  } else if (t.get_type() == RETURN) {
    expr = parse_returnexpr();
  } else if (t.get_type() == NEW) {
    expr = parse_newexpr();
  } else if (t.get_type() == KILL) {
    expr = parse_killexpr();
  } else if (t.get_type() == INT_CONST) {
    expr = parse_int_const_expr();
  } else if (t.get_type() == BOOL_CONST) {
    expr = parse_bool_const_expr();
  } else if (t.get_type() == STR_CONST) {
    expr = parse_str_const_expr();
  } else if (t.get_type() == CHAR_CONST) {
    expr = parse_char_const_expr();
  } else if (t.get_type() == CONTINUE) {
    /* TODO: ensure works after cont/break that nothing follows */
    expr = new ContExpr();
    expr_tq.tq.pop_front();
  } else if (t.get_type() == BREAK) {
    expr = new BreakExpr();
    expr_tq.tq.pop_front();
  }
  // else if (t.get_type() == THIS) {
  //   expr = new ThisExpr();
  //   expr_tq.tq.pop_front();
  // }
  else if (t.get_type() == OBJECTID && expr_tq.tq.size() == 1) {
    expr = parse_objectid_expr();
  } else if (t.get_str() == "[" && expr_tq.tq.back().get_str() == "]") {
    expr = parse_list_const_expr();
  } else if (t.get_str() == "(" && expr_tq.tq.back().get_str() == ")") {
    expr_tq.tq.pop_front();
    expr_tq.tq.pop_back();
    expr = parse_exprstmt();
  } else if (expr_tq.tq.back().get_str() == ")") {
    expr = parse_dispexpr();
  } else if (expr_tq.tq.back().get_str() == "]") {
    expr = parse_list_elem_ref_expr();
  }

  if (!expr_tq.tq.empty()) {
    if (t.get_type() == OBJECTID) {
      // error here on an attribute that is not basic and lowercase
      // ex: gang g;
    }
    bool val = expr_tq.tq.back().get_str() == ")";
    error_blank(lineno, "Error, unrecognized expression.");
    expr_tq.dump();
    expr_tq.tq.clear();
    return NULL;
  }

  expr->lineno = lineno;
  debug_msg("END: parse_exprstmt()");
  return expr;
}

ExprList Parser::parse_comma_separated_exprlist() {
  ExprList exprlist;

  ExprTQ original = expr_tq;
  expr_tq.tq.clear();

  int stack = 0;
  while (!original.tq.empty()) {
    Token t = original.tq.front();
    original.tq.pop_front();
    if (t.get_str() == "(" || t.get_str() == "[")
      stack++;
    else if (t.get_str() == ")" || t.get_str() == "]")
      stack--;
    else if (t.get_str() == "," && stack == 0) {
      if (original.tq.empty()) {
        error_blank(t.get_lineno(), "Error: expected expression after ','");
      }

      ExprStmt *expr = parse_exprstmt();
      if (expr)
        exprlist.push_back(expr);
      continue;
    }

    expr_tq.tq.push_back(t);
  }

  /* for the spill over*/
  if (!expr_tq.tq.empty()) {
    ExprStmt *expr = parse_exprstmt();
    if (expr)
      exprlist.push_back(expr);
  }

  return exprlist;
}

ListConstExpr *Parser::parse_list_const_expr() {
  debug_msg("BEGIN parse_list_const_expr()");
  ExprList exprlist;
  int lineno = expr_tq.tq[0].get_lineno();
  assert(expr_tq.tq.front().get_str() == "[");
  assert(expr_tq.tq.back().get_str() == "]");

  if (expr_tq.tq.empty()) {
    /* this is allowed. ex: list l = []; */
    return NULL;
  }

  expr_tq.tq.pop_front();
  expr_tq.tq.pop_back();

  if (!expr_tq.tq.empty()) {
    exprlist = parse_comma_separated_exprlist();
  }

  ListConstExpr *list_const_expr = new ListConstExpr(exprlist);
  list_const_expr->lineno        = lineno;
  debug_msg("END parse_list_const_expr()");
  return list_const_expr;
}

SublistExpr *Parser::parse_sublist_expr(int colon_idx, ExprStmt *list_name) {
  ExprStmt *st_idx;
  ExprStmt *end_idx;

  ExprTQ original = expr_tq;
  expr_tq.tq.clear();
  for (int i = 0; i < colon_idx; i++) {
    expr_tq.tq.push_back(original.tq[i]);
  }
  if (!expr_tq.tq.empty())
    st_idx = parse_exprstmt();
  else // need to be set to NULL because they are optional
    st_idx = NULL;

  expr_tq.tq.clear();
  for (int i = colon_idx + 1; i < (int)original.tq.size(); i++) {
    expr_tq.tq.push_back(original.tq[i]);
  }
  if (!expr_tq.tq.empty())
    end_idx = parse_exprstmt();
  else // need to be set to NULL because they are optional
    end_idx = NULL;

  SublistExpr *sle = new SublistExpr(list_name, st_idx, end_idx);
  return sle;
}

ListElemRef *Parser::parse_list_elem_ref_expr() {
  int lineno = expr_tq.tq[0].get_lineno();
  ExprStmt *list_name;
  ExprStmt *index;

  assert(expr_tq.tq.back().get_str() == "]");

  int open_brack_idx = get_str_idx_expr_tq("[");
  ExprTQ original    = expr_tq;
  expr_tq.tq.clear();
  for (int i = 0; i < (int)open_brack_idx; i++) {
    expr_tq.tq.push_back(original.tq[i]);
  }
  list_name = parse_exprstmt();

  assert(expr_tq.tq.empty());

  // also need to check for ":" in case it's SublistExpr
  int stk              = -1; // starts at -1 because the first char is '['
  bool is_sublist_expr = false;
  int colon_idx        = -1;
  for (int i = open_brack_idx; i < (int)original.tq.size(); i++) {
    if (original.tq[i].get_str() == "(" || original.tq[i].get_str() == "[") {
      stk++;
    } else if (original.tq[i].get_str() == ")" || original.tq[i].get_str() == "]") {
      stk--;
    }
    if (stk == 0 && original.tq[i].get_str() == ":") {
      colon_idx       = i - open_brack_idx - 1;
      is_sublist_expr = true;
    }

    expr_tq.tq.push_back(original.tq[i]);
  }

  assert(expr_tq.tq[0].get_str() == "[" && expr_tq.tq[expr_tq.tq.size() - 1].get_str() == "]");

  expr_tq.tq.pop_front();
  expr_tq.tq.pop_back();

  if (is_sublist_expr) {
    SublistExpr *sl = parse_sublist_expr(colon_idx, list_name);
    sl->lineno      = lineno;
    return sl;
  }

  index = parse_exprstmt();

  ListElemRef *list_elem_ref = new ListElemRef(list_name, index);
  list_elem_ref->lineno      = lineno;
  return list_elem_ref;
}

int Parser::get_str_idx_expr_tq(string s) {
  int idx = -1;

  stack<string> stk;
  for (int i = (int)expr_tq.tq.size() - 1; i >= 0; i--) {
    Token t = expr_tq.tq[i];
    if (t.get_str() == "]" || t.get_str() == ")") {
      stk.push(t.get_str());
    } else if (t.get_str() == "(" || t.get_str() == "[") {
      stk.pop();
    }

    if (stk.empty() && t.get_str() == s) {
      idx = i;
      break;
    }
  }

  // assert(idx != -1);
  assert(stk.empty());

  return idx;
}

DispatchExpr *Parser::parse_dispexpr() {
  debug_msg("BEGIN parse_dispexpr()");
  int lineno = expr_tq.tq.front().get_lineno();
  ExprStmt *calling_class;
  string name;
  ExprList args;

  int dot_index   = get_str_idx_expr_tq(".");
  ExprTQ original = expr_tq;
  expr_tq.tq.clear();
  for (int i = 0; i < dot_index; i++) {
    expr_tq.tq.push_back(original.tq[i]);
  }
  /* parse on tokens from 0 to dot_index */
  calling_class = parse_exprstmt();
  assert(expr_tq.tq.empty());

  for (int i = dot_index + 1; i < original.tq.size(); i++) {
    expr_tq.tq.push_back(original.tq[i]);
  }

  name = expr_tq.tq[0].get_str();
  expr_tq.tq.pop_front();

  assert(expr_tq.tq.front().get_str() == "(");
  assert(expr_tq.tq.back().get_str() == ")");

  expr_tq.tq.pop_front();
  expr_tq.tq.pop_back();

  if (!expr_tq.tq.empty())
    args = parse_comma_separated_exprlist();

  DispatchExpr *disp_expr = new DispatchExpr(calling_class, name, args);
  disp_expr->lineno       = lineno;
  return disp_expr;
}

ReturnExpr *Parser::parse_returnexpr() {
  debug_msg("BEGIN parse_returnexpr()");
  int lineno = expr_tq.tq[0].get_lineno();
  ExprStmt *expr;

  assert(expr_tq.tq.front().get_type() == RETURN);

  expr_tq.tq.pop_front();
  calibrate_expr_tq();
  expr = parse_exprstmt();

  ReturnExpr *retexpr = new ReturnExpr(expr);
  retexpr->lineno     = lineno;
  debug_msg("END parse_returnexpr()");
  return retexpr;
}

KillExpr *Parser::parse_killexpr() {
  debug_msg("BEGIN parse_killexpr()");
  int lineno = expr_tq.tq[0].get_lineno();
  ExprStmt *expr;

  assert(expr_tq.tq.front().get_type() == KILL);

  expr_tq.tq.pop_front(); // pop 'kill'
  calibrate_expr_tq();
  expr               = parse_exprstmt();
  KillExpr *killexpr = new KillExpr(expr);
  killexpr->lineno   = lineno;
  debug_msg("END parse_killexpr()");
  return killexpr;
}

NewExpr *Parser::parse_newexpr() {
  debug_msg("BEGIN parse_newexpr()");
  int lineno = expr_tq.tq[0].get_lineno();
  ExprStmt *expr;

  assert(expr_tq.tq.front().get_type() == NEW);

  expr_tq.tq.pop_front();
  if (expr_tq.tq.empty() || expr_tq.tq.front().get_type() != TYPEID) {
    string err_msg = "Error: There must be a class constructor following keyword NEW.";
    error_blank(lineno, err_msg);
    panic_recover({";"});
    return NULL;
  }

  NewExpr *newexpr = new NewExpr(expr_tq.tq.front().get_str());
  expr_tq.tq.pop_front();
  newexpr->lineno = lineno;
  debug_msg("END parse_newexpr()");
  return newexpr;
}

BinopExpr *Parser::parse_binopexpr() {
  debug_msg("BEGIN parse_binopexpr()");
  int lineno = expr_tq.tq[expr_tq.binop_index].get_lineno();
  ExprStmt *lhs;
  string op;
  ExprStmt *rhs;

  Token op_tok = expr_tq.tq[expr_tq.binop_index];
  assert(op_tok.get_type() == BINOP);
  op = op_tok.get_str();

  ExprTQ original = expr_tq;

  /* create lhs expr_tq */
  expr_tq.tq.clear();
  for (int i = 0; i < original.binop_index; i++) {
    expr_tq.tq.push_back(original.tq[i]);
  }
  lhs = parse_exprstmt();

  /* create rhs expr_tq */
  expr_tq.tq.clear();
  for (int i = original.binop_index + 1; i < original.tq.size(); i++) {
    expr_tq.tq.push_back(original.tq[i]);
  }
  rhs = parse_exprstmt();

  BinopExpr *binop = new BinopExpr(lhs, op, rhs);
  binop->lineno    = lineno;
  debug_msg("END parse_binopexpr()");
  return binop;
}

IntConstExpr *Parser::parse_int_const_expr() {
  debug_msg("BEGIN: parse_int_const_expr()");
  Token t = expr_tq.tq.front();
  expr_tq.tq.pop_front();
  int lineno           = t.get_lineno();
  long val             = stol(t.get_str());
  IntConstExpr *icstmt = new IntConstExpr(val);
  icstmt->lineno       = lineno;
  debug_msg("END: parse_int_const_expr()");
  return icstmt;
}
BoolConstExpr *Parser::parse_bool_const_expr() {
  Token t = expr_tq.tq.front();
  expr_tq.tq.pop_front();
  int lineno            = t.get_lineno();
  string val            = t.get_str();
  BoolConstExpr *bcstmt = new BoolConstExpr(val);
  bcstmt->lineno        = lineno;
  return bcstmt;
}
StrConstExpr *Parser::parse_str_const_expr() {
  Token t = expr_tq.tq.front();
  expr_tq.tq.pop_front();
  int lineno            = t.get_lineno();
  string val            = t.get_str();
  StrConstExpr *strstmt = new StrConstExpr(val);
  strstmt->lineno       = lineno;
  return strstmt;
}

CharConstExpr *Parser::parse_char_const_expr() {
  Token t = expr_tq.tq.front();
  expr_tq.tq.pop_front();
  int lineno              = t.get_lineno();
  string c                = t.get_str();
  CharConstExpr *charexpr = new CharConstExpr(c);
  charexpr->lineno        = lineno;
  return charexpr;
}

ObjectIdExpr *Parser::parse_objectid_expr() {
  debug_msg("BEGIN parse_objectid_expr()");
  Token t = expr_tq.tq.front();
  expr_tq.tq.pop_front();
  ObjectIdExpr *objidexpr = new ObjectIdExpr(t.get_str());
  objidexpr->lineno       = t.get_lineno();
  debug_msg("END parse_objectid_expr()");
  return objidexpr;
}

Stmt *Parser::parse_stmt() {
  debug_msg("BEGIN parse_stmt()");
  Stmt *stmt;
  Token t = tbuff.lookahead(0);
  if (t.get_type() == TYPEID && tbuff.lookahead(1).get_str() != "(") {
    stmt = parse_feature();
  } else if (t.get_type() == FOR) {
    stmt = parse_for_stmt();
  } else if (t.get_type() == CLASS) {
    stmt = parse_class_stmt();
  } else if (t.get_type() == IF) {
    stmt = parse_if_stmt();
  } else if (t.get_type() == WHILE) {
    stmt = parse_while_stmt();
  } else {
    if (!build_expr_tq(";"))
      stmt = NULL;
    else {
      stmt = parse_exprstmt();
    }
    if (parse_check_and_pop(";") == -1) {
      return NULL;
    }
  }
  debug_msg("END parse_stmt()");
  return stmt;
}

Program Parser::parse_program() {
  Program program;
  while (tbuff.has_next()) {
    Stmt *stmt = parse_stmt();
    if (stmt != NULL)
      program.add_stmt(stmt);
  }
  return program;
}

bool Parser::check_lexer_errors() {
  if (tbuff.has_errors) {
    token_dump();
    return false;
  }
  return true;
}

void Parser::token_dump() {
  if (tbuff.has_errors) {
    while (tbuff.has_next()) {
      Token t = tbuff.get_next();
      if (t.get_type() == ERROR) {
        error_blank(t.get_lineno(), t.get_str());
      }
    }
  } else {
    while (tbuff.has_next()) {
      Token t = tbuff.get_next();
      t.dump();
    }
  }
}

void Parser::debug_msg(string s) {
  if (debug)
    cout << s << endl;
}
