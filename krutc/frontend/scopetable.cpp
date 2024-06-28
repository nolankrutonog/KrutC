#include <iostream>
#include <cassert>
#include "include/scopetable.h"

using namespace std;

/* Given id S, returns a ptr to the Type_ of the first occurrence of S. */
Type_ *ScopeTable::lookup(string s) {
  assert(table.size());
  for (int i = (int) table.size() - 1; i >= 0; i--) {
    map<string, Type_*>& scope = table[i];
    if (scope.find(s) != scope.end()) {
      return scope[s];
    }
  }
  return NULL;
}

/* Given id s, returns a ptr to S's Type_ if S is defined in the current scope */
Type_ *ScopeTable::check_current_scope(string s) {
  assert(table.size());
  map<string, Type_*>& scope = table[table.size() - 1];
  if (scope.find(s) != scope.end())
    return scope[s];
  return NULL;
}

void ScopeTable::add_elem(string key, Type_* value) {
  assert(table.size());
  table[table.size() - 1][key] = value;
}

void ScopeTable::push_scope() {
  map<string, Type_*> m;
  table.push_back(m);
}

void ScopeTable::pop_scope() {
  assert(table.size());
  table.pop_back();
}


