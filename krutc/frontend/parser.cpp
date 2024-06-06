#include <iostream>
#include <cassert>

#include "include/parser.h"



using namespace std;


void Parser::panic_recover(string s) {
  while (true) {
    Token t = tbuff.lookahead(0);
    if (t.get_str() == s || t.get_type() == EMPTY) {
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
    string type;
    string name;
    Token next = tbuff.lookahead(0);
    int lineno = next.get_lineno();
    if (next.get_type() != TYPEID) {
      error_blank(lineno, "Error: Expected TYPEID in formal definition.");
      if (next.get_str() == ")") {
        break;
      }
    } else {
      type = next.get_str();      
    }
    tbuff.get_next();

    next = tbuff.lookahead(0);
    if (next.get_type() != OBJECTID) {
      error_blank(lineno, "Error: Expected OBJECTID to follow TYPEID in formal definition.");
      if (next.get_str() == ")") {
        break;
      }
    } else {
      name = next.get_str();
    }
    tbuff.get_next();

    FormalStmt *f = new FormalStmt(type, name);
    f->lineno = lineno;
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
MethodStmt *Parser::parse_methodstmt() {
  debug_msg("BEGIN parse_method_stmt()");
  int methodstmt_lineno = tbuff.get_lineno();
  string type;
  string name;
  FormalList formal_list;  
  StmtList stmt_list;

  Token type_tok = tbuff.get_next();
  assert(type_tok.get_type() == TYPEID);
  type = type_tok.get_str();

  if (tbuff.lookahead(0).get_str() == "(") {
    name = "constructor";
  } else {
    Token next = tbuff.get_next();
    if (next.get_type() != OBJECTID) {
      error_blank(next.get_lineno(), "Error: expected an identifier.");
    }
    name = next.get_str();
  }

  parse_check_and_pop("("); // pop leading "("

  formal_list = parse_formallist();
  // doing_formals = false;
  parse_check_and_pop(")"); // pop closing ')'

  parse_check_and_pop("{"); // parse opening {

  while (tbuff.lookahead(0).get_str() != "}") {
    Stmt *stmt = parse_stmt();
    stmt_list.push_back(stmt);
  }

  parse_check_and_pop("}");

  MethodStmt *methodstmt = new MethodStmt(name, type, formal_list, stmt_list);
  methodstmt->lineno = methodstmt_lineno; 
  debug_msg("END parse_method_stmt()");

  return methodstmt; 
}



AttrStmt *Parser::parse_attrstmt() {
  debug_msg("BEGIN parse_attrstmt()");
  int attrstmt_lineno = tbuff.get_lineno();
  string type;
  string name;
  ExprStmt *init;

  Token type_tok = tbuff.get_next();
  assert(type_tok.get_type() == TYPEID);
  type = type_tok.get_str();

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
  } else if (decider_tok.get_str() == "="){ 
    tbuff.get_next(); // pop "="
    build_expr_tq(";");
    init = parse_exprstmt();
    parse_check_and_pop(";");
  } else {
    /* this should never hit because its checked in parse_features() */
    error_blank(decider_tok.get_lineno(), "Error: Expected either ';' or expression following attribute declaration.");
  }

  AttrStmt *attrstmt = new AttrStmt(name, type, init);
  attrstmt->lineno = attrstmt_lineno;
  debug_msg("END parse_attrstmt()");
  return attrstmt;
}


Feature *Parser::parse_feature() {
  // debug_msg("BEGIN parse_feature()");
  Token tdecider = tbuff.lookahead(2); // 0 - type, 1 - name, 2 - = or (
  if (tdecider.get_str() == "=" || tdecider.get_str() == ";") {
    return parse_attrstmt();
  } else if (tdecider.get_str() == "(") {
    return parse_methodstmt();
  } else if (tbuff.lookahead(1).get_str() == "(") {
    return parse_methodstmt();
  }
  
  else {
    string err_msg = "Syntax error at token " + tdecider.get_str() + ". Expected one of ';', '=', '('";
    error_blank(tdecider.get_lineno(), err_msg);
    return NULL;
  }
  // debug_msg("BEGIN parse_feature()");

}


int Parser::parse_check_and_pop(string s) {
  Token t = tbuff.lookahead(0);
  if (t.get_str() != s) {
    error_blank(tbuff.get_lineno(), "Error: Expected '" + s + "', instead got '" + t.get_str() + "'.");
    panic_recover(s);
    tbuff.get_next(); // pop t
    return -1;
  }
  tbuff.get_next(); // pop t
  return 1;
}



ForStmt *Parser::parse_for_stmt() {
  debug_msg("BEGIN parse_for_stmt()");
  int forstmt_lineno = tbuff.get_lineno();
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
    if (next.get_type() == TYPEID) {
      stmt = parse_attrstmt();
    } else {
      build_expr_tq(";");
      stmt = parse_exprstmt();
      parse_check_and_pop(";"); // pop ';'
    }
  }

  if (tbuff.lookahead(0).get_str() != ";") {
    build_expr_tq(";");
    cond = parse_exprstmt(); 
  } else {
    cond = NULL;
  }
  parse_check_and_pop(";"); // pop ';'

  if (tbuff.lookahead(0).get_str() != ")") {
    build_expr_tq(")");
    repeat = parse_exprstmt(); 
  } else {
    repeat = NULL;
  }
  parse_check_and_pop(")"); // pop ')'

  parse_check_and_pop("{");

  while (tbuff.has_next() && tbuff.lookahead(0).get_str() != "}") {
    Stmt *stmt = parse_stmt();
    stmt_list.push_back(stmt);
  }

  parse_check_and_pop("}");

  ForStmt *for_stmt = new ForStmt(stmt, cond, repeat, stmt_list);
  for_stmt->lineno = forstmt_lineno;
  debug_msg("END parse_for_stmt()");
  return for_stmt;
}

vector<string> Parser::get_vibers() {
  tbuff.get_next(); // pop keyword VIBESWITH

  vector<string> vibers;
  while (tbuff.has_next() && tbuff.lookahead(0).get_str() != "{") {
    Token viber = tbuff.get_next();
    if (viber.get_type() != TYPEID) {
      string err_msg = "Error: VIBER cannot be of type " + viber.get_type_str();
      error_blank(viber.get_lineno(), err_msg);
      // panic_recover();
      /* dont need to return now because we may be able to recover */
    } else {
      vibers.push_back(tbuff.get_next().get_str());
    }

    Token next = tbuff.lookahead(0);
    if (next.get_str() == "{")
      break;
    if (next.get_str() == ",") {
      tbuff.get_next();
      continue;
    } else {
      string err_msg = "Error: Expected ',' or '{' after VIBER.";
      error_blank(tbuff.get_lineno(), err_msg);
      return vibers;
    }
  }
  return vibers;
}

/* BRO TYPEID [VIBESWITH [OBJECTID[, OBJECTID]*]]? { STMTLIST }; */
BroStmt *Parser::parse_bro_stmt() {
  debug_msg("BEGIN parse_bro_stmt()");
  int brostmt_lineno = tbuff.get_lineno();
  string name;
  vector<string> vibers;
  StmtList stmt_list;

  Token bro_tok = tbuff.lookahead(0);
  assert(bro_tok.get_type() == BRO);
  tbuff.get_next(); // pop BRO

  Token next = tbuff.lookahead(0);
  if (next.get_type() != TYPEID) {
    string err_msg = "Error: BRO name must start with a capital letter.";
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

  parse_check_and_pop("{");

  while (tbuff.has_next() && tbuff.lookahead(0).get_str() != "}") {
    Stmt *stmt = parse_stmt();
    stmt_list.push_back(stmt);
  }

  parse_check_and_pop("}");

  BroStmt *bro_stmt = new BroStmt(name, vibers, stmt_list);
  bro_stmt->lineno = brostmt_lineno;
  debug_msg("END parse_bro_stmt()");
  return bro_stmt;
}

IfStmt *Parser::parse_if_stmt() {
  debug_msg("BEGIN parse_if_stmt()");
  int lineno = tbuff.get_lineno();
  ExprStmt *pred;
  StmtList then_branch;
  StmtList else_branch;
  tbuff.get_next(); // pop if

  /* evaluate expression */
  parse_check_and_pop("(");
  build_expr_tq(")");
  pred = parse_exprstmt();
  parse_check_and_pop(")");

  parse_check_and_pop("{");

  while (true) {
    if (tbuff.lookahead(0).get_str() == "}" || tbuff.lookahead(0).get_type() == EMPTY) {
      break;
    }
    Stmt *stmt = parse_stmt();
    if (stmt)
      then_branch.push_back(stmt);
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
      if (stmt) 
        else_branch.push_back(stmt);
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
  int lineno = tbuff.get_lineno();
  ExprStmt *pred;
  StmtList stmt_list;

  tbuff.get_next(); // pop 'while'

  /* evaluate expression */
  parse_check_and_pop("(");
  build_expr_tq(")");
  pred = parse_exprstmt();
  parse_check_and_pop(")");

  parse_check_and_pop("{");

  while (true) {
    if (tbuff.lookahead(0).get_str() == "}" || tbuff.lookahead(0).get_type() == EMPTY) {
      break;
    }
    Stmt *stmt = parse_stmt();
    if (stmt)
      stmt_list.push_back(stmt);
  }

  parse_check_and_pop("}");

  WhileStmt *whilestmt = new WhileStmt(pred, stmt_list);
  whilestmt->lineno = lineno;
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
int Parser::calibrate_expr_tq() {
  int lineno = expr_tq.tq.front().get_lineno();
  int binop_index = -1;
  int binop_prec = -1;
  int paren_stack = 0;

  for (int i = 0; i < (int) expr_tq.tq.size(); i++) {
    Token t = expr_tq.tq[i];
    if (t.get_str() == "(") 
      paren_stack++;
    else if (t.get_str() == ")")
      paren_stack--;
    else if (t.get_type() == BINOP && paren_stack == 0) {
      int curr_precedence = get_op_precedence(t.get_str());
      if (curr_precedence > binop_prec) {
        binop_prec = curr_precedence;
        binop_index = i;
      }
    }
  }
  expr_tq.lineno = lineno;
  expr_tq.binop_index = binop_index;
  expr_tq.binop_prec = binop_prec;
  expr_tq.paren_stack = paren_stack;

  if (expr_tq.paren_stack > 0) {
    error_blank(lineno, "Error: unbalanced parentheses.");
    // panic_recover();
    return 0;
  }

  return 1;
}

/* builds up an expression token deque which stops after finding 'S'*/
bool Parser::build_expr_tq(string s) {
  // deque<Token> tvec;
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

    if (t.get_str() == s) {
      if (s != ")") 
        break;
      else if (paren_stack == 0)
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
    expr_tq.tq.push_back(t);
    tbuff.get_next();
  }

  expr_tq.lineno = lineno;
  expr_tq.binop_index = binop_index;
  expr_tq.binop_prec = binop_prec;
  expr_tq.paren_stack = paren_stack;

  if (expr_tq.paren_stack > 0) {
    error_blank(lineno, "Error: unbalanced parentheses.");
    // panic_recover();
    return false;
  }
  return true;
}


ExprStmt *Parser::parse_exprstmt() {
  debug_msg("BEGIN: parse_exprstmt()");

  if (!expr_tq.tq.empty()) {
    if (!calibrate_expr_tq()) {
      return NULL;
    }
  }

  if (expr_tq.tq.size() == 0) {
    /* does this ever happen? */
    return NULL;
  }

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
  } else if (t.get_type() == VERSE_CONST) {
    expr = parse_verse_const_expr();
  }  else if (t.get_type() == CONTINUE) {
    expr = new ContExpr();
    expr_tq.tq.pop_front();
  } else if (t.get_type() == BREAK) {
    expr = new BreakExpr();
    expr_tq.tq.pop_front();
  } else if (t.get_type() == THIS) {
    expr = new ThisExpr();
    expr_tq.tq.pop_front();
  } else if (t.get_type() == OBJECTID && expr_tq.tq.size() == 1) {
    expr = parse_objectid_expr();
  } else if (t.get_type() == TYPEID || t.get_type() == OBJECTID) {
    expr = parse_dispexpr();
  } else if (t.get_str() == "(" && expr_tq.tq.back().get_str() == ")") {
    expr_tq.tq.pop_front();
    expr_tq.tq.pop_back();
    expr = parse_exprstmt();
  } else {
    error_blank(lineno, "Error: unrecognized expression.");
    expr_tq.dump();
    // panic_recover();
    return NULL;
  }

  if (!expr_tq.tq.empty()) {
    error_blank(lineno, "Error, unkown expression.");
    expr_tq.dump();
    // panic_recover();
    return NULL;
  }
  
  expr->lineno = lineno; 
  debug_msg("END: parse_exprstmt()");
  return expr;
}

ReturnExpr *Parser::parse_returnexpr() {
  debug_msg("BEGIN parse_returnexpr()");
  int lineno = expr_tq.tq[0].get_lineno();
  ExprStmt *expr;
  expr_tq.tq.pop_front();
  calibrate_expr_tq();
  if (expr_tq.tq.empty()) {
    /* let the caller of expr handle ';' */
    expr = NULL;
  } else {
    expr = parse_exprstmt();
    // if (expr->exprtype == RETURN_EXPR) {
    //   string err_msg = "Error: Expression following keyword RETURN cannot be of type RETURN.";
    //   error_blank(expr->lineno, err_msg);
    //   panic_recover(";");
    // }
  }
  ReturnExpr *retexpr = new ReturnExpr(expr);
  retexpr->lineno = lineno;
  debug_msg("END parse_returnexpr()");
  return retexpr;
}

KillExpr *Parser::parse_killexpr() {
  debug_msg("BEGIN parse_killexpr()");
  int lineno = expr_tq.tq[0].get_lineno();
  ExprStmt *expr;
  expr_tq.tq.pop_front(); // pop 'kill'
  calibrate_expr_tq();
  if (expr_tq.tq.empty()) {
    expr = NULL;
  } 
  // else if (expr_tq.tq[0].get_type() != VERSE_CONST) {
  //   string err_msg = "Error: Expression following keyword KILL must be of type VERSE_CONST";
  //   error_blank(lineno, err_msg);
  //   panic_recover();
  //   return NULL;
  // } 
  // else if (expr_tq.tq.size() > 1) {
  //   error_blank(lineno, "Error: cannot have expression after KILL.");
  //   expr_tq.tq.clear();
  // } 
  else {
    /* collect error message */
    // err = expr_tq.tq.front().get_str();
    // expr_tq.tq.pop_front(); // pop err string
    expr = parse_exprstmt();
  }
  KillExpr *killexpr= new KillExpr(expr);
  killexpr->lineno = lineno;
  debug_msg("END parse_killexpr()");
  return killexpr;
}

NewExpr *Parser::parse_newexpr() {
  debug_msg("BEGIN parse_newexpr()");
  int lineno = expr_tq.tq[0].get_lineno();
  expr_tq.tq.pop_front();
  if (expr_tq.tq.empty()) {
    string err_msg = "Error: There must be an expression following keyword NEW.";
    error_blank(lineno, err_msg);
    panic_recover(";");
    return NULL;
  }
  build_expr_tq(";");
  ExprStmt *expr = parse_exprstmt();
  // if (expr->exprtype == NEW_EXPR || expr->exprtype == RETURN_EXPR) {
  //   string err_msg = "Error: Expression following keyword NEW cannot be of type " + expr->classname();
  //   error_blank(expr->lineno, err_msg);
  //   panic_recover();
  //   return NULL;
  // }   
  NewExpr *newexpr = new NewExpr(expr);
  newexpr->lineno = lineno;
  debug_msg("END parse_newexpr()");
  return newexpr;
}

// bool Parser::expr_tvec_size_check(int n) {
//   if (expr_tq.tq.size() < n) {
//     error_blank(expr_tq.tq[0].get_lineno(), "Error: unknown dipsatch call");
//     panic_recover();
//     expr_tq.dump();
//     return false;
//   }
//   return true;

// }

DispatchExpr *Parser::parse_dispexpr() {
  debug_msg("BEGIN parse_dispexpr()");
  int lineno = expr_tq.tq.front().get_lineno();
  ExprStmt *calling_class;
  string name;
  ExprList args;


  



  DispatchExpr *disp_expr = new DispatchExpr(calling_class, name, args);
  disp_expr->lineno = lineno;
  return disp_expr;
}

BinopExpr *Parser::parse_binopexpr() {
  debug_msg("BEGIN parse_binopexpr()");
  int lineno = expr_tq.tq[expr_tq.binop_index].get_lineno();
  ExprStmt *lhs;
  string op;
  ExprStmt *rhs;

  // if (expr_tq.binop_index == 0 && expr_tq.tq.front().get_str() != "-") {
  //   /* if there is a binary operator at the beginning but its not a '-' */
  //   string err_msg = "Error: cannot use " + expr_tq.tq.front().get_str() + " as a unary operator, only '-'.";
  //   error_blank(expr_tq.lineno, err_msg);
  //   panic_recover();
  //   return NULL;
  // }

  /* this will never hit in here because if there are () then binop_index = -1 */
  // if (expr_tq.tq.front().get_str() == "(" && expr_tq.tq.back().get_str() == ")") {
  //   /* if parentheses surround, remove them and recalibrate */
  //   expr_tq.tq.pop_front();
  //   expr_tq.tq.pop_back();
  //   if (!calibrate_expr_tq())
  //     return NULL;
  // }

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
  binop->lineno = lineno;
  debug_msg("END parse_binopexpr()");
  return binop;
}

IntConstExpr *Parser::parse_int_const_expr() {
  Token t = expr_tq.tq.front();
  expr_tq.tq.pop_front();
  int lineno = t.get_lineno();
  long val = stol(t.get_str());
  IntConstExpr *icstmt = new IntConstExpr(val);
  icstmt->lineno = lineno;
  return icstmt; 
}
BoolConstExpr *Parser::parse_bool_const_expr() {
  Token t = expr_tq.tq.front();
  expr_tq.tq.pop_front();
  int lineno = t.get_lineno();
  string val = t.get_str();
  BoolConstExpr *bcstmt = new BoolConstExpr(val);
  bcstmt->lineno = lineno;
  return bcstmt;
}
VerseConstExpr *Parser::parse_verse_const_expr() {
  Token t = expr_tq.tq.front();
  expr_tq.tq.pop_front();
  int lineno = t.get_lineno();
  string val = t.get_str();
  VerseConstExpr *vcstmt = new VerseConstExpr(val);
  vcstmt->lineno = lineno;
  return vcstmt; 
}

ObjectIdExpr *Parser::parse_objectid_expr() {
  debug_msg("BEGIN parse_objectid_expr()");
  Token t = expr_tq.tq.front();
  expr_tq.tq.pop_front();
  ObjectIdExpr *objidexpr = new ObjectIdExpr(t.get_str());
  objidexpr->lineno = t.get_lineno();
  debug_msg("END parse_objectid_expr()");
  return objidexpr;
}


Stmt *Parser::parse_stmt() {
  debug_msg("BEGIN parse_stmt()");
  Stmt *stmt;
  Token t = tbuff.lookahead(0);
  if (t.get_type() == TYPEID) {
    stmt = parse_feature();
  } else if (t.get_type() == FOR) {
    stmt = parse_for_stmt();
  } else if (t.get_type() == BRO) {
    stmt = parse_bro_stmt();
  } else if (t.get_type() == IF) {
    stmt = parse_if_stmt();
  } else if (t.get_type() == WHILE) {
    stmt = parse_while_stmt();
  } else {
    build_expr_tq(";");
    stmt = parse_exprstmt();
    if (parse_check_and_pop(";") == -1) {
      return NULL;
    }
  }
  debug_msg("END parse_stmt()");
  return stmt;
}


void Parser::parse_program() {
  if (tbuff.has_errors) {
    token_dump();
    return; 
  }

  while (tbuff.has_next()) {
    Stmt *stmt = parse_stmt();
    if (stmt != NULL) 
      program.add_stmt(stmt);
  }

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
    cerr << "Usage: " << argv[0] << " <file.krut>" << endl;
    return -1;
  }
  string filename = argv[1];
  string suffix = ".krut";

  if (filename.compare(filename.size() - suffix.size(), suffix.size(), suffix) != 0) {
    cerr << "Error: file must be of extension " + suffix << endl;
    return -1;
  } 

  bool token_dump = false;
  bool debug = false;
  bool tree = false;
  if (argc >= 3) {
    for (int i = 2; i < argc; i++) {
      string flag = argv[i];
      if (strncmp(flag.c_str(), "-debug", 6) == 0) {
        debug = true;
      } else if (strncmp(flag.c_str(), "-tdump", 6) == 0) {
        token_dump = true;
      } else if (strncmp(flag.c_str(), "-tree", 5) == 0) {
        tree = true;
      }
      else {
        cerr << "Error: Unknown flag " + flag << endl;
        cerr << "Expected flag '-tdump', '-debug', or '-tree'." << endl;
      }

    }
  }

  Parser parser = Parser(filename, debug);

  if (token_dump) {
    parser.token_dump();
  } else {
    parser.parse_program();
    if (tree && !parser.parser_errors) {
      parser.program.dump();      
    }
  } 

  return 1;

}