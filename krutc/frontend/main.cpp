#include <iostream>

#include "include/codegen.h"
#include "include/parser.h"
#include "include/typechecker.h"

using namespace std;

int main(int argc, char *argv[]) {
  if (argc < 2) {
    cerr << "Usage: " << argv[0] << " <file.krut>" << endl;
    return -1;
  }
  string filename = argv[1];
  string suffix = ".krut";

  if (filename.compare(filename.size() - suffix.size(), suffix.size(),
                       suffix) != 0) {
    cerr << "Error: file must be of type" + suffix << endl;
    return -1;
  }

  bool token_dump = false;
  bool debug = false;
  bool tree = false;
  if (argc >= 3) {
    for (int i = 2; i < argc; i++) {
      string flag = argv[i];
      if (flag == "-debug") {
        debug = true;
      } else if (flag == "-tdump") {
        token_dump = true;
      } else if (flag == "-tree") {
        tree = true;
      } else {
        cerr << "Error: Unknown flag " + flag << endl;
        cerr << "Expected flag '-tdump', '-debug', or '-tree'." << endl;
      }
    }
  }

  Parser parser = Parser(filename, debug, token_dump);

  if (!parser.check_lexer_errors()) {
    return -1;
  }

  Program program = parser.parse_program();

  if (parser.parser_errors) {
    /* cannot typecheck if the parser has errors */
    return -1;
  }

  TypeChecker typechecker = TypeChecker(program, debug, filename);

  int semant_errors = typechecker.typecheck();

  if (semant_errors) {
    return -1;
  }

  if (tree) {
    program.dump();
  }

  // CodeGen cgen = CodeGen(program, filename);

  // // TODO: are there going to be CGen errors?
  // int cgen_errors = cgen.codegen();

  // if (cgen_errors) {
  //   return -1;
  // }

  return 1;
}