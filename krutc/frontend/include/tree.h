#ifndef TREE_H
#define TREE_H

#include <iostream>
#include <vector>
#include <set>



enum StmtType {
  STMT = 500,
  CLASS_STMT = 501,
  ATTR_STMT = 502,
  METHOD_STMT = 503,
  FORMAL_STMT = 504,
  FOR_STMT = 505,
  IF_STMT = 506,
  WHILE_STMT = 507,


  EXPR_EXPR = 600,
  RETURN_EXPR = 601,
  INT_CONST_EXPR = 602,
  STRING_CONST_EXPR = 603,
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
  LIST_ELEM_REF = 616,
  TYPE_EXPR = 617,
  CHAR_CONST_EXPR = 618,
  SUBLIST_EXPR = 619
};


class Program;
class Stmt;
typedef class std::vector<Stmt*> StmtList; 

class ClassStmt;
class Feature;
typedef class std::vector<Feature*> FeatureList;
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
class StrConstExpr;
class CharConstExpr;
class BoolConstExpr;
class ListConstExpr;
class ListElemRef;
class SublistExpr;
class ObjectIdExpr;
class DispatchExpr;
class BinopExpr;
class ContExpr;
class BreakExpr;
class KillExpr;


class Type_;



/*
  tree.h
  Defines the KrutC language. A program is represented as a list (vector) of stmts.
  Every stmt can be broken up into smaller parts. 
*/
class Stmt {
public:
  virtual StmtType get_stmttype() = 0;
  int lineno = 0;
  virtual ~Stmt() = default;
  virtual void dump(int indent) = 0;
  virtual std::string classname() { return "Stmt"; };
  virtual Type_ *typecheck() = 0;
};

class Program {
  StmtList stmt_list;
public:
  Program() {}
  void add_stmt(Stmt *stmt) { stmt_list.push_back(stmt); } 
  int len() { return (int) stmt_list.size(); }
  Stmt *ith(int i) { return stmt_list[i]; }
  void dump(); 
  StmtList get_stmt_list() { return stmt_list; }
};

class ExprStmt : public Stmt {
public:
  StmtType get_stmttype() { return EXPR_EXPR; }
  int lineno = 0;
  virtual std::string classname() { return "ExprStmt";}
  virtual void dump(int indent);
  
};


////////////////////////////////////////////////////////////
//
//
// Statements
//
//
////////////////////////////////////////////////////////////
class ClassStmt: public Stmt {
  std::string name;
  std::vector<std::string> parents;
  FeatureList feature_list;

public:
  ClassStmt(std::string name, std::vector<std::string> parents, FeatureList feature_list):
    name(name), parents(parents), feature_list(feature_list) {}
  
  StmtType get_stmttype() { return CLASS_STMT; }
  std::string classname() { return "ClassStmt"; } 
  void dump(int indent); 

  std::string get_name() { return name; } 
  std::vector<std::string> get_parents() { return parents; } 
  FeatureList get_feature_list() { return feature_list; } 
  Type_ *typecheck();

};


class Feature : public Stmt {
public:
  virtual bool is_method() = 0;
};

/* in the form TYPEID OBJECTID = EXPRSTMT; */
class AttrStmt : public Feature {
  Type_ *type;
  std::string name;
  ExprStmt *init;
public:
  AttrStmt(Type_ *type, std::string name, ExprStmt *init) : 
    type(type), name(name), init(init) {}
  StmtType get_stmttype() { return ATTR_STMT; }
  void dump(int indent);
  bool is_method() { return false; } 
  std::string classname()  { return "AttrStmt"; } 
  Type_ *get_type() { return type; } 
  std::string get_name() { return name; } 
  ExprStmt *get_init() { return init; }  
  Type_ *typecheck();

};

class FormalStmt : public Stmt {
  Type_ *type;
  std::string name;

public:
  FormalStmt(Type_ *type, std::string name) : type(type), name(name) {}
  StmtType get_stmttype() { return FORMAL_STMT; }
  void dump(int indent);
  std::string classname()  { return "FORMAL_STMT"; } 
  std::string get_name() { return name; } 
  Type_ *get_type() { return type; } 
  Type_ *typecheck();
};



/* in the form TYPEID OBJECTID(FORMAL_LIST) {STMT_LIST}*/
class MethodStmt : public Feature {
  Type_ *ret_type;
  std::string name;
  FormalList formal_list;
  StmtList stmt_list;
public:
  MethodStmt(Type_ *ret_type, std::string name, FormalList formal_list, StmtList stmt):
    ret_type(ret_type), name(name), formal_list(formal_list), stmt_list(stmt) {}
  StmtType get_stmttype() { return METHOD_STMT; }
  std::string classname() { return "MethodStmt"; } 
  void dump(int indent);

