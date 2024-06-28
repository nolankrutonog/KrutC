#include <iostream>
#include <cassert>
#include <map>
#include <set>
#include "include/typechecker.h"
#include "include/scopetable.h"
#include "include/inheritance-graph.h"

using namespace std;

/* basic class names */
#define Void "void"
#define Object "object"
#define Int "int"
#define Bool "bool"
#define Char "char"
#define String "string"
#define List "list"
#define Stack "stack"

/* basic classes methods */
#define Constructor "constructor"
#define Length "length"
#define Clear "clear"
#define Is_Emtpy "is_emtpy"
#define Front "front"
#define Back "back"
#define Push_Back "push_back"
#define Pop_Back "pop_back"
#define Push_Front "push_front"
#define Pop_Front "pop_front"
#define Pop "pop"
#define Push "push"
#define Contains "contains"

static const set<string> basic_classes = {
  "void", 
  "object", 
  "int", 
  "bool", 
  "char", 
  "string", 
  "list", 
  "stack" 
};

static string curr_filename;                          /* current filename */
static int semant_errors = 0;                         /* semant errors */
static int warnings = 0;                              /* warnings */
static ScopeTable scopetable;                         /* scope table */

static vector<string> class_names;                    /* vector of class names */
static map<string, Type_*> class_type;                /* map from class name to Type_* */
static map<string, ClassStmt*> classes;               /* map from class name to ClassStmt* */
static map<string, vector<string>> class_parents;     /* map from class name to vector of parents */
static map<string, set<MethodStmt*>> class_methods;   /* map from class name to its MethodStmt* */
static map<string, set<AttrStmt*>> class_attrs;       /* map from class name to its AttrStmt* */
static map<string, set<MethodStmt*>> parent_methods;  /* intermediate map from class name to its parent methods */
static map<string, set<AttrStmt*>> parent_attrs;      /* intermediate map from class name to its parent attrs */

static set<MethodStmt*> global_methods;               /* set of global methods */


static bool conforms(Type_ *a, Type_ *b);
static void populate_parent_feature_tables(string& child, map<string, bool>& visited);
static bool check_valid_type_(Type_ *t);
bool check_method_sigs(MethodStmt *p, MethodStmt *c, const string& cname);

void error(int lineno, std::string err_msg) {
  semant_errors++;
  std::cout << curr_filename + ":" + std::to_string(lineno) + ": ";
  // TODO: can I make the 'ERROR' text red?? 
  std::cout << "ERROR: " + err_msg << std::endl;
}

void warning(int lineno, std::string warn_msg) {
  warnings++;
  std::cout << curr_filename + ":" + std::to_string(lineno) + ": ";
  // TODO: can I make the 'WARNING' text pink/other?? 
  std::cout << "WARNING: " + warn_msg << std::endl;
}

//////////////////////////////////////////////////////////////
//
// Initializing  
// 
//////////////////////////////////////////////////////////////


