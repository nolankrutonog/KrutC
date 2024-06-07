#include <iostream>
#include <vector>



enum StmtType {
  STMT = 500,
  BRO_STMT = 501,
  ATTR_STMT = 502,
  METHOD_STMT = 503,
  FORMAL_STMT = 504,
  FOR_STMT = 505,
  IF_STMT = 506,
  WHILE_STMT = 507,
  // TYPEID_STMT = 514,
  // DISPATCH_STMT = 517,
};

enum ExprType {
  EXPR_EXPR = 600,
  RETURN_EXPR = 601,
  INT_CONST_EXPR = 602,
  VERSE_CONST_EXPR = 603,
  LIST_CONST_EXPR = 604,
  OBJECTID_EXPR = 605,
  DISPATCH_EXPR = 606,
  OP_EXPR = 607,
  THIS_EXPR = 608,
  BOOL_CONST_EXPR = 609,
  BINOP_EXPR = 610,
  CONT_EXPR = 611,
  BREAK_EXPR = 612,
  NONE_EXPR = 613,
  KILL_EXPR = 614,
  NEW_EXPR = 615,
  LIST_ELEM_REF = 616
};

class Program;
class Stmt;
typedef class std::vector<Stmt*> StmtList; /* Stmtessions is a vector of Stmt ptrs */

class BroStmt;
class Feature;
class AttrStmt;
class MethodStmt;
class FormalStmt;
typedef class std::vector<FormalStmt*> FormalList;
class ForStmt;
class IfStmt;
class WhileStmt;

class ExprStmt;
typedef class std::vector<ExprStmt*> ExprList;
class ReturnExpr;
class IntConstExpr;
class VerseConstExpr;
class BoolConstExpr;
class ListConstExpr;
class ObjectIdExpr;
class DispatchExpr;
class BinopExpr;
class ThisExpr; 
class ContExpr;
class BreakExpr;
class NoneExpr;
class KillExpr;



/*
  tree.h
  Defines the KrutC language. A program is represented as a list (vector) of stmts.
  Every stmt can be broken up into smaller parts. 
*/

class Stmt {
public:
  StmtType stmttype = STMT;
  int lineno = 0;
  virtual ~Stmt() = default;
  virtual void dump(int indent) = 0;
  virtual std::string classname() { return "Stmt"; };
};

class Program {
  StmtList stmt_list;
public:
  Program() {}
  void add_stmt(Stmt *stmt) { stmt_list.push_back(stmt); } 
  void dump(); 
};

class ExprStmt : public Stmt {
public:
  int lineno = 0;
  ExprType exprtype = EXPR_EXPR;
  virtual std::string classname() { return "ExprStmt";}
  virtual void dump(int indent);

  std::string get_name() { return "ExprStmt"; }
};


////////////////////////////////////////////////////////////
//
//
// Statements
//
//
////////////////////////////////////////////////////////////
class BroStmt: public Stmt {
  std::string name;
  std::vector<std::string> vibers;
  StmtList stmt_list;

public:
  BroStmt(std::string name, std::vector<std::string> vibers, StmtList stmt_list):
    name(name), vibers(vibers), stmt_list(stmt_list) {}
  StmtType stmttype = BRO_STMT; 
  std::string classname() { return "BroStmt"; } 
  void dump(int indent); 

  std::string get_name() { return name; } 
  std::vector<std::string> get_vibers() { return vibers; } 
  StmtList get_stmt_list() { return stmt_list; } 

};


class Feature : public Stmt {
public:
  virtual bool is_method() = 0;
};

/* in the form TYPEID OBJECTID = EXPRSTMT; */
class AttrStmt : public Feature {
  std::string name;
  std::string type;
  ExprStmt *init;
public:
  AttrStmt(std::string name, std::string type, ExprStmt *init) : 
    name(name), type(type), init(init) {}
  StmtType stmttype = ATTR_STMT;
  void dump(int indent);
  bool is_method() { return false; } 
  std::string get_name() { return name; } 
  std::string classname()  { return "AttrStmt"; } 
  std::string get_type() { return type; } 
  ExprStmt *get_init() { return init; }  

};

class FormalStmt : public Stmt {
  std::string type;
  std::string name;

public:
  FormalStmt(std::string type, std::string name) : type(type), name(name) {}
  StmtType stmttype = FORMAL_STMT;
  void dump(int indent);
  std::string classname()  { return "FORMAL_STMT"; } 
  std::string get_name() { return name; } 
  std::string get_type() { return type; } 
};



