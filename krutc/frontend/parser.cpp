#include <iostream>

#include "parser.h"



using namespace std;


void Parser::panic_recover() {
  while (true) {
    if (tbuff.lookahead(0).get_type() == EMPTY || tbuff.lookahead(0).get_str() == ";") {
      // tbuff.get_next(); // pop ';'
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
FormalStmt *Parser::parse_formalstmt() {
  int formalstmt_lineno = tbuff.get_lineno();
  string type;
  string name;

  Token next = tbuff.lookahead(0);
  if (next.get_type() != TYPEID) {
    string err_msg = "Expected TYPE decl in formal definition";
    error_blank(tbuff.get_lineno(), err_msg);
    panic_recover();
    return NULL;
  }
  type = tbuff.get_next().get_str();

  next = tbuff.lookahead(0);
  if (next.get_type() != OBJECTID) {
    string err_msg = "Expected OBJECTID in method definition";
    error_blank(tbuff.get_lineno(), err_msg);
    panic_recover();
    return NULL;
  }
  name = tbuff.get_next().get_str();

  FormalStmt *formal = new FormalStmt(type, name);
  formal->lineno = formalstmt_lineno;
  return formal;
}

/* type name(FormalList) {StmtList};*/
MethodStmt *Parser::parse_methodstmt() {
  int methodstmt_lineno = tbuff.get_lineno();

  string type = tbuff.get_next().get_str();
  string name = tbuff.get_next().get_str();
  FormalList formal_list;  
  StmtList stmt_list;

  tbuff.get_next(); // pop leading '('
  // doing_formals = true;
  while (tbuff.lookahead(0).get_str() != ")") {
    FormalStmt *formal = parse_formalstmt();
    if (formal)
      formal_list.push_back(formal);
    if (tbuff.lookahead(0).get_str() == ",") {
      tbuff.get_next();
    }
  }
  // doing_formals = false;
  tbuff.get_next(); // pop closing ')'

  if (tbuff.lookahead(0).get_str() != "{") {
    error_expected_token(tbuff.get_lineno(), "{");
    panic_recover();
    return NULL;
  }

  tbuff.get_next(); // pop open '{'

  while (tbuff.lookahead(0).get_str() != "}") {
    Stmt *stmt = parse_stmt();
    stmt_list.push_back(stmt);
  }

  tbuff.get_next(); // pop closing '}'

  // if (tbuff.lookahead(0).get_str() != ";") {
  //   error_expected_token(tbuff.get_lineno(), ";");
  //   panic_recover();
  //   return NULL;
  // }
  // tbuff.get_next();

  MethodStmt *methodstmt = new MethodStmt(name, type, formal_list, stmt_list);
  methodstmt->lineno = methodstmt_lineno; 

  return methodstmt; 
}



AttrStmt *Parser::parse_attrstmt() {
  int attrstmt_lineno = tbuff.get_lineno();

  string type = tbuff.get_next().get_str();

  if (tbuff.lookahead(0).get_type() != OBJECTID) {
    error_blank(tbuff.get_lineno(), "Error: expected OBJECTID after type declaration.");
    panic_recover();
    return NULL;
  }

  string name = tbuff.get_next().get_str();
  ExprStmt *init;

  Token next = tbuff.lookahead(0);
  if (next.get_str() == ";") {
    tbuff.get_next(); // pop ';'
    init = NULL;
  } else if (next.get_str() == "="){ 
    tbuff.get_next(); // pop "="
    init = parse_exprstmt();
  } 

  AttrStmt *attrstmt = new AttrStmt(name, type, init);
  attrstmt->lineno = attrstmt_lineno;
  return attrstmt;
}


Feature *Parser::parse_feature() {
  Token tdecider = tbuff.lookahead(2); // 0 - type, 1 - name, 2 - = or (
  if (tdecider.get_str() == "=" || tdecider.get_str() == ";") {
    return parse_attrstmt();
  } else if (tdecider.get_str() == "(") {
    return parse_methodstmt();
  } else {
    string err_msg = "Syntax error at token " + tdecider.get_str() + ". Expected one of ';', '=', '('";
    error_blank(tdecider.get_lineno(), err_msg);
    panic_recover();
    return NULL;
  }

}


int Parser::parse_check_and_pop(Token t, string s) {
  if (t.get_str() != s) {
    error_expected_token(tbuff.get_lineno(), s);
    panic_recover();
    return -1;
  }
  tbuff.get_next(); // pop t
  return 1;
}

int Parser::parse_check_and_pop_type(Token t, TokenType type) {
  if (t.get_type() != type) {
    error_expected_token(tbuff.get_lineno(), TOKEN_TYPE_TO_STRING[type]);
    panic_recover();
    return -1;
  }
  tbuff.get_next(); // pop t
  return 1;
}

/* FOR (OBJECTID; START=EXPRSTMT; END=EXPRSTMT; STEP=EXPRSTMT) { STMTLIST }*/
ForStmt *Parser::parse_for_stmt() {
  int forstmt_lineno = tbuff.get_lineno();
  FormalStmt *formal;
  ExprStmt *start;
  ExprStmt *end;
  ExprStmt *step;
  StmtList stmt_list;

  tbuff.get_next().get_str(); // pop 'FOR'

  Token next = tbuff.lookahead(0);
  if (parse_check_and_pop(next, "(") == -1)
    return NULL;

  if (tbuff.lookahead(0).get_type() == TYPEID) {
    formal = parse_formalstmt(); /* can be separated by either , and ; */ 
  } else {
    formal = NULL;
  }

  next = tbuff.lookahead(0);
  if (parse_check_and_pop(next, ";") == -1) {
    return NULL;
  }

  if (tbuff.lookahead(0).get_str() != ";") {
    start = parse_exprstmt(); // ';' should alr be popped by parse_exprstmt()
  } else {
    start = NULL;
    tbuff.get_next(); // pop ';'
  }

  if (tbuff.lookahead(0).get_str() != ";") {
    end = parse_exprstmt(); // ';' should alr be popped by parse_exprstmt()
  } else {
    end = NULL;
    tbuff.get_next(); // pop ';'
  }

  if (tbuff.lookahead(0).get_str() != ";") {
    step = parse_exprstmt(); // ';' should alr be popped by parse_exprstmt()
  } else {
    step = NULL;
    tbuff.get_next(); // pop ';'
  }

  next = tbuff.lookahead(0);
  if (parse_check_and_pop(next, ")") == -1) {
    return NULL;
  }

  next = tbuff.lookahead(0);
  if (parse_check_and_pop(next, "{") == -1) {
    return NULL;
  }

  while (tbuff.has_next() && tbuff.lookahead(0).get_str() != "}") {
    Stmt *stmt = parse_stmt();
    stmt_list.push_back(stmt);
  }

  next = tbuff.lookahead(0);
  if (parse_check_and_pop(next, "}") == -1) {
    return NULL;
  }


  ForStmt *for_stmt = new ForStmt(formal, start, end, step, stmt_list);
  for_stmt->lineno = forstmt_lineno;
  return for_stmt;
}

vector<string> Parser::get_vibers() {
  tbuff.get_next(); // pop keyword VIBESWITH

  vector<string> vibers;
  while (tbuff.has_next() && tbuff.lookahead(0).get_str() != "{") {
    Token viber = tbuff.lookahead(0);
    if (viber.get_type() != TYPEID) {
      string err_msg = "Error: VIBER cannot be of type " + viber.get_type_str();
      error_blank(tbuff.get_lineno(), err_msg);
      panic_recover();
      /* dont need to return now because we may be able to recover */
    }
    vibers.push_back(tbuff.get_next().get_str());

    Token next = tbuff.lookahead(0);
    if (next.get_str() == "{")
      continue;
    if (next.get_str() == ",") {
      tbuff.get_next(); // pop ,
      continue;
    } else {
      string err_msg = "Error: Expected ',' between VIBERs, two VIBERs always need space";
      error_blank(tbuff.get_lineno(), err_msg);
      panic_recover();
      continue;
    }
  }
  return vibers;
}

/* BRO TYPEID [VIBESWITH [OBJECTID[, OBJECTID]*]]? { STMTLIST }; */
BroStmt *Parser::parse_bro_stmt() {
  int brostmt_lineno = tbuff.get_lineno();
  string name;
  vector<string> vibers;
  StmtList stmt_list;

  tbuff.get_next(); // pop BRO

  Token next = tbuff.lookahead(0);
  if (next.get_type() != TYPEID) {
    string err_msg = "Error: BRO cannot be of type " + next.get_type_str();
    error_blank(tbuff.get_lineno(), err_msg);
  }

  name = tbuff.get_next().get_str();

  next = tbuff.lookahead(0);
  if (next.get_type() == VIBESWITH) {
    vibers = get_vibers();
    if (vibers.size() == 0) {
      string err_msg = "Error: Every BRO must have one or more VIBER if keyword VIBESWITH is present";
      error_blank(tbuff.get_lineno(), err_msg);
    }
  }

  next = tbuff.lookahead(0);
  if (parse_check_and_pop(next, "{") == -1) {
    return NULL;
  }
  while (tbuff.has_next() && tbuff.lookahead(0).get_str() != "}") {
    Stmt *stmt = parse_stmt();
    stmt_list.push_back(stmt);
  }

  next = tbuff.lookahead(0);
  if (parse_check_and_pop(next, "}") == -1) {
    return NULL;
  }

  BroStmt *bro_stmt = new BroStmt(name, vibers, stmt_list);
  bro_stmt->lineno = brostmt_lineno;
  return bro_stmt;
}

IfStmt *Parser::parse_if_stmt() {
  int lineno = tbuff.get_lineno();
  ExprStmt *pred;
  StmtList then_branch;
  StmtList else_branch;
  tbuff.get_next(); // pop if

  Token next = tbuff.lookahead(0);
  if (parse_check_and_pop(next, "(") == -1) {
    return NULL;
  }

  pred = parse_exprstmt();

  next = tbuff.lookahead(0);
  if (parse_check_and_pop(next, ")") == -1) {
    return NULL;
  }

  next = tbuff.lookahead(0);
  if (parse_check_and_pop(next, "{") == -1) {
    return NULL;
  }

  while (true) {
    if (tbuff.lookahead(0).get_str() == "}" || tbuff.lookahead(0).get_type() == EMPTY) {
      break;
    }
    Stmt *stmt = parse_stmt();
    if (stmt)
      then_branch.push_back(stmt);
  }

  next = tbuff.lookahead(0);
  if (parse_check_and_pop(next, "}") == -1) {
    return NULL;
  }

  if (tbuff.lookahead(0).get_type() == ELSE) {
    tbuff.get_next(); // pop else
    next = tbuff.lookahead(0);
    if (parse_check_and_pop(next, "{") == -1) {
      return NULL;
    }

    while (true) {
      if (tbuff.lookahead(0).get_str() == "}" || tbuff.lookahead(0).get_type() == EMPTY) {
        break;
      }
      Stmt *stmt = parse_stmt();
      if (stmt) 
        else_branch.push_back(stmt);
    }

    next = tbuff.lookahead(0);
    if (parse_check_and_pop(next, "}") == -1) {
      return NULL;
    }

  }

  // IfStmt *ifstmt = new IfStmt(pred, then_list, else_list);
  IfStmt *if_stmt = new IfStmt(pred, then_branch, else_branch);
  if_stmt->lineno = lineno;
  return if_stmt;
}

WhileStmt *Parser::parse_while_stmt() {
  int lineno = tbuff.get_lineno();
  ExprStmt *pred;
  StmtList stmt_list;

  tbuff.get_next(); // pop 'while'

  Token next = tbuff.lookahead(0);
  if (parse_check_and_pop(next, "(") == -1) {
    return NULL;
  }

  pred = parse_exprstmt();


  next = tbuff.lookahead(0);
  if (parse_check_and_pop(next, ")") == -1) {
    return NULL;
  }

  next = tbuff.lookahead(0);
  if (parse_check_and_pop(next, "{") == -1) {
    return NULL;
  }

  while (true) {
    if (tbuff.lookahead(0).get_str() == "}" || tbuff.lookahead(0).get_type() == EMPTY) {
      break;
    }
    Stmt *stmt = parse_stmt();
    if (stmt)
      stmt_list.push_back(stmt);
  }

  next = tbuff.lookahead(0);
  if (parse_check_and_pop(next, "}") == -1) {
    return NULL;
  }


  WhileStmt *whilestmt = new WhileStmt(pred, stmt_list);
  whilestmt->lineno = lineno;
  return whilestmt;
}




////////////////////////////////////////////////////////////
//
//
// Expressions 
//
//
////////////////////////////////////////////////////////////
ExprStmt *Parser::parse_exprstmt() {
  debug_msg("BEGIN: parse_exprstmt()");
  int lineno = tbuff.get_lineno();
  ExprStmt *final_expr;

  Token t = tbuff.lookahead(0);
  if (t.get_type() == RETURN) {
    final_expr = parse_returnexpr();
    final_expr->lineno = lineno;
  } else {
    vector<Token> tvec;
    int lineno = tbuff.get_lineno();
    int binop_index = -1;
    int binop_prec= -1;
    int paren_stack = 0;

    int i = -1;
    while (true) {
      i++;
      Token t = tbuff.lookahead(0);
      if (t.get_type() == EMPTY || t.get_str() == ";") {
        break;
      }

      if (t.get_str() == "(") 
        paren_stack++;
      else if (t.get_str() == ")")
        paren_stack--;
      else if (paren_stack == 0 && t.get_type() == BINOP) {
        int curr_prec = get_op_precedence(t.get_str());
        if (curr_prec > binop_prec) {
          binop_index = i;
          binop_prec = curr_prec;
        }
      }
      tvec.push_back(t);
      tbuff.get_next();
    }


    if (paren_stack > 0) {
      error_blank(lineno, "Error: unbalanced parentheses.");
      panic_recover();
      return NULL;
    } 

    final_expr = parse_binopexpr(tvec, binop_index);
  }

  if (t.get_type() != RETURN) {
    if (tbuff.lookahead(0).get_str() != ";") {
      error_expected_token(tbuff.get_lineno(), ";");
      panic_recover();
      return NULL;
    }
    tbuff.get_next(); // pop ';'
  }

  debug_msg("END: parse_exprstmt()");
  return final_expr;
}

ReturnExpr *Parser::parse_returnexpr() {
  tbuff.get_next(); // pop 'return'
  ExprStmt *expr = parse_exprstmt();
  if (expr->exprtype == RETURN_EXPR) {
    string err_msg = "Error: Expression following keyword RETURN cannot be of type RETURN.";
    error_blank(expr->lineno, err_msg);
    panic_recover();
    return NULL;
  }
  ReturnExpr *retexpr = new ReturnExpr(expr);
  return retexpr;
}

std::vector<Token> Parser::disp_config_name_and_caller(vector<Token> &token_vec, 
                                                       string &calling_class, string &name) {
  vector<Token> arglist_tvec;
  if (token_vec[1].get_str() == ".") {
    calling_class = token_vec[0].get_str();
    name = token_vec[2].get_str();
    arglist_tvec.assign(token_vec.begin() + 3, token_vec.end());
  } else if (token_vec[1].get_str() == "(") {
    calling_class = "";
    name = token_vec[0].get_str();
    arglist_tvec.assign(token_vec.begin() + 1, token_vec.end());
  } else {
    error_blank(token_vec[0].get_lineno(), "Syntax error at token " + token_vec[0].get_str());
    panic_recover();
  }
  return arglist_tvec;

}

/* checks that there are '(' and ')' surrounding the arglist_tvec. then pops them. Returns 1 on
   success, and -1 on error. */
int Parser::disp_arg_check(vector<Token> &arglist_tvec) {
  if (arglist_tvec[0].get_str() != "(") {
    error_expected_token(arglist_tvec[0].get_lineno(), "(");
    panic_recover();
    return -1;
  } 
  if (arglist_tvec[arglist_tvec.size() - 1].get_str() != ")") {
    error_expected_token(arglist_tvec[0].get_lineno(), ")");
    panic_recover();
    return -1;
  }

  /* pop parenthesis around args */
  arglist_tvec.assign(arglist_tvec.begin() + 1, arglist_tvec.end() - 1);
  return 1;
}

DispatchExpr *Parser::parse_dispexpr(vector<Token> &token_vec) {
  debug_msg("BEGIN parse_dispexpr()");
  int lineno = token_vec[0].get_lineno();
  string calling_class;
  string name;
  ExprList args;

  vector<Token> arglist_tvec = disp_config_name_and_caller(token_vec, calling_class, name);
  if (disp_arg_check(arglist_tvec) == -1) {
    return NULL;
  }

  ExprStmt *arg;
  int binop_index = -1;
  int binop_prec= -1;
  int paren_stack = 0;
  int used_toks = 0;
  vector<Token> argvec;
  for (int i = 0; i < (int) arglist_tvec.size(); i++) {
    Token t = arglist_tvec[i];
    if (t.get_str() == "," || i == arglist_tvec.size() - 1) {

      if (t.get_str() == ")") {
        paren_stack--;
        argvec.push_back(arglist_tvec[i]);
      } else if (t.get_str() != ",") {
        argvec.push_back(arglist_tvec[i]);
      }
      arg = parse_binopexpr(argvec, binop_index);
      binop_index = -1;
      binop_prec = -1;
      if (paren_stack != 0) {
        error_expected_token(t.get_lineno(), ")");
        panic_recover();
        return NULL;
      }
      args.push_back(arg);
      used_toks += (int) argvec.size() + 1; // add one for the comma
      arg = NULL;
      argvec.clear();
      continue;
    }

    if (t.get_str() == "(")
      paren_stack++;
    else if (t.get_str() == ")")
      paren_stack--;
    else if (t.get_type() == BINOP && paren_stack == 0) {
      int curr_prec = get_op_precedence(t.get_str());
      if (curr_prec > binop_prec) {
        binop_index = i - used_toks;
        binop_prec = curr_prec;
      } 
    }

    argvec.push_back(arglist_tvec[i]);
  }

  DispatchExpr *dispexpr = new DispatchExpr(calling_class, name, args);
  dispexpr->lineno = lineno;
  debug_msg("END parse_dispexpr()");
  return dispexpr;

}

BinopExpr *Parser::parse_binopexpr(vector<Token> &token_vec, int index) {
  debug_msg("BEGIN parse_binopexpr()");
  int lineno;
  ExprStmt *lhs;
  string op;
  ExprStmt *rhs;
  if (token_vec.size() == 0) {
    return NULL;
  } else if (token_vec.size() == 1) {
    Token t = token_vec[0];
    lineno = t.get_lineno();
    if (t.get_type() == INT_CONST) {
      lhs = parse_int_const_expr(t);
    } else if (t.get_type() == BOOL_CONST) {
      lhs = parse_bool_const_expr(t);
    } else if (t.get_type() == VERSE_CONST) {
      lhs = parse_verse_const_expr(t);
    } else if (t.get_type() == OBJECTID) {
      lhs = parse_objectid_expr(t);
    } 
    else if (t.get_type() == THIS) {
      ThisExpr *thisexpr = new ThisExpr();
      thisexpr->lineno = lineno;
      lhs = thisexpr;
    } else if (t.get_type() == BREAK) {
      BreakExpr *breakexpr = new BreakExpr();
      breakexpr->lineno = lineno;
      lhs = breakexpr;
    } else if (t.get_type() == CONTINUE) {
      ContExpr *contexpr = new ContExpr();
      contexpr->lineno = lineno;
      lhs = contexpr;
    }
    else {
      // some error case??
      if (t.get_type() == RETURN) {
        error_blank(lineno, "Syntax error: 'return' cannot be part of binop expression");
      } else {
        error_blank(lineno, "Syntax error at token " + t.get_str());
      }
      panic_recover();
      return NULL;
    }
    op = "";
    rhs = NULL;
  } 
  // 3 * -4 is legal in krutc
  // -a + (4 * 3)
  /*
            *
          /   \ 
        3      -
              /   \  
            o     4
  */
  else if (index == -1 && token_vec[0].get_type() == OBJECTID) {
    /* if no binops in the token_vec and starts with object id */
    lhs = parse_dispexpr(token_vec);
    op = "";
    rhs = NULL;
  } else {
    if (index == -1 && token_vec[0].get_str() != "(") {
      string err_msg = "Error: expected binop type, instead got "; 
      if (token_vec[1].get_str() != "") {
        err_msg += token_vec[1].get_str();
      } else {
        err_msg += "type " + TOKEN_TYPE_TO_STRING[token_vec[1].get_type()];
      }
      error_blank(token_vec[0].get_lineno(), err_msg);
      panic_recover();
      return NULL;
    } else if (index == 0 && token_vec[0].get_str() != "-") {
      string err_msg = "Error: cannot use " + token_vec[0].get_str() + " as a unary operator, only '-'.";
      error_blank(token_vec[0].get_lineno(), err_msg);
      panic_recover();
      return NULL;
    }

    if (token_vec[0].get_str() == "(" && token_vec[token_vec.size() - 1].get_str() == ")") {
      token_vec.assign(token_vec.begin() + 1, token_vec.end() - 1);
      index = find_highest_binop_index(token_vec, 0, (int) token_vec.size());
    }
    vector<Token> lhs_vec, rhs_vec;

    lhs_vec.assign(token_vec.begin(), token_vec.begin() + index);
    int lhs_highest_index = find_highest_binop_index(lhs_vec, 0, lhs_vec.size());

    Token op_tok = token_vec[index];
    if (op_tok.get_type() != BINOP) {
      error_blank(op_tok.get_lineno(), "Error: '" + op_tok.get_str() + "' is not of type BINOP.");
      panic_recover();
      return NULL;
    }
    op = op_tok.get_str();

    rhs_vec.assign(token_vec.begin() + index + 1, token_vec.end());
    int rhs_highest_index = find_highest_binop_index(rhs_vec, 0, rhs_vec.size());

    lhs = parse_binopexpr(lhs_vec, lhs_highest_index);
    rhs = parse_binopexpr(rhs_vec, rhs_highest_index);
  }

  BinopExpr *binop = new BinopExpr(lhs, op, rhs);
  binop->lineno = lineno;
  debug_msg("END parse_binopexpr()");
  return binop;
}

/* Searches TVEC from START_ITER to END_ITER for the highest binary operator index. */
int Parser::find_highest_binop_index(vector<Token> &tvec, int start_iter, int end_iter) {
  debug_msg("BEGIN find_highest_binop_index()");
  int highest_prec = -1;
  int highest_index = -1;
  int paren_stack = 0;

  for (int i = start_iter; i < end_iter; i++) {
    Token t = tvec[i];
    if (t.get_str() == "(") 
      paren_stack++;
    else if (t.get_str() == ")")
      paren_stack--;
    else if (t.get_type() == BINOP && paren_stack == 0) {
      int curr_precedence = get_op_precedence(t.get_str());
      if (curr_precedence > highest_prec) {
        highest_prec= curr_precedence;
        highest_index = i;
      }
    }
  }
  debug_msg("END find_highest_binop_index()");
  return highest_index;
}

IntConstExpr *Parser::parse_int_const_expr(Token t) {
  int lineno = t.get_lineno();
  long val = stol(t.get_str());
  IntConstExpr *icstmt = new IntConstExpr(val);
  icstmt->lineno = lineno;
  return icstmt; 
}
BoolConstExpr *Parser::parse_bool_const_expr(Token t) {
  int lineno = t.get_lineno();
  string val = t.get_str();
  BoolConstExpr *bcstmt = new BoolConstExpr(val);
  bcstmt->lineno = lineno;
  return bcstmt;
}
VerseConstExpr *Parser::parse_verse_const_expr(Token t) {
  int lineno = t.get_lineno();
  string val = t.get_str();
  VerseConstExpr *vcstmt = new VerseConstExpr(val);
  vcstmt->lineno = lineno;
  return vcstmt; 
}

ObjectIdExpr *Parser::parse_objectid_expr(Token t) {
  ObjectIdExpr *objidexpr = new ObjectIdExpr(t.get_str());
  objidexpr->lineno = t.get_lineno();
  return objidexpr;
}


Stmt *Parser::parse_stmt() {
  Token t = tbuff.lookahead(0);
  if (t.get_type() == TYPEID) {
    return parse_feature();
  } else if (t.get_type() == FOR) {
    return parse_for_stmt();
  } else if (t.get_type() == BRO) {
    return parse_bro_stmt();
  } else if (t.get_type() == IF) {
    return parse_if_stmt();
  } else if (t.get_type() == WHILE) {
    return parse_while_stmt();
  } else {
    return parse_exprstmt();
  }
  /* this is not an error case */
  return NULL;
}


void Parser::parse_program() {
  if (tbuff.has_errors || tdump) {
    token_dump();
    return; 
  }

  while (tbuff.has_next()) {
    Stmt *stmt = parse_stmt();
    if (stmt != NULL) 
      program.add_stmt(stmt);
  }

  if (!parser_errors)
    program.dump();
}

void Parser::token_dump() {
  if (tbuff.has_errors) {
    while (tbuff.has_next()) {
      Token t = tbuff.get_next();
      if (t.get_type() == ERROR) {
        error_blank(tbuff.get_lineno(), t.get_str());
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


int main(int argc, char *argv[]) {
  if (argc < 2) {
    cerr << "Usage: " << argv[0] << " <file.kpl>" << endl;
    return -1;
  }
  string filename = argv[1];
  bool token_dump = false;
  bool debug = false;
  if (argc >= 3) {
    string flag = argv[2];
    if (strncmp(flag.c_str(), "-d", 2) == 0) {
      debug = true;
    } else if (strncmp(flag.c_str(), "-t", 2) == 0) {
      token_dump = true;
    } else {
      /* flag -t found */
      cerr << "Error: Unknown flag " + flag << endl;
      cerr << "Expected flag '-t' or '-d'" << endl;
      return -1;
    }
  }

  Parser parser = Parser(filename, token_dump, debug);
  parser.parse_program();

  return 1;

}