void TypeChecker::initialize_basic_classes() {
  /* bool, char, int, string, list, stack, void, object 
  defined below are the basic types and their methods
  */

  /*
  void ->
    parent: None
    no member attrs/methods
  */
  class_type[Void] = new Type_(Void, NULL);
  class_parents[Void] = {};
  class_names.push_back(Void);
  classes[Void] = new ClassStmt(Void, {}, {});

  /*
  object -> 
    parent: None
    no member attrs/methods
  */
  class_type[Object] = new Type_(Object, NULL);
  class_parents[Object] = {};
  class_names.push_back(Object);
  classes[Object] = new ClassStmt(Object, {}, {});

  /*
  bool -> 
    parent: object
    no member attrs/methods
  */
  class_type[Bool] = new Type_(Bool, NULL);
  class_parents[Bool].push_back(Object);
  class_names.push_back(Bool);
  classes[Bool] = new ClassStmt(Bool, {Object}, {});
  
  /*
  int -> 
    parent: object
    no member attrs/methods
  */
  class_type[Int] = new Type_(Bool, NULL);
  class_parents[Int].push_back(Object);
  class_names.push_back(Int);
  classes[Int] = new ClassStmt(Int, {Object}, {});

  /*
  char ->
    parent: object
    no member attrs/methods
  */
  class_type[Char] = new Type_(Char, NULL);
  class_parents[Char].push_back(Object);
  class_names.push_back(Char);
  classes[Char] = new ClassStmt(Char, {Object}, {});

  /*
  string ->
    parent: object
    int length() -- returns num chars in string
    void clear() -- empties the string
    bool is_empty() -- returns true on length == 0, false otherwise
    char front() -- returns char at the beginning (Runtime error when emtpy)
    char back() -- returns last at the end (Runtime error when empty)
    binary operators -- in the form `string1 op string2`
      acceptable ops: +, +=, ==, != 

    bracket operations:
      string[int n] -- returns char at index n
      string[(int n):(int m)] -- returns string from index n inclusive to m exclusive
        * m >= n
        * if n is not present --> n = 0
        * if m is not present --> m = string.length()
  */
  class_type[String] = new Type_(String, NULL);
  class_parents[String].push_back(Object);
  set<MethodStmt*> string_methods = {
    new MethodStmt(class_type[Int], Length, {}, {}),
    new MethodStmt(class_type[Void], Clear, {}, {}), 
    new MethodStmt(class_type[Bool], Is_Emtpy, {}, {}), 
    new MethodStmt(class_type[Char], Front, {}, {}), 
    new MethodStmt(class_type[Char], Back, {}, {})
  };
  class_methods[String] = string_methods;
  class_names.push_back(String);
  FeatureList string_features;
  for (MethodStmt *m: string_methods) {
    string_features.push_back(m);
  }
  classes[String] = new ClassStmt(String, {Object}, string_features);
  
  /*
  list<object> ->
    parent: object
    int length() -- returns num objects in list
    void clear() -- empties the list
    bool is_empty() -- returns true on length == 0, false otherwise
    void push_back(object o) -- adds object to end
    void push_front(object o) -- adds object to front
    void pop_front() -- pops first object
    void pop_back() -- pops last object
    object front() -- returns first object (None on empty)
    object back() -- returns last object
    int contains(object o) -- returns idx if o in list, else -1
    binary operators -- in the form `list1 op list2`
      acceptable ops: +, +=, ==, !=

    bracket operations:
      list[int n] -- returns object at index n
      list[(int n): (int m)] -- returns list from index n inclusive to m exclusive
        * m >= n
        * if n is not present --> n = 0
        * if m is not present --> m = list.length()
  */
  class_type[List] = new Type_(List, class_type[Object]);
  class_parents[List].push_back(Object);
  set<MethodStmt*> list_methods = {
    new MethodStmt(class_type[Int], Length, {}, {}),
    new MethodStmt(class_type[Void], Clear, {}, {}),
    new MethodStmt(class_type[Bool], Is_Emtpy, {}, {}),
    new MethodStmt(class_type[Void], Push_Back, {new FormalStmt(class_type[Object], Object)}, {}),
    new MethodStmt(class_type[Void], Push_Front, {new FormalStmt(class_type[Object], Object)}, {}),
    new MethodStmt(class_type[Void], Pop_Front, {}, {}),
    new MethodStmt(class_type[Void], Pop_Back, {}, {}),
    new MethodStmt(class_type[Object], Front, {}, {}),
    new MethodStmt(class_type[Object], Back, {}, {}),
    new MethodStmt(class_type[Int], Contains, {new FormalStmt(class_type[Object], Object)}, {})
  };
  class_methods[List] = list_methods; 
  class_names.push_back(List);
  FeatureList list_features;
  for (MethodStmt *m: list_methods) {
    list_features.push_back(m);
  }
  classes[List] = new ClassStmt(String, {Object}, list_features);

  /*
  stack<object> ->
    parent: object
    int length() -- returns num objects in stack
    void clear() -- empties the stack
    bool is_empty() -- returns true on length == 0, false otherwise
    void push(object o) -- pushes object on stack
    void pop() -- pops last object 
    object top() -- returns top object
  */
  class_type[Stack] = new Type_(Stack, class_type[Object]);
  class_parents[Stack].push_back(Object);
  set<MethodStmt*> stack_methods = {
    new MethodStmt(class_type[Int], Length, {}, {}),
    new MethodStmt(class_type[Void], Clear, {}, {}),
    new MethodStmt(class_type[Bool], Is_Emtpy, {}, {}),
    new MethodStmt(class_type[Void], Push, {new FormalStmt(class_type[Object], Object)}, {}),
    new MethodStmt(class_type[Void], Pop, {}, {}),
  };
  class_methods[Stack] = stack_methods; 
  class_names.push_back(Stack);
  FeatureList stack_features;
  for (MethodStmt *m: stack_methods) {
    stack_features.push_back(m);
  }
  classes[Stack] = new ClassStmt(String, {Object}, stack_features);
}