/* in the form TYPEID OBJECTID(FORMAL_LIST) {STMT_LIST}*/
class MethodStmt : public Feature {
  std::string name;
  std::string ret_type;
  FormalList formal_list;
  StmtList stmt_list;
public:
  MethodStmt(std::string name, std::string ret_type, FormalList formal_list, StmtList stmt):
    name(name), ret_type(ret_type), formal_list(formal_list), stmt_list(stmt) {}
  StmtType stmttype = METHOD_STMT;
  std::string classname() { return "MethodStmt"; } 
  void dump(int indent);


  bool is_method() { return true; } 
  std::string get_name() { return name; } 
  std::string get_ret_type() { return ret_type; } 
  FormalList get_formal_list() { return formal_list; } 
  StmtList get_stmt_list() { return stmt_list; } 

};

/* in the form FOR (OBJECTID, START=START_EXPR, END=END_EXPR, STEP=STEP_EXPR) {STMT_LIST}; */
class ForStmt : public Stmt {
  Stmt *stmt;
  // ExprStmt *start;
  ExprStmt *cond;
  ExprStmt *repeat;
  StmtList stmt_list;

public:
  ForStmt(Stmt *stmt, /* ExprStmt *start,*/ ExprStmt *cond, 
          ExprStmt *repeat, StmtList stmt_list)
    : stmt(stmt), /* start(start),*/ cond(cond), 
      repeat(repeat), stmt_list(stmt_list) {}
  StmtType stmttype = FOR_STMT;
  void dump(int indent);
  std::string classname() { return "ForStmt"; }

  Stmt *get_formal() { return stmt; }
  // ExprStmt *get_start() { return start; }
  ExprStmt *get_cond() { return cond; }
  ExprStmt *get_repeat() { return repeat; }
  StmtList get_stmt_list() { return stmt_list; }
};


class IfStmt : public Stmt {
  ExprStmt *pred;
  StmtList then_branch;
  StmtList else_branch;

public:
  IfStmt(ExprStmt *pred, StmtList then_branch, StmtList else_branch) :
    pred(pred), then_branch(then_branch), else_branch(else_branch) {}
  StmtType stmttype = IF_STMT;
  void dump(int indent);
  std::string classname() { return "IfStmt";}
  std::string get_name() { return "IfStmt"; }

  ExprStmt *get_pred() { return pred; }
  StmtList get_then() { return then_branch; }
  StmtList get_else() { return else_branch; }
};

class WhileStmt : public Stmt {
  ExprStmt *pred;
  StmtList stmt_list;
public: 
  WhileStmt(ExprStmt *pred, StmtList stmt_list) : pred(pred), stmt_list(stmt_list) {}
  StmtType stmttype = WHILE_STMT;
  void dump(int indent);
  std::string classname() { return "WhileStmt";}
  std::string get_name() { return "WhileStmt"; }

  ExprStmt *get_pred() { return pred; }
  StmtList get_stmt_list() { return stmt_list; }
};


////////////////////////////////////////////////////////////
//
//
// Expressions
//
//
////////////////////////////////////////////////////////////

class BinopExpr : public ExprStmt {
  ExprStmt *lhs;
  std::string op;
  ExprStmt *rhs;
public:
  BinopExpr(ExprStmt *lhs, std::string op, ExprStmt *rhs)
    : lhs(lhs), op(op), rhs(rhs) {}
  ExprType exprtype = BINOP_EXPR;
  std::string classname() { return "BinopExpr"; }
  void dump(int indent);

  ExprStmt *get_lhs() { return lhs; }
  std::string get_op() { return op; }
  ExprStmt *get_rhs() { return rhs; }
};

class DispatchExpr : public ExprStmt {
  // ExprStmt *calling_class;
  ExprStmt *calling_expr;
  std::string name;
  ExprList args;

public:
  DispatchExpr(ExprStmt *calling_expr, std::string name, ExprList args) 
    : calling_expr(calling_expr), name(name), args(args) {}
  ExprType exprtype = DISPATCH_EXPR;
  void dump(int indent);

  ExprStmt *get_calling_expr() { return calling_expr; }
  std::string get_name() { return name; }
  ExprList get_args() { return args; }
  

