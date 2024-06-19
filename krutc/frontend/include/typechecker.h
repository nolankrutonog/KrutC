#ifndef TYPECHECKER_H 
#define TYPECHECKER_H 

#include "tree.h"
#include "parser.h"
#include <map>
#include <set>

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
  void check_declared_classes_parents();
  bool check_inheritance_cycles();
  
};

class Graph {
  std::vector<std::string> nodes;
  std::vector<std::pair<std::string, std::string>> edges;
  std::map<std::string, std::vector<std::string>> adj_list;

  void build_adjacency_list() {
    adj_list.clear();
    for (const auto& node : nodes) {
      adj_list[node] = std::vector<std::string>();
    }
    for (const auto& edge : edges) {
      adj_list[edge.first].push_back(edge.second);
    }
  }

  bool dfs(const std::string& node, std::set<std::string>& visited, std::set<std::string>& rec_stack, std::vector<std::string>& path) const {
    visited.insert(node);
    rec_stack.insert(node);
    path.push_back(node);

    for (const auto& neighbor : adj_list.at(node)) {
      if (rec_stack.find(neighbor) != rec_stack.end()) {
        // Cycle detected, append the starting node to complete the cycle path
        path.push_back(neighbor);
        return true;
      }
      if (visited.find(neighbor) == visited.end()) {
        if (dfs(neighbor, visited, rec_stack, path)) {
          return true;
        }
      }
    }

    rec_stack.erase(node);
    path.pop_back();
    return false;
  }


public:
  Graph() {}

  void add_node(std::string s) { nodes.push_back(s); }
  void add_edge(std::string s1, std::string s2) { edges.push_back(std::make_pair(s1, s2)); }

  bool has_cycle(std::vector<std::string>& cycle_path) {
    build_adjacency_list();
    std::set<std::string> visited;
    std::set<std::string> rec_stack;

    for (const auto& node : nodes) {
      if (visited.find(node) == visited.end()) {
        std::vector<std::string> path;
        if (dfs(node, visited, rec_stack, path)) {
          // Reverse path to get the correct cycle path
          auto cycle_start = std::find(path.begin(), path.end(), path.back());
          cycle_path.assign(cycle_start, path.end());
          return true;
        }
      }
    }
    return false;
  }


};

// class InheritanceGraph {
//   int num_classes;
//   std::vector<std::vector<int>> parents;

//   bool dfs_util(int c, std::vector<bool>& visited, 
//                 std::vector<bool>& rec_stack, std::vector<std::string>& path);

// public:
//   InheritanceGraph(int num_classes) : num_classes(num_classes) {
//     parents.resize(num_classes);
//   }

//   void add_parent(int c, int p);
//   bool has_cycle();
// };


#endif // TYPECHECKER_H 