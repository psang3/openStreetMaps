#pragma once

#include <iostream>
#include <map>
#include <set>
#include <unordered_map>
#include <vector>

using namespace std;

/// @brief Simple directed graph using an adjacency list.
/// @tparam VertexT vertex type
/// @tparam WeightT edge weight type
template <typename VertexT, typename WeightT>
class graph {
 private:
  // TODO_STUDENT
  unordered_map<VertexT, unordered_map<VertexT, WeightT>> edgeData;
    int connectionCount = 0;
 public:
  /// Default constructor
  graph() {
    // TODO_STUDENT
  }

  /// @brief Add the vertex `v` to the graph, must typically be O(1).
  /// @param v
  /// @return true if successfully added; false if it existed already
  bool addVertex(VertexT v) {
    // Check for existing vertex entry
    if (edgeData.count(v) == 0) {
        // Initialize empty connection map for new vertex
        edgeData.emplace(v, unordered_map<VertexT, WeightT>());
        return true;
    }
    return false;
}

  /// @brief Add or overwrite directed edge in the graph, must typically be
  /// O(1).
  /// @param from starting vertex
  /// @param to ending vertex
  /// @param weight edge weight / label
  /// @return true if successfully added or overwritten;
  ///         false if either vertices isn't in graph
  bool addEdge(VertexT from, VertexT to, WeightT weight) {
    
    if (edgeData.find(from) == edgeData.end() ||
        edgeData.find(to) == edgeData.end()) {
      return false;
    }

    
    if (edgeData[from].find(to) == edgeData[from].end()) {
      edgeData[from][to] = weight;
      connectionCount++;
    }
    
    else {
      edgeData[from][to] = weight;
    }
    return true;
}

  /// @brief Maybe get the weight associated with a given edge, must typically
  /// be O(1).
  /// @param from starting vertex
  /// @param to ending vertex
  /// @param weight output parameter
  /// @return true if the edge exists, and `weight` is set;
  ///         false if the edge does not exist
  bool getWeight(VertexT from, VertexT to, WeightT& weight) const {
    // TODO_STUDENT
    if (edgeData.find(from) == edgeData.end()) {
      return false;
    }
    if (edgeData.at(from).find(to) == edgeData.at(from).end()) {
      return false;
    }
    weight = edgeData.at(from).at(to);
    return true;
    
  }

  /// @brief Get the out-neighbors of `v`. Must run in at most O(|V|).
  /// @param v
  /// @return vertices that v has an edge to
  set<VertexT> neighbors(VertexT v) const {
    set<VertexT> S;
    // TODO_STUDENT
    if (edgeData.find(v) != edgeData.end()) {
      
      for (auto nb : edgeData.at(v)) {
        
        S.insert(nb.first);
      }
    }
    return S;
  }

  /// @brief Return a vector containing all vertices in the graph
  vector<VertexT> getVertices() const {
    // TODO_STUDENT
    vector<VertexT> vrts;
    for (auto vt : edgeData) {
      
      vrts.push_back(vt.first);
    }


    return vrts;
  }

  /// @brief Get the number of vertices in the graph. Runs in O(1).
  size_t numVertices() const {
    // TODO_STUDENT
    return edgeData.size();
  }

  /// @brief Get the number of directed edges in the graph. Runs in at most
  /// O(|V|), but should be O(1).
  size_t numEdges() const {
    // TODO_STUDENT
    return connectionCount;
  }
};