  std::string classname() { return "DispatchExpr"; }
};


class ReturnExpr : public ExprStmt {
  ExprStmt *expr;
public:
  ReturnExpr(ExprStmt *expr) : expr(expr) {}
  // ReturnExpr() {}
  ExprType exprtype = RETURN_EXPR;
  void dump(int indent);
  std::string classname() { return "ReturnExpr";}

  ExprStmt *get_expr() { return expr; }
};


class IntConstExpr : public ExprStmt {
  long val;
public:
  IntConstExpr(long val) : val(val) {}
  ExprType exprtype = INT_CONST_EXPR;
  void dump(int indent);
  std::string classname() { return "IntConstExpr"; }

  long get_val() { return val; }
};

class VerseConstExpr : public ExprStmt {
  const std::string verse;
public: 
  VerseConstExpr(std::string verse) : verse(verse) {}
  ExprType exprtype = VERSE_CONST_EXPR;
  void dump(int indent);
  std::string classname() { return "VerseConstExpr";}

  std::string get_verse() { return verse; }
};

class BoolConstExpr : public ExprStmt {
  int val;
public:
  BoolConstExpr(std::string bool_val) { val = bool_val == "facts" ? 1 : 0; }
  ExprType exprtype = BOOL_CONST_EXPR;
  void dump(int indent);
  std::string classname() { return "BoolConstExpr";}

  int get_val() { return val; }
};

class ListConstExpr : public ExprStmt {
  ExprList exprlist;
public:
  ListConstExpr(ExprList exprlist) : exprlist(exprlist) {}
  ExprType exprtype = LIST_CONST_EXPR;
  void dump(int indent);
  std::string classname() { return "ListConstExpr"; }

  ExprList get_exprlist() { return exprlist; }
};

class ListElemRef: public ExprStmt {
  ObjectIdExpr *list_name;
  IntConstExpr *index;
public:
  ListElemRef(ObjectIdExpr *list_name, IntConstExpr *index) 
    : list_name(list_name), index(index) {}
  ExprType exprtype = LIST_ELEM_REF;
  void dump(int indent);
  std::string classname() { return "ListElemRef"; }

  ObjectIdExpr *get_list_name() { return list_name; }
  IntConstExpr *get_index() { return index; }

};

class ThisExpr : public ExprStmt {
  std::string name = "THIS";
public:
  ThisExpr() {}
  ExprType exprtype = THIS_EXPR;
  std::string classname() { return "ThisExpr"; }
  void dump(int indent);
};

class ContExpr : public ExprStmt {
  std::string name = "CONTINUE";
public:
  ContExpr() {}
  ExprType exprtype = CONT_EXPR;
  std::string classname() { return "ContExpr"; }
  void dump(int indent);
};

class BreakExpr: public ExprStmt {
  std::string name = "BREAK";
public:
  BreakExpr() {}
  ExprType exprtype = BREAK_EXPR;
  std::string classname() { return "BreakExpr"; }
  void dump(int indent);
};

class ObjectIdExpr : public ExprStmt {
  std::string name;
public:
  ObjectIdExpr(std::string name) : name(name) {}
  ExprType exprtype = OBJECTID_EXPR;
  std::string classname() { return "ObjectIdStmt"; }
  void dump(int indent);

  std::string get_name() { return name; }
};

class NoneExpr : public ExprStmt {
  std::string name = "NONE";
public:
  NoneExpr() {}
  ExprType exprtype = NONE_EXPR;
  std::string classname() { return "NoneExpr"; }
  void dump(int indent);

  std::string get_name() { return name; }

};

class NewExpr : public ExprStmt {
  ExprStmt *expr;
public:
  NewExpr(ExprStmt *expr) : expr(expr) {}
  ExprType exprtype = NEW_EXPR;
  std::string classname() { return "NewExpr"; }
  void dump(int indent);

  ExprStmt *get_expr() { return expr; }

};

class KillExpr: public ExprStmt {
  std::string name = "KILL";
  ExprStmt *expr;
public:
  // KillExpr() {}
  KillExpr(ExprStmt *expr) : expr(expr) {}
  ExprType exprtype = KILL_EXPR;
  std::string classname() { return "KillExpr"; }
  void dump(int indent);

  std::string get_name() { return name; }
  ExprStmt *get_error() { return expr; }

};