/* Returns false if T is invalid. ex: list, char<string> are invalid */ 
bool check_valid_type_(Type_ *t) {
  Type_ *nested = t->get_nested_type();
  if (t->get_name() == List || t->get_name() == Stack) {
    if (nested == NULL) {
      return false; 
    } else {
      check_valid_type_(nested);
    }
  } else if (nested) {
    return false;
  }
  return true;
    
}

/* Declared classes (of the form class ClassName {...}) can only be decalared in outer scope */
void TypeChecker::initialize_declared_classes() {
  for (int i = 0; i < program.len(); i++) {
    Stmt *s = program.ith(i);

    ClassStmt *cs = dynamic_cast<ClassStmt*>(s);
    if (!cs)
      continue;
    string name = cs->get_name();
    // if class defined twice
    if (class_type.find(name) != class_type.end()) {
      string err_msg = "Class " + name + " cannot be defined twice.";
      error(cs->lineno, err_msg);
      continue;
    }
    class_type[name] = new Type_(name, NULL);
    class_names.push_back(name);

    FeatureList feature_list = cs->get_feature_list();
    for (Feature *f: feature_list) {
      if (f->is_method()) {
        MethodStmt *m = static_cast<MethodStmt*>(f);
        if (m->get_name() == Constructor) {
          string err_msg = "Redefined constructor in class " + name + ", or method named `constructor`. A method `constructor` is builtin for every class.";
          error(m->lineno, err_msg);
        }
        set<MethodStmt*> *existing_methods = &class_methods[cs->get_name()];
        for (MethodStmt *em: *existing_methods) {
          // check current method against existing methods
          if (em->get_name() == m->get_name()) {
            string err_msg = "Method " + m->get_name() + " cannot be defined twice in class " + cs->get_name();
            error(m->lineno, err_msg);
            continue;
          }
          
        }
        existing_methods->insert(m); // this should insert it in class_methods bc ptr

        if (!check_valid_type_(m->get_ret_type())) {
          string err_msg = "Method `" + m->get_name() + "` has invalid return type `" + m->get_ret_type()->to_str() + "`";
          error(m->lineno, err_msg);
        }

        FormalList formal_list = m->get_formal_list();
        for (FormalStmt *f: formal_list) {
          if (!check_valid_type_(f->get_type())) {
            string err_msg = "Formal `" + f->get_name() + "` has invalid type `" + f->get_type()->to_str() + "`";
            error(f->lineno, err_msg);
          }
        }

      } else {
        AttrStmt *a = static_cast<AttrStmt*>(f);
        set<AttrStmt*> *existing_attrs = &class_attrs[cs->get_name()];
        for (AttrStmt *ea: *existing_attrs) {
          if (ea->get_name() == a->get_name()) {
            string err_msg = "Attribute " + a->get_name() + " cannot be defined twice in class " + cs->get_name();
            error(a->lineno, err_msg);
            continue;
          }
        }
        if (!check_valid_type_(a->get_type())) {
          string err_msg = "Attribute `" + a->get_name() + "` in class `" + name + "` has invalid type `" + a->get_type()->to_str() + "`";
          error(a->lineno, err_msg);
        }
        existing_attrs->insert(a); // this should insert it in class_attrs bc ptr
      }
    }
  }
}

/* check that parents arent repeating and are not from base classes */
void TypeChecker::check_valid_class_parents() {
  for (int i = 0; i < program.len(); i++) {
    Stmt *s = program.ith(i);

    ClassStmt *cs = dynamic_cast<ClassStmt*>(s);
    if (!cs)
      continue;

    string name = cs->get_name();
    vector<string> parents = cs->get_parents();
    for (string parent: parents) {
      if (class_type.find(parent) == class_type.end()) {
        // unknown parent
        string err_msg = "Class " + name + " inheriting from unknown class " + parent;
        error(cs->lineno, err_msg);
        /* erase bad parents so we can typecheck as if they have good parents */
        parents.erase(remove(parents.begin(), parents.end(), parent), parents.end());
        continue;
      }
      if (basic_classes.find(parent) != basic_classes.end()) {
        string err_msg = "Class " + name + " cannot inherit from base class " + parent;
        error(cs->lineno, err_msg);
        /* erase bad parents so we can typecheck as if they have good parents */
        parents.erase(remove(parents.begin(), parents.end(), parent), parents.end());
        continue;
      }
    }
    if (parents.empty())
      parents.push_back(Object);
    class_parents[name] = parents;
    
  }
}

