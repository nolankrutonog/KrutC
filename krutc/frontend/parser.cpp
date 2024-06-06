#include <iostream>

#include "include/parser.h"



using namespace std;


void Parser::panic_recover() {
  // while (true) {
  //   if (tbuff.lookahead(0).get_type() == EMPTY || tbuff.lookahead(0).get_str() == ";") {
  //     // tbuff.get_next(); // pop ';'
  //     break;
  //   }
  //   tbuff.get_next();
  // }
  
}


////////////////////////////////////////////////////////////
//
//
// Statements
//
//
////////////////////////////////////////////////////////////
FormalStmt *Parser::parse_formalstmt() {
  debug_msg("BEGIN parse_formal_stmt()");
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
  debug_msg("END parse_formal_stmt()");
  return formal;
}

/* type name(FormalList) {StmtList};*/
MethodStmt *Parser::parse_methodstmt() {
  debug_msg("BEGIN parse_method_stmt()");
  int methodstmt_lineno = tbuff.get_lineno();
  string type;
  string name;
  FormalList formal_list;  
  StmtList stmt_list;

  type = tbuff.get_next().get_str();
  if (tbuff.lookahead(0).get_str() == "(") {
    name = "constructor";
  } else {
    Token next = tbuff.lookahead(0);
    if (parse_check_and_pop_type(next, OBJECTID) == -1) {
      return NULL;
    }
    name = next.get_str();
  }

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
  debug_msg("END parse_method_stmt()");

  return methodstmt; 
}



AttrStmt *Parser::parse_attrstmt() {
  debug_msg("BEGIN parse_attrstmt()");
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
    build_expr_tq(";");
    init = parse_exprstmt();
    if (parse_check_and_pop(";") == -1) {
      return NULL;
    }
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
    panic_recover();
    return NULL;
  }
  // debug_msg("BEGIN parse_feature()");

}


