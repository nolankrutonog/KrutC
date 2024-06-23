#ifndef TYPECHECKER_H 
#define TYPECHECKER_H 

#include "tree.h"
#include "parser.h"
#include "inheritance-graph.h"
#include <map>
#include <set>

class Graph;

class TypeChecker {
  Program program;
  std::string filename;

public: 
  bool debug = false;
  TypeChecker(Program program, bool debug, std::string filename) 
    : program(program), debug(debug), filename(filename) {}


  int typecheck();
  void initialize_basic_classes();
  void initialize_declared_classes();
  void check_valid_class_parents();

  // InheritanceGraph *create_inheritance_graph();
  bool check_inheritance_cycles();
  void populate_meth_attr_tables();
  
};


#endif // TYPECHECKER_H 