/* prints class_parents */
void print_class_parents() {
  for (pair<string, vector<string>> clp: class_parents) {
    cout << clp.first << ": ";
    if (clp.second.empty()) {
      cout << "None" << endl;
      continue;
    }
    cout << clp.second[0];
    for (int i = 1; i < clp.second.size(); i++) {
      cout << ", " + clp.second[i];
    }
    cout << endl;
  }
}


/* 
  Returns false if graph contains inheritance cycles, true otherwise. 
*/
bool TypeChecker::check_inheritance_cycles() {
  InheritanceGraph *g = new InheritanceGraph(class_parents);
  vector<vector<string>> cycles;
  g->check_for_cycles(cycles);
  if (!cycles.empty()) {
    for (vector<string>& cycle: cycles) {
      string err_msg = "Cycle detected in classes " + cycle[0];
      for (int i = 1; i < (int) cycle.size(); i++) {
        err_msg += ", " + cycle[i];
      }
      error(0, err_msg);
    }
    return false;
  }
  return true;
}

/* DFS to populate a set of meths/attrs that is the intersection of the child's parent's meths/attrs */
void populate_parent_feature_tables(string& child, map<string, bool>& visited) {
  visited[child] = true;

  for (string& parent: class_parents[child]) {
    if (!visited[parent]) {
      populate_parent_feature_tables(parent, visited);
    }
    for (MethodStmt *nmeth: class_methods[parent]) {
      bool can_insert = true;
      for (MethodStmt *emeth: parent_methods[child]) {
        if (emeth->get_name() == nmeth->get_name()) {
          can_insert = false;
          break;
        }
      }
      if (can_insert) {
        parent_methods[child].insert(nmeth);
      }
    }

    for (AttrStmt *nattr: class_attrs[parent]) {
      bool can_insert = true;
      for (AttrStmt *eattr: parent_attrs[child]) {
        if (eattr->get_name() == nattr->get_name()) {
          can_insert = false;
          break;
        }
      }
      if (can_insert) {
        parent_attrs[child].insert(nattr);
      }
    }
  }
}

/* Returns true if return type and formal list are consistent between inherited methods, otherwise false */
bool check_method_sigs(MethodStmt *p, MethodStmt *c, const string& cname) {
  bool ret_val = true;
  if (!conforms(c->get_ret_type(), p->get_ret_type())) {
    string err_msg = "Return type `" + c->get_ret_type()->to_str() + "`"; //  of method `" + c->get_name() + "` in class `" + cname + "`";
    err_msg += " does not conform to inherited return type `" + p->get_ret_type()->to_str() + "` defined on line " + to_string(p->lineno);
    error(c->lineno, err_msg);
    ret_val = false; 
  }

  return ret_val;
}


void TypeChecker::populate_feature_tables() {
  map<string, bool> visited;
  for (pair<string, vector<string>> entry: class_parents) {
    if (!visited[entry.first]) {
      populate_parent_feature_tables(entry.first, visited);
    }
  }

  for (const string& cname: class_names) {
    /* for every class */
    for (MethodStmt *pmeth: parent_methods[cname]) {
      bool can_insert = true;
      for (MethodStmt *cmeth: class_methods[cname]) {
        if (pmeth->get_name() == cmeth->get_name()) {
          if (!check_method_sigs(pmeth, cmeth, cname)) {
            can_insert = false;
            break;
          }
        }
      }
      if (can_insert) {
        class_methods[cname].insert(pmeth);
      }
    }

    for (AttrStmt *pattr: parent_attrs[cname]) {
      bool can_insert = true;
      for (AttrStmt *cattr: class_attrs[cname]) {
        if (pattr->get_name() == cattr->get_name()) {
          string err_msg = "Class " + cname + " cannot redefine inherited attribute `" + cattr->get_name() + "` defined by parent on line " + to_string(pattr->lineno);
          error(cattr->lineno, err_msg);
          can_insert = false;
        }
      }
      if (can_insert) {
        class_attrs[cname].insert(pattr);
      }
    }
  }
}

