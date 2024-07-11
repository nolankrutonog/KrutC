#ifndef CODEGEN_H
#define CODEGEN_H

#include "tree.h"

class CodeGen {
  Program program;
  std::string filename;

public:
  CodeGen(Program program, std::string filename)
    : program(program), filename(filename) {}

  int codegen();
};


#endif // CODEGEN_H