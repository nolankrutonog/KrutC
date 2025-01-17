#ifndef SCOPETABLE_H
#define SCOPETABLE_H

#include <map>
#include <vector>

#include "parser.h"
#include "tree.h"

/* table that maps an object id to its type */
class ScopeTable {
  std::vector<std::map<std::string, Type_ *>> table;

 public:
  Type_ *lookup(std::string s);

  Type_ *check_current_scope(std::string);
  void add_elem(std::string, Type_ *);
  void push_scope();
  void pop_scope();
};

#endif  // SCOPETABLE_H