bool TypeChecker::check_global_features() {
  bool ret_val = true;
  for (int i = 0; i < program.len(); i++) {
    Stmt *s = program.ith(i);
    Feature *f = dynamic_cast<Feature*>(s);
    if (!f)
      continue;

    if (f->is_method()) {
      MethodStmt *m = static_cast<MethodStmt*>(f);
      /* check return type is valid */
      if (!check_valid_type_(m->get_ret_type())) {
        string err_msg = "Method `" + m->get_name() + "` has invalid return type `" + m->get_ret_type()->to_str() + "`";
        error(m->lineno, err_msg);
        ret_val = false;
      }

      /* check formal type is valid */
      FormalList formal_list = m->get_formal_list();
      for (FormalStmt *f: formal_list) {
        if (!check_valid_type_(f->get_type())) {
          string err_msg = "Formal `" + f->get_name() + "` has invalid type `" + f->get_type()->to_str() + "`";
          error(f->lineno, err_msg);
          ret_val = false;
        }
      }

      /* check if method is already defined */
      bool can_insert = true;
      for (MethodStmt *gm: global_methods) {
        if (gm->get_name() == m->get_name()) {
          string err_msg = "Method `" + m->get_name() + "` cannot be defined twice";
          error(m->lineno, err_msg);
          ret_val = false;
          can_insert = false;
        }
      }
      if (can_insert) {
        global_methods.insert(m);
      }
    } else {
      AttrStmt *a = static_cast<AttrStmt*>(f);
      if (!check_valid_type_(a->get_type())) {
        string err_msg = "Attribute `" + a->get_name() + "` has invalid type `" + a->get_type()->to_str() + "`";
        error(a->lineno, err_msg);
        ret_val = false;
      }
    }

  }
  return ret_val;

}

/* entry point for type checking, called from main. */
int TypeChecker::typecheck() {

  curr_filename = filename;

  initialize_basic_classes();
  initialize_declared_classes();
  check_valid_class_parents();

  if (!check_inheritance_cycles()) {
    /* cannot continue typechecking if there are inheritance cycles */
    return semant_errors++;
  } 

  populate_feature_tables();

  if (!check_global_features()) {
    /* cannot continue typechecking if global features have type declaration errors */
    return semant_errors;
  }
  scopetable.push_scope();

  for (int i = 0; i < program.len(); i++) {
    Stmt *s = program.ith(i);
    s->typecheck();
  }

  scopetable.pop_scope();
  
  return semant_errors;
  
}


//////////////////////////////////////////////////////////////
//
// 
// TYPE EXPRESSIONS  
// 
// 
//////////////////////////////////////////////////////////////

/* returns true of a conforms to b, false otherwise */
bool conforms_util(const string& a, const string& b) {
  if (a == b) {
    return true;
  }

  stack<string> frontier;
  frontier.push(a);
  set<string> visited;

  while (!frontier.empty()) {
    string curr = frontier.top();
    frontier.pop();

    if (curr == b) {
      return true;
    }

    visited.insert(curr);

    for (string& parent: class_parents[curr]) {
      if (visited.find(parent) == visited.end()) {
        frontier.push(parent);
      }
    }
  }
  return false;

}

/* Given two ptrs to Type_ objects, returns true if A conforms to B, false otherwise. */
bool conforms(Type_ *a, Type_ *b) {
  if (b->get_name() == Object) {
    /* every class conforms to Object class */
    return true;
  }

  if (!conforms_util(a->get_name(), b->get_name())) {
    return false;
  }

  Type_ *a_nest = a->get_nested_type();
  Type_ *b_nest = b->get_nested_type();

  if (a_nest == NULL && b_nest != NULL || a_nest != NULL && b_nest == NULL) {
    return false;
  }
  if (a_nest == NULL && b_nest == NULL)
    return true;
  if (a_nest != NULL && b_nest != NULL)
    return conforms(a_nest, b_nest);
  
}



//////////////////////////////////////////////////////////////
//
// 
// Statement Typechecking 
// 
// 
//////////////////////////////////////////////////////////////

/* 
  STATEMENTS should return NULL on every typecheck
*/

Type_ *FormalStmt::typecheck() {
  scopetable.add_elem(name, type);
  return NULL;
}

