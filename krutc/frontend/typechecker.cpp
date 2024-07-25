#include "include/typechecker.h"

#include <cassert>
#include <iostream>
#include <map>
#include <set>

#include "include/constants.h"
#include "include/error.h"
#include "include/inheritance-graph.h"
#include "include/scopetable.h"

using namespace std;
using namespace basic_classes;
using namespace typechecking;

static string curr_filename;  /* current filename */
static int semant_errors = 0; /* semant errors */
static int warnings = 0;      /* warnings */
static ScopeTable scopetable; /* scope table */

static vector<string> class_names;                /* vector of class names */
static map<string, Type_ *> class_type;           /* class to Type_* */
static map<string, ClassStmt *> classes;          /* class to ClassStmt* */
static map<string, vector<string>> class_parents; /* class to parents */
static map<string, set<string>> class_ancestors;  /* class to all ancestors */
static map<string, set<MethodStmt *>> class_methods; /* class to methods */
static map<string, set<AttrStmt *>> class_attrs;     /* class to attrs */
/* intermediate map from class name to parent meths */
static map<string, set<MethodStmt *>> parent_methods;
/* intermediate map from class name to its parent attrs */
static map<string, set<AttrStmt *>> parent_attrs;
static set<MethodStmt *> global_methods; /* set of global methods */

static bool conforms(Type_ *a, Type_ *b);
static void populate_parent_feature_tables(string &child,
                                           map<string, bool> &visited);
static bool check_valid_type_(Type_ *t);
bool check_method_sigs(MethodStmt *p, MethodStmt *c, const string &cname);

static void error(int lineno, const std::string &err_msg) {
  Error e(SEMANTIC_ERROR, curr_filename, lineno, err_msg);
  e.print();
  semant_errors++;
}

static void warning(int lineno, const std::string &warn_msg) {
  Warning w(SEMANTIC_ERROR, curr_filename, lineno, warn_msg);
  w.print();
  warnings++;
}

//////////////////////////////////////////////////////////////
//
// Initializing
//
//////////////////////////////////////////////////////////////

