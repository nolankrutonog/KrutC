#ifndef INHERITANCE_GRAPH_H
#define INHERITANCE_GRAPH_H

#include <iostream>
#include <map>
#include <set>
#include <stack>
#include <vector>

class InheritanceGraph {
  std::map<std::string, std::vector<std::string>> parents;

  void dfs(const std::string& node, std::map<std::string, bool>& visited, std::map<std::string, bool>& recStack,
           std::vector<std::string>& path, std::vector<std::vector<std::string>>& cycles) {
    visited[node]  = true;
    recStack[node] = true;
    path.push_back(node);

    for (const auto& parent : parents[node]) {
      if (!visited[parent]) {
        dfs(parent, visited, recStack, path, cycles);
      } else if (recStack[parent]) {
        std::vector<std::string> cycle;
        auto it = std::find(path.begin(), path.end(), parent);
        while (it != path.end()) {
          cycle.push_back(*it);
          it++;
        }
        cycle.push_back(parent);
        cycles.push_back(cycle);
      }
    }

    path.pop_back();
    recStack[node] = false;
  }

 public:
  InheritanceGraph(std::map<std::string, std::vector<std::string>> parents) : parents(parents) {}

  void check_for_cycles(std::vector<std::vector<std::string>>& cycles) {
    std::map<std::string, bool> visited;
    std::map<std::string, bool> recStack;
    std::vector<std::string> path;

    for (const auto& node : parents) {
      if (!visited[node.first]) { dfs(node.first, visited, recStack, path, cycles); }
    }
  }
};

#endif  // INHERITANCE_GRAPH_H