Type_ *MethodStmt::typecheck() {
  scopetable.push_scope();
  /* add all formals */
  for (FormalStmt *f: formal_list) {
    f->typecheck();
  }

  /* typecheck every expression, save the last one for RETURN possibility */
  int i;
  for (i = 0; i < (int) stmt_list.size() - 1; i++) {
    stmt_list[i]->typecheck();
  }

  if (!stmt_list.empty()) {
    Stmt *last_stmt = stmt_list[i];

    Type_ *last_type = last_stmt->typecheck();
    if (!last_type)
      goto done;

    if (!conforms(class_type[Void], ret_type)) {
      /* if method expects a return value */
      if (last_stmt->get_stmttype() != RETURN_EXPR) {
        // TODO: is there a way to get lineno here to work? last_stmt is generic and we wont know the error...
        string err_msg = "Method `" + name + "` expects return expression, with type `" + ret_type->to_str() + "`";
        error(last_stmt->lineno, err_msg);
      } else {
        ReturnExpr *retexpr = static_cast<ReturnExpr*>(last_stmt);
        if (!conforms(last_type, ret_type)) {
          /* if return type and decl ret type do not match */
          string err_msg = "Return expression type `" + last_type->to_str() + "` does not conform to declared return type `" + ret_type->to_str() + "`";
          error(retexpr->lineno, err_msg);
        }
      }
    } else { // ret_type == class_type[Void]
      if (last_stmt->get_stmttype() == RETURN_EXPR) {
        ReturnExpr *retexpr = static_cast<ReturnExpr*>(last_stmt);
        if (last_type != NULL) {
          string warn_msg = "Method `" + name + "` (declared as void) returns a non-void value.";
          warning(retexpr->lineno, warn_msg);
        }
      }
    }

  }

done: 
  scopetable.pop_scope();
  return NULL;
}

Type_ *IfStmt::typecheck() {
  Type_ *pred_type = pred->typecheck();
  if (pred_type && !conforms(pred_type, class_type[Bool])) {
    string err_msg = "Expected bool in predicate, instead got `" + pred_type->to_str() + "`";
    error(pred->lineno, err_msg);
  }

  for (Stmt *s: then_branch) {
    s->typecheck();
  }

  for (Stmt *s: else_branch) {
    s->typecheck();
  }

  return NULL;
}
Type_ *ClassStmt::typecheck() {

  return NULL;
}
Type_ *ForStmt::typecheck() {

  return NULL;
}

/* checks if init expr type conforms to declared, adds to scopetable */
Type_ *AttrStmt::typecheck() {
  Type_ *init_type = init->typecheck();
  if (init_type && !conforms(init_type, type)) {
    // string err_msg = "Type `" + init_type->to_str() + "` of attribute `" + name;
    // err_msg += "` does not conform to declared type `" + type->to_str() + "`";

    string err_msg = "Attribute `" + name + "` (declared as " + type->to_str() + ") is initialized to a value of type " + init_type->to_str();
    error(lineno, err_msg);
  }
  /* give declared type precedence on error, else they are the same */
  scopetable.add_elem(name, type);
  return NULL;
}

Type_ *WhileStmt::typecheck() {

  return NULL;
}




/* 
  EXPRESSIONS must return their type
*/
Type_ *ReturnExpr::typecheck() {
  /* possible that RETURN has no expression */
  return expr ? expr->typecheck() : NULL;  
}
Type_ *ListElemRef::typecheck() {
  /* should return T = list<object> */
  Type_ *name_type = list_name->typecheck();

  if (!name_type) 
    return NULL;

  return name_type->get_nested_type();
}

Type_ *DispatchExpr::typecheck() {
  Type_ *calling_type = calling_expr->typecheck(); 
}
Type_ *IntConstExpr::typecheck() {
  return new Type_(Int, NULL);
}

Type_ *ObjectIdExpr::typecheck() {
  Type_ *type_ = scopetable.lookup(name);
  if (!type_) {
    string err_msg = "Unknown reference: `" + name + "`";
    error(lineno, err_msg);
  }
  return type_; 
}

Type_ *BoolConstExpr::typecheck() {
  return new Type_(Bool, NULL);
}
Type_ *ListConstExpr::typecheck() {
  return new Type_(List, new Type_("object", NULL));
}
Type_ *StrConstExpr::typecheck() {
  return new Type_(String, NULL);
}
Type_ *NewExpr::typecheck() {
  return class_type[newclass];
}
Type_ *ContExpr::typecheck() {}
Type_ *KillExpr::typecheck() {}
Type_ *ThisExpr::typecheck() {}
Type_ *BinopExpr::typecheck() {}
Type_ *BreakExpr::typecheck() {
  return new Type_("break", NULL);
}