  bool is_method() { return true; } 
  std::string get_name() { return name; } 
  Type_ *get_ret_type() { return ret_type; } 
  FormalList get_formal_list() { return formal_list; } 
  StmtList get_stmt_list() { return stmt_list; } 
  Type_ *typecheck();

};

class ForStmt : public Stmt {
  Stmt *stmt;         /* stmt is executed one time */
  ExprStmt *cond;     /* the loop repeats until condition is false */
  ExprStmt *repeat;   /* repeat is executed on every loop */
  StmtList stmt_list;

public:
  ForStmt(Stmt *stmt, ExprStmt *cond, 
          ExprStmt *repeat, StmtList stmt_list)
    : stmt(stmt), cond(cond), 
      repeat(repeat), stmt_list(stmt_list) {}
  StmtType get_stmttype() { return FOR_STMT; }
  void dump(int indent);
  std::string classname() { return "ForStmt"; }

  Stmt *get_formal() { return stmt; }
  ExprStmt *get_cond() { return cond; }
  ExprStmt *get_repeat() { return repeat; }
  StmtList get_stmt_list() { return stmt_list; }
  Type_ *typecheck();
};


class IfStmt : public Stmt {
  ExprStmt *pred;
  StmtList then_branch;
  StmtList else_branch;

public:
  IfStmt(ExprStmt *pred, StmtList then_branch, StmtList else_branch) :
    pred(pred), then_branch(then_branch), else_branch(else_branch) {}
  StmtType get_stmttype() { return IF_STMT; }
  void dump(int indent);
  std::string classname() { return "IfStmt";}
  std::string get_name() { return "IfStmt"; }

  ExprStmt *get_pred() { return pred; }
  StmtList get_then() { return then_branch; }
  StmtList get_else() { return else_branch; }
  Type_ *typecheck();
};

class WhileStmt : public Stmt {
  ExprStmt *pred;
  StmtList stmt_list;
public: 
  WhileStmt(ExprStmt *pred, StmtList stmt_list) : pred(pred), stmt_list(stmt_list) {}
  StmtType get_stmttype() { return WHILE_STMT; }
  void dump(int indent);
  std::string classname() { return "WhileStmt";}
  std::string get_name() { return "WhileStmt"; }

  ExprStmt *get_pred() { return pred; }
  StmtList get_stmt_list() { return stmt_list; }
  Type_ *typecheck();
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
  StmtType get_stmttype() { return BINOP_EXPR; }
  std::string classname() { return "BinopExpr"; }
  void dump(int indent);

  ExprStmt *get_lhs() { return lhs; }
  std::string get_op() { return op; }
  ExprStmt *get_rhs() { return rhs; }
  Type_ *typecheck();
};

class DispatchExpr : public ExprStmt {
  // ExprStmt *calling_class;
  ExprStmt *calling_expr;
  std::string name;
  ExprList args;

public:
  DispatchExpr(ExprStmt *calling_expr, std::string name, ExprList args) 
    : calling_expr(calling_expr), name(name), args(args) {}
  StmtType get_stmttype() { return DISPATCH_EXPR; }
  void dump(int indent);
  std::string classname() { return "DispatchExpr"; }

  ExprStmt *get_calling_expr() { return calling_expr; }
  std::string get_name() { return name; }
  ExprList get_args() { return args; }
  

  Type_ *typecheck();
};


class ReturnExpr : public ExprStmt {
  ExprStmt *expr;
public:
  ReturnExpr(ExprStmt *expr) : expr(expr) {}
  // ReturnExpr() {}
  StmtType get_stmttype() { return RETURN_EXPR; }
  void dump(int indent);
  std::string classname() { return "ReturnExpr";}

  ExprStmt *get_expr() { return expr; }
  Type_ *typecheck();
};


class IntConstExpr : public ExprStmt {
  long val;
public:
  IntConstExpr(long val) : val(val) {}
  StmtType get_stmttype() { return INT_CONST_EXPR; }
  void dump(int indent);
  std::string classname() { return "IntConstExpr"; }

  long get_val() { return val; }
  Type_ *typecheck();
};

class StrConstExpr : public ExprStmt {
  const std::string str;
public: 
  StrConstExpr(std::string str) : str(str) {}
  StmtType get_stmttype() { return STRING_CONST_EXPR; }
  void dump(int indent);
  std::string classname() { return "StrConstExpr";}

  std::string get_str() { return str; }
  Type_ *typecheck();
};

class CharConstExpr : public ExprStmt {
  const std::string c;
public: 
  CharConstExpr(std::string c) : c(c) {}
  StmtType get_stmttype() { return CHAR_CONST_EXPR; }
  void dump(int indent);
  std::string classname() { return "CharConstExpr";}

