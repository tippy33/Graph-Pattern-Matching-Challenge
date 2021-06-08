/**
 * @file backtrack.h
 *
 */

#ifndef BACKTRACK_H_
#define BACKTRACK_H_

#include "candidate_set.h"
#include "common.h"
#include "graph.h"

class Backtrack {
 public:
  Backtrack();
  ~Backtrack();

  void Preprocess(const Graph &query);
  void BacktrackMain(const Graph &data, const Graph &query, const CandidateSet &cs);
  void SubgraphSearch(const Graph &data, const Graph &query,
                                const CandidateSet &cs, std::vector<std::pair<Vertex, Vertex>> match);
  Vertex NextQueryVertex(const Graph &query, const CandidateSet &cs);
  void UpdateState(const Graph &query, Vertex u);
  void UpdateState(std::pair<Vertex, Vertex> pair);
  void RestoreState(const Graph &query, Vertex u);
  void printMatch(std::vector<std::pair<Vertex, Vertex>> match);
  std::vector<Vertex> GetCandidates(const CandidateSet &cs, Vertex queryVertexID);
  bool IsExtendable(std::pair<Vertex, Vertex> pair, std::vector<std::pair<Vertex, Vertex>> match, 
                            const Graph &data, const Graph &query, const CandidateSet &cs);
  bool cmp(std::pair<Vertex, Vertex> pair1, std::pair<Vertex, Vertex> pair2);
  void PrintAllMatches(const Graph &query, std::vector<std::pair<Vertex, Vertex>> match);
  
  void MatchAllPairs(const Graph &data, const Graph &query,
                       const CandidateSet &cs, Vertex myID);
  int GetPathIndex(size_t size);
  bool CheckVertexCondition(int vertexID, const Graph &data, const Graph &query,
                       const CandidateSet &cs);
};

#endif  // BACKTRACK_H_
