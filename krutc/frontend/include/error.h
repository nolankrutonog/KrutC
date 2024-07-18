#ifndef ERROR_H
#define ERROR_H

#include <iostream>
#include <vector>

#define RESET "\033[0m"
#define RED "\033[31m"
#define MAGENTA "\033[35m"

enum ErrorType { LEXER_ERROR, SYNTAX_ERROR, SEMANTIC_ERROR };

const std::string ErrorTypeStrings[] = {"lexer", "syntax", "semantic"};

class Error {
 protected:
  ErrorType err_type;
  std::string filename;
  int lineno;
  std::string err_msg;

 public:
  Error(ErrorType err_type, const std::string& filename, int lineno,
        const std::string& err_msg)
      : err_type(err_type),
        filename(filename),
        lineno(lineno),
        err_msg(err_msg) {}

  virtual void print() {
    std::cerr << RED << "ERROR " << RESET << ErrorTypeStrings[err_type] << ":"
              << filename << ":" << lineno << ": " << err_msg << std::endl;
  }

  virtual ~Error() = default;
};

class Warning : public Error {
 public:
  Warning(ErrorType err_type, std::string filename, int lineno,
          std::string warn_msg)
      : Error(err_type, filename, lineno, warn_msg) {}

  void print() override {
    std::cerr << MAGENTA << "WARNING " << RESET << ErrorTypeStrings[err_type]
              << ":" << filename << ":" << lineno << ": " << err_msg
              << std::endl;
  }
};

#endif // ERROR_H