  std::string get_str() { return c; }
  Type_ *typecheck();
};

class BoolConstExpr : public ExprStmt {
  int val;
public:
  BoolConstExpr(std::string bool_val) { val = bool_val == "true" ? 1 : 0; }
  StmtType get_stmttype() { return BOOL_CONST_EXPR; }
  void dump(int indent);
  std::string classname() { return "BoolConstExpr";}

  int get_val() { return val; }
  Type_ *typecheck();
};

class ListConstExpr : public ExprStmt {
  ExprList exprlist;
public:
  ListConstExpr(ExprList exprlist) : exprlist(exprlist) {}
  StmtType get_stmttype() { return LIST_CONST_EXPR; }
  void dump(int indent);
  std::string classname() { return "ListConstExpr"; }

  ExprList get_exprlist() { return exprlist; }
  Type_ *typecheck();
};

class ListElemRef: public ExprStmt {
  ExprStmt *list_name;
  ExprStmt *index;
public:
  ListElemRef(ExprStmt *list_name, ExprStmt *index) 
    : list_name(list_name), index(index) {}
  StmtType get_stmttype() { return LIST_ELEM_REF; }
  void dump(int indent);
  std::string classname() { return "ListElemRef"; }

  ExprStmt *get_list_name() { return list_name; }
  ExprStmt *get_index() { return index; }

  Type_ *typecheck();
};

class SublistExpr: public ListElemRef {
  ExprStmt *end_idx;

public:
  SublistExpr(ExprStmt *list_name, ExprStmt *start_idx, ExprStmt *end_idx)
    : ListElemRef(list_name, start_idx), 
    end_idx(end_idx) {}
  StmtType get_stmttype() { return SUBLIST_EXPR; }
  void dump(int indent);
  std::string classname() { return "SublistExpr"; }

  ExprStmt *get_st_idx() { return get_index(); }
  ExprStmt *get_end_idx() { return end_idx; }

  Type_ *typecheck();
};

class ContExpr : public ExprStmt {
  std::string name = "CONTINUE";
public:
  ContExpr() {}
  StmtType get_stmttype() { return CONT_EXPR; }
  std::string classname() { return "ContExpr"; }
  void dump(int indent);
  Type_ *typecheck();
};

class BreakExpr: public ExprStmt {
  std::string name = "BREAK";
public:
  BreakExpr() {}
  StmtType get_stmttype() { return BREAK_EXPR; }
  std::string classname() { return "BreakExpr"; }
  void dump(int indent);
  Type_ *typecheck();
};

class ObjectIdExpr : public ExprStmt {
  std::string name;
public:
  ObjectIdExpr(std::string name) : name(name) {}
  StmtType get_stmttype() { return OBJECTID_EXPR; }
  std::string classname() { return "ObjectIdStmt"; }
  void dump(int indent);

  std::string get_name() { return name; }
  Type_ *typecheck();
};

// class NoneExpr : public ExprStmt {
//   std::string name = "NONE";
// public:
//   NoneExpr() {}
//   StmtType get_stmttype() { return NONE_EXPR; }
//   std::string classname() { return "NoneExpr"; }
//   void dump(int indent);

//   std::string get_name() { return name; }
//   // Type_ *typecheck();

// };

class NewExpr : public ExprStmt {
  // ExprStmt *expr;
  std::string newclass;
public:
  NewExpr(std::string newclass) : newclass(newclass) {}
  StmtType get_stmttype() { return NEW_EXPR; }
  std::string classname() { return "NewExpr"; }
  void dump(int indent);

  std::string get_newclass() { return newclass; }
  Type_ *typecheck();

};

class KillExpr: public ExprStmt {
  std::string name = "KILL";
  ExprStmt *expr;
public:
  // KillExpr() {}
  KillExpr(ExprStmt *expr) : expr(expr) {}
  StmtType get_stmttype() { return KILL_EXPR; }
  std::string classname() { return "KillExpr"; }
  void dump(int indent);

  std::string get_name() { return name; }
  ExprStmt *get_error() { return expr; }

  Type_ *typecheck();
};

/* this is not an actual expression, but a class used for 
   typechecking. 

  Basic types include:
  object, int, str, bool, float(maybe?), list<object>, stack<object>
*/
class Type_ {
  std::string name;
  Type_ *nested_type;
public:
  int lineno;
  Type_(std::string name, Type_ *nested_type)
    : name(name), nested_type(nested_type) {}
  void dump(int indent);
  std::string to_str();

  std::string get_name() { return name; }
  Type_ *get_nested_type() { return nested_type; }
};

#endif // TREE_H