void TypeChecker::initialize_basic_classes() {
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
  int ->
    parent: object
    no member attrs/methods
  */
  class_type[Int] = new Type_(Int, NULL);
  class_parents[Int].push_back(Object);
  class_names.push_back(Int);
  classes[Int] = new ClassStmt(Int, {Object}, {});

  /*
  deci ->
    parent: object
    no member attr/methods
  */
  class_type[Deci] = new Type_(Deci, NULL);
  class_parents[Deci].push_back(Object);
  class_names.push_back(Deci);
  classes[Deci] = new ClassStmt(Deci, {Object}, {});

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
      string[(int n):(int m)] -- returns string from index n inclusive to m
  exclusive
        * m >= n
        * if n is not present --> n = 0
        * if m is not present --> m = string.length()
  */
  class_type[String] = new Type_(String, NULL);
  class_parents[String].push_back(Object);
  set<MethodStmt *> string_methods = {
      new MethodStmt(class_type[Int], Length, {}, {}),
      new MethodStmt(class_type[Void], Clear, {}, {}),
      new MethodStmt(class_type[Bool], Is_Empty, {}, {}),
      new MethodStmt(class_type[Char], Front, {}, {}),
      new MethodStmt(class_type[Char], Back, {}, {})};
  class_methods[String] = string_methods;
  class_names.push_back(String);
  FeatureList string_features;
  for (MethodStmt *m : string_methods) {
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
    object front() -- returns first object (Runtime error on empty)
    object back() -- returns last object
    int contains(object o) -- returns idx if o in list, else -1
    binary operators -- in the form `list1 op list2`
      acceptable ops: +, +=, ==, !=

    bracket operations:
      list[int n] -- returns object at index n
      list[(int n): (int m)] -- returns list from index n inclusive to m
  exclusive
        * m >= n
        * if n is not present --> n = 0
        * if m is not present --> m = list.length()
  */
  class_type[List] = new Type_(List, class_type[Object]);
  class_parents[List].push_back(Object);
  set<MethodStmt *> list_methods = {
      new MethodStmt(class_type[Int], Length, {}, {}),
      new MethodStmt(class_type[Void], Clear, {}, {}),
      new MethodStmt(class_type[Bool], Is_Empty, {}, {}),
      new MethodStmt(class_type[Void], Push_Back,
                     {new FormalStmt(class_type[Object], Object)}, {}),
      new MethodStmt(class_type[Void], Push_Front,
                     {new FormalStmt(class_type[Object], Object)}, {}),
      new MethodStmt(class_type[Void], Pop_Front, {}, {}),
      new MethodStmt(class_type[Void], Pop_Back, {}, {}),
      new MethodStmt(class_type[Object], Front, {}, {}),
      new MethodStmt(class_type[Object], Back, {}, {}),
      new MethodStmt(class_type[Int], Contains,
                     {new FormalStmt(class_type[Object], Object)}, {})};
  class_methods[List] = list_methods;
  class_names.push_back(List);
  FeatureList list_features;
  for (MethodStmt *m : list_methods) {
    list_features.push_back(m);
  }
  classes[List] = new ClassStmt(List, {Object}, list_features);

  /*
  set<object> -->
    parent: object
    int length() -- returns num objects in set
    void clear() -- empties the set
    bool is_empty() -- returns true if set is empty, false otherwise
    void insert(object o) -- inserts o into set
    bool remove(object o) -- removes o if o in set,
      returns true if removed, else false
    bool contains(object o) -- returns true if o in set binary
  operators -- in the form `set1` op `set2`
    acceptable ops: +, -, ==, !=
  */
  class_type[Set] = new Type_(Set, class_type[Object]);
  class_parents[Set].push_back(Object);
  set<MethodStmt *> set_methods = {
      new MethodStmt(class_type[Int], Length, {}, {}),
      new MethodStmt(class_type[Void], Clear, {}, {}),
      new MethodStmt(class_type[Bool], Is_Empty, {}, {}),
      new MethodStmt(class_type[Void], Insert,
                     {new FormalStmt(class_type[Object], Object)}, {}),
      new MethodStmt(class_type[Bool], Remove,
                     {new FormalStmt(class_type[Object], Object)}, {}),
      new MethodStmt(class_type[Bool], Contains,
                     {new FormalStmt(class_type[Object], Object)}, {}),
  };
  class_methods[Set] = set_methods;
  class_names.push_back(Set);
  FeatureList set_features;
  for (MethodStmt *m : set_methods) {
    set_features.push_back(m);
  }
  classes[Set] = new ClassStmt(Set, {Object}, set_features);
}

/* Returns false if T is invalid. ex: list, char<string> are invalid */
bool check_valid_type_(Type_ *t) {
  if (!t) return false;

  Type_ *nested = t->get_nested_type();
  bool is_container = (t->get_name() == List || t->get_name() == Set);

  if (is_container) {
    return nested && check_valid_type_(nested);
  }

  return !nested;
}

/* defines KrutC builtin methods */
void TypeChecker::initialize_builtin_methods() {
  /*
  void print(string s); -- prints s to console
  string to_str(int i); -- returns string representation of i
  object type(object o); -- returns type of object
  int abs(int x); -- returns abs value of x
  int sum(list<object> l); -- returns sum of l
  int min(list<object> l); -- returns min val of l
  int max(list<object> l); -- returns max val of l
  string input(string prompt); -- returns input from console/terminal

  TODO:
    deci (decimal type), int-->dec & dec-->int
    file object?? how does that work?

  */
  global_methods.insert(new MethodStmt(
      class_type[Void], Print, {new FormalStmt(class_type[String], "s")}, {}));
  global_methods.insert(
      new MethodStmt(class_type[String], Input,
                     {new FormalStmt(class_type[String], "prompt")}, {}));
  global_methods.insert(new MethodStmt(class_type[String], To_String,
                                       {new FormalStmt(class_type[Int], "i")},
                                       {}));
  global_methods.insert(
      new MethodStmt(class_type[Object], Type_Of,
                     {new FormalStmt(class_type[Object], "o")}, {}));
  global_methods.insert(new MethodStmt(
      class_type[Int], Abs, {new FormalStmt(class_type[Int], "x")}, {}));
  global_methods.insert(new MethodStmt(
      class_type[Int], Sum, {new FormalStmt(class_type[List], "l")}, {}));
  global_methods.insert(new MethodStmt(
      class_type[Int], Min, {new FormalStmt(class_type[List], "l")}, {}));
  global_methods.insert(new MethodStmt(
      class_type[Int], Max, {new FormalStmt(class_type[List], "l")}, {}));
  global_methods.insert(
      new MethodStmt(class_type[Void], Kill,
                     {new FormalStmt(class_type[String], "err_msg")}, {}));
}

/* Declared classes (of the form class ClassName {...}) can only be decalared in
 * outer scope */
void TypeChecker::initialize_declared_classes() {
  for (int i = 0; i < program.len(); i++) {
    Stmt *s = program.ith(i);

    ClassStmt *cs = dynamic_cast<ClassStmt *>(s);
    if (!cs) continue;
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
    for (Feature *f : feature_list) {
      if (f->is_method()) {
        MethodStmt *m = static_cast<MethodStmt *>(f);
        if (m->get_name() == Constructor) {
          string err_msg = "Redefined constructor in class " + name +
                           ", or method named `constructor`. A method "
                           "`constructor` is builtin for every class.";
          error(m->lineno, err_msg);
        }
        set<MethodStmt *> *existing_methods = &class_methods[cs->get_name()];
        for (MethodStmt *em : *existing_methods) {
          // check current method against existing methods
          if (em->get_name() == m->get_name()) {
            string err_msg = "Method " + m->get_name() +
                             " cannot be defined twice in class " +
                             cs->get_name();
            error(m->lineno, err_msg);
            continue;
          }
        }
        existing_methods->insert(
            m);  // this should insert it in class_methods bc ptr

        if (!check_valid_type_(m->get_ret_type())) {
          string err_msg = "Method `" + m->get_name() +
                           "` has invalid return type `" +
                           m->get_ret_type()->to_str() + "`";
          error(m->lineno, err_msg);
        }

        FormalList formal_list = m->get_formal_list();
        for (FormalStmt *f : formal_list) {
          if (!check_valid_type_(f->get_type())) {
            string err_msg = "Formal `" + f->get_name() +
                             "` has invalid type `" + f->get_type()->to_str() +
                             "`";
            error(f->lineno, err_msg);
          }
        }
      } else {
        AttrStmt *a = static_cast<AttrStmt *>(f);
        set<AttrStmt *> *existing_attrs = &class_attrs[cs->get_name()];
        for (AttrStmt *ea : *existing_attrs) {
          if (ea->get_name() == a->get_name()) {
            string err_msg = "Attribute " + a->get_name() +
                             " cannot be defined twice in class " +
                             cs->get_name();
            error(a->lineno, err_msg);
            continue;
          }
        }
        if (!check_valid_type_(a->get_type())) {
          string err_msg = "Attribute `" + a->get_name() + "` in class `" +
                           name + "` has invalid type `" +
                           a->get_type()->to_str() + "`";
          error(a->lineno, err_msg);
        }
        existing_attrs->insert(
            a);  // this should insert it in class_attrs bc ptr
      }
    }
  }
}

/* check that parents arent repeating and are not from base classes */
void TypeChecker::check_valid_class_parents() {
  for (int i = 0; i < program.len(); i++) {
    Stmt *s = program.ith(i);

    ClassStmt *cs = dynamic_cast<ClassStmt *>(s);
    if (!cs) continue;

    string name = cs->get_name();
    vector<string> parents = cs->get_parents();
    for (string parent : parents) {
      if (class_type.find(parent) == class_type.end()) {
        // unknown parent
        string err_msg =
            "Class " + name + " inheriting from unknown class " + parent;
        error(cs->lineno, err_msg);
        /* erase bad parents so we can typecheck as if they have good parents */
        parents.erase(remove(parents.begin(), parents.end(), parent),
                      parents.end());
        continue;
      }
      if (basic_classes::is_basic_class(parent)) {
        string err_msg =
            "Class " + name + " cannot inherit from base class " + parent;
        error(cs->lineno, err_msg);
        /* erase bad parents so we can typecheck as if they have good parents */
        parents.erase(remove(parents.begin(), parents.end(), parent),
                      parents.end());
        continue;
      }
    }
    if (parents.empty()) parents.push_back(Object);
    class_parents[name] = parents;
  }
}

/* prints class_parents */
void print_class_parents() {
  for (pair<string, vector<string>> clp : class_parents) {
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
    for (vector<string> &cycle : cycles) {
      string err_msg = "Cycle detected in classes " + cycle[0];
      for (int i = 1; i < (int)cycle.size(); i++) {
        err_msg += ", " + cycle[i];
      }
      error(0, err_msg);
    }
    return false;
  }
  return true;
}

/* DFS to populate a set of meths/attrs that is the intersection of the child's
 * parent's meths/attrs */
void populate_parent_feature_tables(string &child, map<string, bool> &visited) {
  visited[child] = true;

  for (string &parent : class_parents[child]) {
    if (!visited[parent]) {
      populate_parent_feature_tables(parent, visited);
    }
    for (MethodStmt *nmeth : class_methods[parent]) {
      bool can_insert = true;
      for (MethodStmt *emeth : parent_methods[child]) {
        if (emeth->get_name() == nmeth->get_name()) {
          can_insert = false;
          break;
        }
      }
      if (can_insert) {
        parent_methods[child].insert(nmeth);
      }
    }

    for (AttrStmt *nattr : class_attrs[parent]) {
      bool can_insert = true;
      for (AttrStmt *eattr : parent_attrs[child]) {
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

/* Returns true if return type and formal list are consistent between inherited
 * methods, otherwise false */
bool check_method_sigs(MethodStmt *p, MethodStmt *c, const string &cname) {
  bool ret_val = true;
  if (!conforms(c->get_ret_type(), p->get_ret_type())) {
    string err_msg =
        "Return type `" + c->get_ret_type()->to_str() +
        "`";  //  of method `" + c->get_name() + "` in class `" + cname + "`";
    err_msg += " does not conform to inherited return type `" +
               p->get_ret_type()->to_str() + "` defined on line " +
               to_string(p->lineno);
    error(c->lineno, err_msg);
    ret_val = false;
  }

  return ret_val;
}

set<string> get_ancestors(string t) {
  set<string> ancs;
  deque<string> frontier;
  frontier.push_front(t);

  while (!frontier.empty()) {
    string curr = frontier.front();
    frontier.pop_front();

    for (const string &parent : class_parents[curr]) {
      if (ancs.find(parent) == ancs.end()) {
        ancs.insert(parent);
        frontier.push_back(parent);
      }
    }
  }

  return ancs;
}

void TypeChecker::populate_class_ancestors() {
  for (const string &name : class_names) {
    class_ancestors[name] = get_ancestors(name);
  }
}

void TypeChecker::populate_feature_tables() {
  map<string, bool> visited;
  for (pair<string, vector<string>> entry : class_parents) {
    if (!visited[entry.first]) {
      populate_parent_feature_tables(entry.first, visited);
    }
  }

  for (const string &cname : class_names) {
    /* for every class */
    for (MethodStmt *pmeth : parent_methods[cname]) {
      bool can_insert = true;
      for (MethodStmt *cmeth : class_methods[cname]) {
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

    for (AttrStmt *pattr : parent_attrs[cname]) {
      bool can_insert = true;
      for (AttrStmt *cattr : class_attrs[cname]) {
        if (pattr->get_name() == cattr->get_name()) {
          string err_msg = "Class " + cname +
                           " cannot redefine inherited attribute `" +
                           cattr->get_name() + "` defined by parent on line " +
                           to_string(pattr->lineno);
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
    Feature *f = dynamic_cast<Feature *>(s);
    if (!f) continue;

    if (f->is_method()) {
      MethodStmt *m = static_cast<MethodStmt *>(f);
      /* check return type is valid */
      if (!check_valid_type_(m->get_ret_type())) {
        string err_msg = "Method `" + m->get_name() +
                         "` has invalid return type `" +
                         m->get_ret_type()->to_str() + "`";
        error(m->lineno, err_msg);
        ret_val = false;
      }

      /* check formal type is valid */
      FormalList formal_list = m->get_formal_list();
      for (FormalStmt *f : formal_list) {
        if (!check_valid_type_(f->get_type())) {
          string err_msg = "Formal `" + f->get_name() + "` has invalid type `" +
                           f->get_type()->to_str() + "`";
          error(f->lineno, err_msg);
          ret_val = false;
        }
      }

      /* check if method is already defined */
      bool can_insert = true;
      for (MethodStmt *gm : global_methods) {
        if (gm->get_name() == m->get_name()) {
          string err_msg =
              "method `" + m->get_name() + "` cannot be defined twice";
          error(m->lineno, err_msg);
          ret_val = false;
          can_insert = false;
        }
      }
      if (can_insert) {
        if (m->get_name() == Main) {
          if (!conforms(class_type[Void], m->get_ret_type())) {
            string err_msg = "method 'main' must return void";
            error(m->lineno, err_msg);
          }
          if (formal_list.size() != 1) {
            string err_msg =
                "method 'main' must have one argument: list<string> args";
            error(m->lineno, err_msg);
          } else {
            FormalStmt *f = formal_list[0];
            Type_ *ft = f->get_type();
            if (!conforms(ft, new Type_(List, class_type[String]))) {
              string err_msg =
                  "method 'main' must have one argument: list<string> args";
              error(m->lineno, err_msg);
            }
          }
        }
        global_methods.insert(m);
      }
    } else {
      AttrStmt *a = static_cast<AttrStmt *>(f);
      if (!check_valid_type_(a->get_type())) {
        string err_msg = "Attribute `" + a->get_name() +
                         "` has invalid type `" + a->get_type()->to_str() + "`";
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
  initialize_builtin_methods();
  initialize_declared_classes();
  check_valid_class_parents();

  if (!check_inheritance_cycles()) {
    /* cannot continue typechecking if there are inheritance cycles */
    return semant_errors++;
  }

  populate_class_ancestors();

  populate_feature_tables();

  if (!check_global_features()) {
    /* cannot continue typechecking if global features have type declaration
     * errors */
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
bool conforms_util(const string &a, const string &b) {
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

    for (string &parent : class_parents[curr]) {
      if (visited.find(parent) == visited.end()) {
        frontier.push(parent);
      }
    }
  }
  return false;
}

/* Given two ptrs to Type_ objects, returns true if A conforms to B, false
 * otherwise. */
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
  } else if (a_nest == NULL && b_nest == NULL)
    return true;
  // if (a_nest != NULL && b_nest != NULL)
  else
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

static bool in_method = false; /* to ensure ReturnExpr* only occur in methods */
static MethodStmt *curr_method; /* the current method being typechecked */
static int curr_method_num_nested_rex = 0;

Type_ *MethodStmt::typecheck() {
  scopetable.push_scope();
  in_method = true;
  curr_method = this;
  /* add all formals */
  for (FormalStmt *f : formal_list) {
    f->typecheck();
  }

  /*
  for return expressions in methods:

    if a method's return value is `void`:
      * it does not need to return on any control flow paths
      * it can return using expression like this: `return;`
      * if it returns a value other than void, a warning is generated

    if a method's return value is not `void`:
      * it must return on all control flow paths:
        a method contains a statement list
        either:
          one of those statements is a return expression
        or:
          if none of them are, then there must be a return statement
          nested in all of the statements
  */
  bool contains_ret_expr = false;
  for (int i = 0; i < (int)stmt_list.size(); i++) {
    Stmt *s = stmt_list[i];

    if (contains_ret_expr) {
      string warn_msg = "Any code after return expression is dead code";
      warning(s->lineno, warn_msg);
      /* no need to keep typechecking */
      break;
    }

    if (dynamic_cast<MethodStmt *>(s)) {
      string err_msg = "Cannot nest method definitions";
      error(s->lineno, err_msg);
      continue;
    }

    s->typecheck();

    if (dynamic_cast<IfStmt *>(s)) {
      curr_method_num_nested_rex--;
    }

    if (dynamic_cast<ReturnExpr *>(s)) {
      contains_ret_expr = true;
    }
  }

  if (!contains_ret_expr) {
    if (curr_method_num_nested_rex != stmt_list.size() ||
        stmt_list.size() == 0) {
      string err_msg =
          "Method `" + name + "` does not return in all control paths";
      error(lineno, err_msg);
    }
  }

  in_method = false;
  curr_method_num_nested_rex = 0;
  curr_method = NULL;
  scopetable.pop_scope();
  return NULL;
}

Type_ *IfStmt::typecheck() {
  Type_ *pred_type = pred->typecheck();
  if (pred_type && !conforms(pred_type, class_type[Bool])) {
    string err_msg =
        "Expected bool in predicate, instead got `" + pred_type->to_str() + "`";
    error(pred->lineno, err_msg);
  }

  for (Stmt *s : then_branch) {
    s->typecheck();
  }

  for (Stmt *s : else_branch) {
    s->typecheck();
  }

  return NULL;
}

Type_ *ClassStmt::typecheck() {
  scopetable.push_scope();

  for (Feature *f : feature_list) {
    if (f->is_method()) {
      MethodStmt *m = static_cast<MethodStmt *>(f);
      m->typecheck();
    } else {
      AttrStmt *a = static_cast<AttrStmt *>(f);
      a->typecheck();
    }
  }

  scopetable.pop_scope();

  return NULL;
}

Type_ *ForStmt::typecheck() {
  if (stmt) stmt->typecheck();

  if (cond) {
    Type_ *c_type = cond->typecheck();
    if (!conforms(class_type[Bool], c_type)) {
      string err_str = "For loop conditional must be of type Bool";
      error(cond->lineno, err_str);
    }
  }

  if (repeat) repeat->typecheck();

  for (Stmt *s : stmt_list) {
    s->typecheck();
  }

  return NULL;
}

/* checks if init expr type conforms to declared, adds to scopetable */
Type_ *AttrStmt::typecheck() {
  Type_ *init_type;

  if (conforms(type, class_type[Void])) {
    /* variable cannot be void */
    string err_msg = "Variable " + name + " cannot be declared as Void";
    error(lineno, err_msg);
    goto done;
  }

  if (!class_type[type->get_name()]) {
    /* unknown type */
    string err_msg = "Undefined type `" + type->get_name() + "`";
    error(lineno, err_msg);
    goto done;
  }

  if (!init) goto done;

  init_type = init->typecheck();
  if (!init_type) {
    /* if there's no initializing of variable */
    goto done;
  }

  if (init_type && !conforms(init_type, type)) {
    string err_msg = "Variable `" + name + "` (declared as " + type->to_str() +
                     ") is initialized to a value of type " +
                     init_type->to_str();
    error(lineno, err_msg);
  }

done:
  /* give declared type precedence on error, else they are the same */
  scopetable.add_elem(name, type);
  return NULL;
}

Type_ *WhileStmt::typecheck() {
  Type_ *pred_t = pred->typecheck();

  if (!conforms(class_type[Bool], pred_t)) {
    string err_str = "Predicate in while loop must be of type bool";
    error(pred->lineno, err_str);
  }

  for (Stmt *s : stmt_list) {
    s->typecheck();
  }

  return NULL;
}

Type_ *BreakStmt::typecheck() { return NULL; }
Type_ *ContStmt::typecheck() { return NULL; }

//////////////////////////////////////////////////////////////
//
//
// Expression Typechecking
//
//
//////////////////////////////////////////////////////////////

/* this checks the declared return type with the actual return type */
void check_rex(Type_ *ret_type_actual, int lineno) {
  assert(in_method);
  Type_ *ret_type_decl = curr_method->get_ret_type();
  if (conforms(ret_type_decl, class_type[Void])) {
    // if return type of method is void
    if (!conforms(ret_type_actual, class_type[Void])) {
      // if return value is not void
      string warn_msg = "Void method `" + curr_method->get_name() +
                        "` may return `" + ret_type_actual->to_str() + "`";
      warning(lineno, warn_msg);
    }
  } else {
    // return type is not void
    if (!ret_type_actual) {
      // but returning void value
      string err_msg = "method `" + curr_method->get_name() +
                       "` has return type of " + ret_type_decl->to_str() +
                       " and is returning a `void` value";
      error(lineno, err_msg);
    } else if (!conforms(ret_type_actual, ret_type_decl)) {
      // returning wrong type
      string err_msg = "method `" + curr_method->get_name() +
                       "` is returning a `" + ret_type_actual->to_str() +
                       "` when its declared return type is `" +
                       ret_type_decl->to_str() + "`";
      error(lineno, err_msg);
    }
  }
}

Type_ *ReturnExpr::typecheck() {
  Type_ *t = NULL;
  if (expr) t = expr->typecheck();

  if (!in_method) {
    string warn_msg = "Return expression outside of method has no function";
    warning(lineno, warn_msg);
  } else {
    check_rex(t, lineno);
    curr_method_num_nested_rex++;
  }
  /* possible that RETURN has no expression */
  return t;
}

Type_ *ListElemRef::typecheck() {
  /* should return T = list<object> */
  Type_ *index_type = index->typecheck();

  if (!conforms(index_type, class_type[Int])) {
    string err_msg =
        "When indexing a list, the index expression must be of type Int";
    error(lineno, err_msg);
  }

  Type_ *name_type = list_name->typecheck();
  if (!name_type) return NULL;

  return name_type->get_nested_type();
}

Type_ *SublistExpr::typecheck() {
  ExprStmt *st_idx = get_st_idx();
  Type_ *st_idx_type_;
  if (st_idx) {
    st_idx_type_ = st_idx->typecheck();
  } else {
    st_idx_type_ = NULL;
  }

  ExprStmt *end_idx = get_end_idx();
  Type_ *end_idx_type_;
  if (end_idx) {
    end_idx_type_ = end_idx->typecheck();
  } else {
    end_idx_type_ = NULL;
  }

  if ((st_idx_type_ && !conforms(st_idx_type_, class_type[Int])) ||
      (end_idx_type_ && !conforms(end_idx_type_, class_type[Int]))) {
    string err_msg =
        "When creating sublist list[a:b], both a and b must be ints";
    error(lineno, err_msg);
  }

  // if (!conforms(end_idx_t, class_type[Int])) {
  //   string err_msg = "When creating sublist l[a:b], both a and b must be
  //   ints"; error(lineno, err_msg);
  // }

  Type_ *name_type = get_list_name()->typecheck();
  if (!name_type) {
    return NULL;
  }

  return name_type;
}

/* when dispatching to methods of nested classes like lists, the 'matched'
   existing method needs to be de-generalized from object to the type of the
   call. For example, without this funciton, code like this is allowed:

   list<int> my_list = [1, 2, 3];
   my_list.push_back("string");

   this function returns a new MethodStmt *, where the argument would be
   of type `int`, so that a `string` would result in an error

*/
MethodStmt *update_cmp_meth(Type_ *calling_type, MethodStmt *orig) {
  Type_ *ret_type;
  string name;
  FormalList formal_list;

  Type_ *nested_type = calling_type->get_nested_type();
  if (conforms(class_type[Object], orig->get_ret_type())) {
    ret_type = nested_type;
  } else {
    ret_type = orig->get_ret_type();
  }

  name = orig->get_name();
  FormalList orig_formal_list = orig->get_formal_list();

  for (FormalStmt *f : orig_formal_list) {
    FormalStmt *new_f;
    if (conforms(class_type[Object], f->get_type())) {
      new_f = new FormalStmt(nested_type, Object);
    } else {
      new_f = f;
    }
    formal_list.push_back(new_f);
  }

  MethodStmt *m = new MethodStmt(ret_type, name, formal_list, {});
  return m;
}

Type_ *DispatchExpr::typecheck() {
  MethodStmt *cmp_meth;
  Type_ *calling_type;
  bool exists = false;
  if (calling_expr) {
    calling_type = calling_expr->typecheck();
    // check method exists for calling type
    string class_name = calling_type->get_name();
    set<MethodStmt *> &meths = class_methods[class_name];
    for (MethodStmt *m : meths) {
      if (m->get_name() == name) {
        exists = true;
        cmp_meth = m;
        break;
      }
    }
    if (!exists) {
      string err_msg = "Class `" + class_type[class_name]->to_str() +
                       "` has no method " + name + "`";
      error(lineno, err_msg);
    }
  } else {
    // check global methods
    for (MethodStmt *m : global_methods) {
      if (m->get_name() == name) {
        exists = true;
        cmp_meth = m;
        break;
      }
    }
    if (!exists) {
      string err_msg = "Method `" + name + "` does not exist";
      error(lineno, err_msg);
    }
  }
  if (exists) {
    if (calling_type->get_nested_type()) {
      cmp_meth = update_cmp_meth(calling_type, cmp_meth);
    }

    FormalList fl = cmp_meth->get_formal_list();

    /* check num args and num params are equal */
    if (args.size() != fl.size()) {
      string err_msg = "Dispatch of method `" + name + "` requires " +
                       to_string(fl.size()) + " arg(s), you provided " +
                       to_string(args.size()) + " arg(s)";
      error(lineno, err_msg);
    } else {
      for (int i = 0; i < (int)args.size(); i++) {
        ExprStmt *arg = args[i];
        Type_ *arg_type = arg->typecheck();
        if (i > (int)fl.size() - 1) {
          string warn_msg = "Method " + cmp_meth->get_name() + " has " +
                            to_string(fl.size()) +
                            " arguments, so any more will be ignored.";
          warning(lineno, warn_msg);
        } else if (!conforms(arg_type, fl[i]->get_type())) {
          string suffix;
          /* this is pretty humor lmao */
          switch ((i + 1) % 10) {
            case 1:
              suffix = i == 11 ? "th" : "st";
              break;
            case 2:
              suffix = i == 12 ? "th" : "nd";
              break;
            case 3:
              suffix = i == 13 ? "th" : "rd";
              break;
            default:
              suffix = "th";
              break;
          }
          string err_msg = "The " + to_string(i + 1) + suffix +
                           " argument of function call `" + name +
                           "()` needs to be of type `" +
                           fl[i]->get_type()->to_str() + "` instead of type `" +
                           arg_type->to_str() + "`";
          error(lineno, err_msg);
        }
      }
    }
  }
  return cmp_meth->get_ret_type();
}

Type_ *IntConstExpr::typecheck() { return class_type[Int]; }

Type_ *DeciConstExpr::typecheck() { return class_type[Deci]; }

Type_ *ObjectIdExpr::typecheck() {
  Type_ *type_ = scopetable.lookup(name);
  if (!type_) {
    string err_msg = "Unknown variable: `" + name + "`";
    error(lineno, err_msg);
    return NULL;
  }
  return type_;
}

Type_ *BoolConstExpr::typecheck() { return class_type[Bool]; }

Type_ *lub(Type_ *a, Type_ *b) {
  if (!a || !b) {
    return class_type[Object];
  }

  if (a->get_name() == b->get_name()) {
    if (!a->get_nested_type() && !b->get_nested_type()) {
      return a;
    }
    return lub(a->get_nested_type(), b->get_nested_type());
  }

  set<string> a_ancs = class_ancestors[a->get_name()];
  set<string> b_ancs = class_ancestors[b->get_name()];

  for (const string &aa : a_ancs) {
    if (b_ancs.find(aa) != b_ancs.end()) {
      return class_type[aa];
    }
  }

  return class_type[Object];
}

Type_ *SetConstExpr::typecheck() {
  // TODO: !!!
  return NULL;
}

Type_ *ListConstExpr::typecheck() {
  if (exprlist.empty()) {
    // return NULL if empty list
    return NULL;
  }

  Type_ *lca = exprlist[0]->typecheck();
  for (int i = 1; i < (int)exprlist.size(); i++) {
    Type_ *curr_t = exprlist[i]->typecheck();
    if (conforms(curr_t, lca)) {
      continue;
    }
    lca = lub(lca, curr_t);
    if (conforms(class_type[Object], lca)) {
      break;
    }
  }

  return new Type_(List, lca);
}

Type_ *StrConstExpr::typecheck() { return class_type[String]; }

Type_ *CharConstExpr::typecheck() { return class_type[Char]; }

Type_ *NewExpr::typecheck() {
  Type_ *type_ = class_type[newclass];
  if (!type_) {
    string err_msg = "Unknown type " + newclass + " in new expression";
    error(lineno, err_msg);
    return NULL;
  }
  return class_type[newclass];
}

Type_ *BinopExpr::typecheck() {
  Type_ *lhs_type = lhs->typecheck();
  Type_ *rhs_type = rhs->typecheck();

  if (BINOP_PRECEDENCE[op] >= 4) {
    /* ensuring that any +=, -=, *=, /=, = ops
       cannot be done on any const expr */
    string classname = lhs->classname();
    if (classname.find("Const") != std::string::npos) {
      string err_msg =
          "Left side of operator `" + op + "` cannot be a constant value";
      error(lineno, err_msg);
    }
  }

  if (!conforms(lhs_type, rhs_type)) {
    /* if int and deci in same operation ==> deci */
    if (((conforms(lhs_type, class_type[Int]) &&
          conforms(rhs_type, class_type[Deci]))) ||
        (conforms(lhs_type, class_type[Deci]) &&
         conforms(rhs_type, class_type[Int]))) {
      string warn_msg = "Operation `" + op +
                        "` done on `int` and `deci` will evaluate to `deci`";
      warning(lineno, warn_msg);
      lhs_type = class_type[Deci]; /* setting the return type */
    } else {
      string err_msg =
          "Left side type (" + lhs_type->to_str() + ") of operator `" + op +
          "` is not the same as right side type (" + rhs_type->to_str() + ")";
      error(lineno, err_msg);
    }
  }

  if (BINOP_PRECEDENCE[op] == 2 || BINOP_PRECEDENCE[op] == 3) {
    /* any of the comparing operators:
      <, >, <=, >=, ==, !=, &&, ||
    */
    return class_type[Bool];
  }

  return lhs_type;
}
