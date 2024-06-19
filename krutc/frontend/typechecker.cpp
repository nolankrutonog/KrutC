#include <iostream>
#include <cassert>
#include <map>
#include <set>
#include <bitset>
#include "include/typechecker.h"
#include "include/scopetable.h"

using namespace std;

/* basic object names */
#define Void "void"
#define Object "object"
#define Int "int"
#define Bool "bool"
#define Char "char"
#define String "string"
#define List "list"
#define Stack "stack"

/* basic classes methods */
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

static string curr_filename;
static int semant_errors = 0;
static ScopeTable scopetable;

// static set<Type_*> classes; // defines the basic classes Type_ expressions
static map<string, Type_*> class_type;
static map<string, ClassStmt*> classes;
static map<string, vector<string>> class_parents;
static map<string, int> class_tags;
static int next_class_tag = 0;
static map<string, set<MethodStmt*>> class_methods;
static map<string, set<AttrStmt*>> class_attrs;

static set<MethodStmt*> global_methods;

void error(int lineno, std::string err_msg) {
  semant_errors++;
  std::cout << curr_filename + ":" + std::to_string(lineno) + ": ";
  std::cout << err_msg << std::endl;
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
  class_tags[Void] = next_class_tag++;
  // classes[Void] = new ClassStmt(Void, {}, {});

  /*
  object -> 
    parent: None
    no member attrs/methods
  */
  class_type[Object] = new Type_(Object, NULL);
  class_parents[Object] = {};
  class_tags[Object] = next_class_tag++;
  // classes[Object] = new ClassStmt(Object, {}, {});

  /*
  bool -> 
    parent: object
    no member attrs/methods
  */
  class_type[Bool] = new Type_(Bool, NULL);
  class_parents[Bool].push_back(Object);
  class_tags[Bool] = next_class_tag++;
  // classes[Bool] = new ClassStmt(Bool, {Object}, {});
  
  /*
  int -> 
    parent: object
    no member attrs/methods
  */
  class_type[Int] = new Type_(Bool, NULL);
  class_parents[Int].push_back(Object);
  class_tags[Int] = next_class_tag++;
  // classes[Int] = new ClassStmt(Int, {Object}, {});

  /*
  char ->
    parent: object
    no member attrs/methods
  */
  class_type[Char] = new Type_(Char, NULL);
  class_parents[Char].push_back(Object);
  class_tags[Char] = next_class_tag++;
  // classes[Char] = new ClassStmt(Char, {Object}, {});

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
  class_tags[String] = next_class_tag++;
  // classes[String] = new ClassStmt(String, {Object}, string_methods);
  
  /*
  list<object> ->
    parent: object
    int length() -- returns num objects in list
    void clear() -- empties the list
    bool is_empty() -- returns true on length == 0, false otherwise
    void push_back(object o) -- adds object to end
    void push_front(object o) -- adds object to front
    object front() -- returns first object (None on empty)
    object back() -- returns last object
    void pop_front() -- pops first object
    void pop_back() -- pops last object
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
    new MethodStmt(class_type[Object], Back, {}, {})
  };
  class_methods[List] = list_methods; 
  class_tags[List] = next_class_tag++;

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
  class_tags[Stack] = next_class_tag++;

}

/* Declared classes (of the form class ClassName {...}) can only be decalared in outer scope */
void TypeChecker::initialize_declared_classes() {
  for (int i = 0; i < program.len(); i++) {
    Stmt *s = program.ith(i);

    ClassStmt *cs = dynamic_cast<ClassStmt*>(s);
    if (!cs)
      continue;
    // if class defined twice
    if (class_type.find(cs->get_name()) != class_type.end()) {
      string err_msg = "Error: Class " + cs->get_name() + " cannot be defined twice.";
      error(cs->lineno, err_msg);
      continue;
    }
    class_type[cs->get_name()] = new Type_(cs->get_name(), NULL);
    class_tags[cs->get_name()] = next_class_tag++;

    class_parents[cs->get_name()] = cs->get_parents();
    /* object is parent of every declared class */
    // class_parents[cs->get_name()].push_back(Object);


    FeatureList feature_list = cs->get_feature_list();
    for (Feature *f: feature_list) {
      if (f->is_method()) {
        MethodStmt *m = static_cast<MethodStmt*>(f);
        set<MethodStmt*> *existing_methods = &class_methods[cs->get_name()];
        for (MethodStmt *em: *existing_methods) {
          // check current method against existing methods
          if (em->get_name() == m->get_name()) {
            string err_msg = "Error: Method " + m->get_name() + " cannot be defined twice in class " + cs->get_name();
            error(m->lineno, err_msg);
            continue;
          }
        }
        existing_methods->insert(m); // should insert it in class_methods bc ptr

      } else {
        AttrStmt *a = static_cast<AttrStmt*>(f);
        set<AttrStmt*> *existing_attrs = &class_attrs[cs->get_name()];
        for (AttrStmt *ea: *existing_attrs) {
          if (ea->get_name() == a->get_name()) {
            string err_msg = "Error: Attribute " + a->get_name() + " cannot be defined twice in class " + cs->get_name();
            error(a->lineno, err_msg);
            continue;
          }
        }
        existing_attrs->insert(a); // should insert it in class_attrs bc ptr
      }
    }
  }
  check_declared_classes_parents();
}

/* make sure parents are real and are not basic classes. Has to be done after
   collecting all classes because they are global
*/
void TypeChecker::check_declared_classes_parents() {
  for (int i = 0; i < program.len(); i++) {
    /* have to iterate through program rather than classes to get lineno */
    ClassStmt *cs = dynamic_cast<ClassStmt*>(program.ith(i));
    if (!cs)
      continue;
    string name = cs->get_name();
    vector<string> parents = class_parents[name];
    for (string parent: parents) {
      if (class_type.find(parent) == class_type.end()) {
        // unknown parent
        string err_msg = "Error: Class " + name + " inheriting from unknown class " + parent;
        error(cs->lineno, err_msg);
        /* erase bad parents so we can typecheck as if they have good parents */
        class_parents[name].erase(remove(class_parents[name].begin(), class_parents[name].end(), parent), class_parents[name].end());
        continue;
      }
      if (basic_classes.find(parent) != basic_classes.end()) {
        string err_msg = "Error: Class " + name + " cannot inherit from base class " + parent;
        error(cs->lineno, err_msg);
        /* erase bad parents so we can typecheck as if they have good parents */
        class_parents[name].erase(remove(class_parents[name].begin(), class_parents[name].end(), parent), class_parents[name].end());
        continue;
      }
    }
    class_parents[cs->get_name()].push_back(Object);
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
  Creates graph where nodes are classes and edges are from children to parents. 
  Returns false if contains inheritance cycles, true otherwise. 
*/
bool TypeChecker::check_inheritance_cycles() {

  /* create graph 
     nodes --> classes
     edges --> from children to parents 
  */
  Graph graph = Graph();

  for (pair<string, vector<string>> cp: class_parents) {
    /* build graph */
    string curr_class = cp.first;
    graph.add_node(curr_class);
    for (string parent: cp.second) {
      graph.add_edge(curr_class, parent);
    }
  }

  vector<string> cycle_path;
  if (graph.has_cycle(cycle_path)) {
    string err_msg = "Error: cycle in classes " + cycle_path[0];
    for (int i = 1; i < (int) cycle_path.size(); i++) {
      err_msg += ", " + cycle_path[i];
    }
    error(0, err_msg);
    return false;
  }
  return true;

}

/* entry point for type checking, called from main. */
int TypeChecker::typecheck() {

  curr_filename = filename;

  initialize_basic_classes();
  initialize_declared_classes();

  if (!check_inheritance_cycles()) {
    /* cant continue to add methods/attrs to classes if there are inheritance cycles */
    return semant_errors;
  }

  /* AT EVERY SCOPE: identify every new CLASS 
     check inheritance to make sure there arent cycles

  */

  /* check inheritance at every scope */



  scopetable.push_scope();

  for (int i = 0; i < program.len(); i++) {
    Stmt *s = program.ith(i);
    s->typecheck();
  }


  scopetable.pop_scope();
  return semant_errors;
  
}


/* Given two ptrs to Type_ objects, returns true if A and B are equivalent. */
bool compare_type(Type_ *a, Type_ *b) {
  if (a->get_name() != b->get_name())
    return false;

  Type_ *a_nest = a->get_nested_type();
  Type_ *b_nest = b->get_nested_type();

  if (a_nest == NULL && b_nest == NULL)
    return true;
  if (a_nest != NULL && b_nest != NULL)
    return compare_type(a_nest, b_nest);
  
  return true;
}

//////////////////////////////////////////////////////////////
//
// Expression Typechecking 
// 
//////////////////////////////////////////////////////////////

Type_ *FormalStmt::typecheck() {}
Type_ *MethodStmt::typecheck() {
  scopetable.push_scope();
  for (FormalStmt *fml: formal_list) {
    scopetable.add_elem(fml->get_name(), fml->get_type());
  }

  scopetable.pop_scope();
}
Type_ *IfStmt::typecheck() {}

Type_ *ClassStmt::typecheck() {

}

Type_ *ForStmt::typecheck() {}
Type_ *AttrStmt::typecheck() {
  Type_ *init_type = init->typecheck();
  if (!compare_type(type, init_type)) {
    string err_msg = "Error: type " + init_type->to_str() + " does not conform to declared type " + type->to_str();
    error(init->lineno, err_msg);
  }
}
Type_ *WhileStmt::typecheck() {}

Type_ *ReturnExpr::typecheck() {}
Type_ *ListElemRef::typecheck() {}
Type_ *DispatchExpr::typecheck() {}
Type_ *IntConstExpr::typecheck() {
  return new Type_("int", NULL);
}
Type_ *ObjectIdExpr::typecheck() {
  return scopetable.lookup(name);
}
Type_ *BoolConstExpr::typecheck() {
  return new Type_("bool", NULL);
}
Type_ *ListConstExpr::typecheck() {
  return new Type_("list", new Type_("object", NULL));
}
Type_ *StrConstExpr::typecheck() {
  return new Type_("string", NULL);
}
Type_ *NewExpr::typecheck() {
  return expr->typecheck();
}
Type_ *ContExpr::typecheck() {}
Type_ *KillExpr::typecheck() {}
Type_ *ThisExpr::typecheck() {}
Type_ *BinopExpr::typecheck() {
  Type_ *lhs_type = lhs->typecheck();
  Type_ *rhs_type = rhs->typecheck();

  if (!compare_type(lhs_type, rhs_type)) {

  }
}
Type_ *BreakExpr::typecheck() {}
