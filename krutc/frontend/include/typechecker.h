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
  void initialize_declared_classes();
  void check_inheritance_cycles();
  void check_declared_classes_parents();
  
};


#endif // TYPECHECKER_H 