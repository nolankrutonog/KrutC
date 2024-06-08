#include "include/tree.h"
#include <iostream>

using namespace std;

void indent(int n) {
  for (int i = 0; i < n; i++) {
    cout << " ";
  }
}

void Program::dump() {
  for (Stmt *stmt : stmt_list) {
    stmt->dump(0);
    cout << "---------------------------" << endl;
  }
}

void BroStmt::dump(int n) {
  indent(n); cout << "bro" << endl;
  indent(n + 1); cout << name << endl;
  if (vibers.size() > 0) {
    indent(n + 2);
    cout << "vibers: ";
    for (string viber: vibers) {
      cout <<  viber + " ";
    }
    cout << endl;

  }
  for (Stmt *stmt : stmt_list) {
    stmt->dump(n + 2);
  }
}

void AttrStmt::dump(int n) {
  // indent(n); cout << type->dump(n) + " " + name << endl;
  indent(n); type->dump(0); cout << " " + name << endl;
  // indent(n + 1); cout << name << endl;
  if (init) {
    indent(n + 1); cout << "init" << endl;
    init->dump(n + 2);
  }
}

void MethodStmt::dump(int n) {
  indent(n);
  cout << "method: "; ret_type->dump(0); cout << " " + name + "()" << endl;
  if (formal_list.size()) {
    indent(n + 1); cout << "formals -> ";
    for (FormalStmt *formal: formal_list) {
      formal->dump(n + 2);
    }
    cout << endl;
  }
  for (Stmt *stmt : stmt_list) {
    stmt->dump(n + 2);
  }
}

void ThisExpr::dump(int n) {
  indent(n); cout << name << endl;
}

void ContExpr::dump(int n) {
  indent(n); cout << name << endl;
}
void BreakExpr::dump(int n) {
  indent(n); cout << name << endl;
}

void ExprStmt::dump(int n) {}
void IntConstExpr::dump(int n) {
  indent(n);
  cout << to_string(val) << endl;
}
void BoolConstExpr::dump(int n) {
  string out = val == 1 ? "facts" : "cap";
  indent(n);
  cout << out << endl;
}
void VerseConstExpr::dump(int n) {
  indent(n);
  cout << verse << endl;
}

void ListConstExpr::dump(int n) {
  indent(n); cout << "[" << endl;
  for (ExprStmt *e: exprlist) {
    e->dump(n + 1);
  }
  indent(n); cout << "]" << endl;
}
void ListElemRef::dump(int n) {
  list_name->dump(n);
  indent(n + 1); cout << "[" << endl;
  index->dump(n + 2);
  indent(n + 1); cout << "]" << endl;
}
// void ContainerStmt::dump(int n) {
//   indent(n); cout << type + "<" + elem_type + "> " + name << endl;
//   init->dump(n + 1);
// }

void ReturnExpr::dump(int n) {
  indent(n);
  cout << "return " << endl;
  if (expr)
    expr->dump(n + 1);
}
void WhileStmt::dump(int n) {
  indent(n); cout << "while" << endl;
  pred->dump(n + 1);
  for (Stmt *s: stmt_list) {
    s->dump(n + 1);
  }
}
void IfStmt::dump(int n) {
  indent(n); cout << "if" << endl;
  pred->dump(n + 1);
  indent(n); cout << "then" << endl;
  for (Stmt *s: then_branch) {
    s->dump(n + 1);
  }
  if (else_branch.size()) {
    indent(n); cout << "else" << endl;
    for (Stmt *s: else_branch) {
      s->dump(n + 1);
    }
  }
}
void ForStmt::dump(int n) {
  indent(n); cout << "for" << endl;
  if (stmt)
    stmt->dump(n + 1); cout << endl;
  if (cond)
    cond->dump(n + 1);
  if (repeat)
    repeat->dump(n + 1);

  for (Stmt *s: stmt_list) {
    s->dump(n + 2);
  }
}
void DispatchExpr::dump(int n) {
  if (calling_expr)
    calling_expr->dump(n);
  indent(n); cout << "." + name << endl;
  for (ExprStmt *a: args) {
    a->dump(n + 1);
  }

}

void TypeExpr::dump(int n) {
  cout << name;
  if (nested_type) {
    cout << "<";
    nested_type->dump(n + 1);
    cout << ">";
  }
}

void FormalStmt::dump(int n) {
  // indent(n); 
  cout <<  type->get_name() + " " + name + ", ";
}

void ObjectIdExpr::dump(int n) {
  indent(n); cout << name << endl;
}


void BinopExpr::dump(int n) {
  if (lhs) {
    lhs->dump(n + 1);
  }
  if (op != "") {
    indent(n + 1); 
    cout << op << endl;
  }
  if (rhs) {
    rhs->dump(n + 1);
  }
  
}

void NoneExpr::dump(int n) {}
void NewExpr::dump(int n) {
  indent(n);
  cout << "new" << endl;
  expr->dump(n + 1);
}
void KillExpr::dump(int n) {
  indent(n);
  cout << "kill: " << endl;
  expr->dump(n + 1);
}