int Parser::parse_check_and_pop(string s) {
  Token t = tbuff.lookahead(0);
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
  debug_msg("BEGIN parse_for_stmt()");
  int forstmt_lineno = tbuff.get_lineno();
  FormalStmt *formal;
  ExprStmt *start;
  ExprStmt *end;
  ExprStmt *step;
  StmtList stmt_list;

  tbuff.get_next().get_str(); // pop 'FOR'

  if (parse_check_and_pop("(") == -1)
    return NULL;

  if (tbuff.lookahead(0).get_type() == TYPEID) {
    formal = parse_formalstmt(); /* can be separated by either , and ; */ 
  } else {
    formal = NULL;
  }

  if (parse_check_and_pop(";") == -1) {
    return NULL;
  }

  if (tbuff.lookahead(0).get_str() != ";") {
    build_expr_tq(";");
    start = parse_exprstmt(); 
  } else {
    start = NULL;
  }
  tbuff.get_next(); // pop ';'

  if (tbuff.lookahead(0).get_str() != ";") {
    build_expr_tq(";");
    end = parse_exprstmt(); 
  } else {
    end = NULL;
  }
  tbuff.get_next(); // pop ';'

  if (tbuff.lookahead(0).get_str() != ")") {
    build_expr_tq(")");
    step = parse_exprstmt(); 
  } else {
    step = NULL;
  }
  tbuff.get_next(); // pop ')'


  if (parse_check_and_pop("{") == -1) {
    return NULL;
  }

  while (tbuff.has_next() && tbuff.lookahead(0).get_str() != "}") {
    Stmt *stmt = parse_stmt();
    stmt_list.push_back(stmt);
  }

  if (parse_check_and_pop("}") == -1) {
    return NULL;
  }


  ForStmt *for_stmt = new ForStmt(formal, start, end, step, stmt_list);
  for_stmt->lineno = forstmt_lineno;
  debug_msg("END parse_for_stmt()");
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
  debug_msg("BEGIN parse_bro_stmt()");
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

  if (parse_check_and_pop("{") == -1) {
    return NULL;
  }
  while (tbuff.has_next() && tbuff.lookahead(0).get_str() != "}") {
    Stmt *stmt = parse_stmt();
    stmt_list.push_back(stmt);
  }

  if (parse_check_and_pop("}") == -1) {
    return NULL;
  }

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

  if (parse_check_and_pop("(") == -1) {
    return NULL;
  }

  build_expr_tq(")");
  pred = parse_exprstmt();

  if (parse_check_and_pop(")") == -1) {
    return NULL;
  }

  if (parse_check_and_pop("{") == -1) {
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

  if (parse_check_and_pop("}") == -1) {
    return NULL;
  }

  if (tbuff.lookahead(0).get_type() == ELSE) {
    tbuff.get_next(); // pop else
    if (parse_check_and_pop("{") == -1) {
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

    if (parse_check_and_pop("}") == -1) {
      return NULL;
    }

  }

  // IfStmt *ifstmt = new IfStmt(pred, then_list, else_list);
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

  if (parse_check_and_pop("(") == -1) {
    return NULL;
  }

  build_expr_tq(";");
  pred = parse_exprstmt();
  if (parse_check_and_pop(";") == -1) {
    return NULL;
  }

  if (parse_check_and_pop(")") == -1) {
    return NULL;
  }

  if (parse_check_and_pop("{") == -1) {
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

  if (parse_check_and_pop("}") == -1) {
    return NULL;
  }


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
    panic_recover();
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
    if (t.get_type() == EMPTY) { 
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
    panic_recover();
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
    panic_recover();
    return NULL;
  }

  if (!expr_tq.tq.empty()) {
    error_blank(lineno, "Error, unkown expression.");
    expr_tq.dump();
    panic_recover();
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
  if (expr_tq.tq.empty()) {
    /* let the caller of expr handle ';' */
    expr = NULL;
  } else {
    build_expr_tq(";");
    expr = parse_exprstmt();
    if (expr->exprtype == RETURN_EXPR) {
      string err_msg = "Error: Expression following keyword RETURN cannot be of type RETURN.";
      error_blank(expr->lineno, err_msg);
      panic_recover();
      return NULL;
    }
  }
  ReturnExpr *retexpr = new ReturnExpr(expr);
  retexpr->lineno = lineno;
  debug_msg("END parse_returnexpr()");
  return retexpr;
}

KillExpr *Parser::parse_killexpr() {
  debug_msg("BEGIN parse_killexpr()");
  int lineno = expr_tq.tq[0].get_lineno();
  string err = "";
  expr_tq.tq.pop_front(); // pop 'kill'
  if (expr_tq.tq.empty()) {
    err = "Generic Error";
  } 
  // else if (expr_tq.tq[0].get_type() != VERSE_CONST) {
  //   string err_msg = "Error: Expression following keyword KILL must be of type VERSE_CONST";
  //   error_blank(lineno, err_msg);
  //   panic_recover();
  //   return NULL;
  // } 
  else if (expr_tq.tq.size() > 1) {
    error_blank(lineno, "Error: cannot have expression after KILL.");
    panic_recover();
    return NULL;
  } else {
    /* collect error message */
    err = expr_tq.tq.front().get_str();
    expr_tq.tq.pop_front(); // pop err string
  }
  KillExpr *killexpr= new KillExpr(err);
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
    panic_recover();
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

bool Parser::expr_tvec_size_check(int n) {
  if (expr_tq.tq.size() < n) {
    error_blank(expr_tq.tq[0].get_lineno(), "Error: unknown dipsatch call");
    panic_recover();
    expr_tq.dump();
    return false;
  }
  return true;

}

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

  if (expr_tq.binop_index == 0 && expr_tq.tq.front().get_str() != "-") {
    /* if there is a binary operator at the beginning but its not a '-' */
    string err_msg = "Error: cannot use " + expr_tq.tq.front().get_str() + " as a unary operator, only '-'.";
    error_blank(expr_tq.lineno, err_msg);
    panic_recover();
    return NULL;
  }

  /* this will never hit in here because if there are () then binop_index = -1 */
  // if (expr_tq.tq.front().get_str() == "(" && expr_tq.tq.back().get_str() == ")") {
  //   /* if parentheses surround, remove them and recalibrate */
  //   expr_tq.tq.pop_front();
  //   expr_tq.tq.pop_back();
  //   if (!calibrate_expr_tq())
  //     return NULL;
  // }

  Token op_tok = expr_tq.tq[expr_tq.binop_index];
  if (op_tok.get_type() != BINOP) {
    error_blank(op_tok.get_lineno(), "Error: '" + op_tok.get_str() + "' is not of type BINOP.");
    panic_recover();
    return NULL;
  }
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