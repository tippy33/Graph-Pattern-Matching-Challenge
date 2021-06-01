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

  void BacktrackMain(const Graph &data, const Graph &query,
                                const CandidateSet &cs);
  void SubgraphSearch(const Graph &data, const Graph &query,
                                const CandidateSet &cs, std::vector<std::pair<Vertex, Vertex>> match);
  Vertex NextQueryVertex();
  std::vector<Vertex> Backtrack::GetCandidates(const CandidateSet &cs, Vertex queryVertexID);

  void PrintAllMatches(std::vector<std::pair<Vertex, Vertex>> match);
  void MatchAllPairs(const Graph &data, const Graph &query,
                       const CandidateSet &cs, Vertex myID);
  int GetPathIndex(size_t size);
  bool CheckVertexCondition(int vertexID, const Graph &data, const Graph &query,
                       const CandidateSet &cs);
};

#endif  // BACKTRACK_H_
