#ifndef INHERITANCE_GRAPH_H
#define INHERITANCE_GRAPH_H

#include <vector>
#include <map>
#include <set>
#include <iostream>

class InheritanceGraph {
  std::vector<std::string> nodes;
  std::map<std::string, std::vector<std::string>> parents;
  std::map<std::string, std::vector<std::string>> children;

public:
  InheritanceGraph() {}

  void add_node(std::string s) { nodes.push_back(s); }
  void add_parent(std::string curr, std::string other) { parents[curr].push_back(other); }
  void add_child(std::string curr, std::string other) { children[curr].push_back(other); }

  void detect_cycles(std::vector<std::vector<std::string>>& cycle_paths) {
    std::set<std::string> visited;
    std::set<std::string> stack;
    std::vector<std::string> path;
    for (const auto& node : nodes) {
      if (visited.find(node) == visited.end()) {
        if (dfs(node, visited, stack, path)) {
          // print_cycle(path);
          cycle_paths.push_back(path);
          path.clear();
        }
      }
    }
  }

private:
  bool dfs(const std::string& node, std::set<std::string>& visited, 
           std::set<std::string>& stack, std::vector<std::string>& path) {
    visited.insert(node);
    stack.insert(node);
    path.push_back(node);

    for (const auto& child : children[node]) {
      if (stack.find(child) != stack.end()) {
        path.push_back(child);  // complete the cycle path
        return true;
      }
      if (visited.find(child) == visited.end()) {
        if (dfs(child, visited, stack, path)) {
          return true;
        }
      }
    }

    stack.erase(node);
    path.pop_back();
    return false;
  }
};


#endif // INHERITANCE_GRAPH_H