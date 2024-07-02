#ifndef TYPECHECKER_H 
#define TYPECHECKER_H 

#include "tree.h"
#include "parser.h"

class TypeChecker {
  Program program;
  std::string filename;

public: 
  bool debug = false;
  TypeChecker(Program program, bool debug, std::string filename) 
    : program(program), debug(debug), filename(filename) {}


  int typecheck();
  void initialize_basic_classes();
  void initialize_builtin_methods();
  void initialize_declared_classes();
  void check_valid_class_parents();

  bool check_inheritance_cycles();
  void populate_feature_tables();
  bool check_global_features();
  
};


#endif // TYPECHECKER_H 