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
  void BacktrackMain(const Graph &data, const Graph &query,
                                const CandidateSet &cs);
  void SubgraphSearch(const Graph &data, const Graph &query,
                                const CandidateSet &cs, std::vector<std::pair<Vertex, Vertex>> match);
  Vertex NextQueryVertex(const Graph &query, const CandidateSet &cs);
  void Backtrack::UpdateState(const Graph &query, Vertex u);
  void UpdateState(std::pair<Vertex, Vertex> pair);
  void Backtrack::RestoreState(const Graph &query, Vertex u);
  std::vector<Vertex> Backtrack::GetCandidates(const CandidateSet &cs, Vertex queryVertexID);
  bool Backtrack::IsExtendable(Vertex candidate, std::vector<std::pair<Vertex, Vertex>> match, 
                            const Graph &data, const Graph &query, const CandidateSet &cs);

  void PrintAllMatches(std::vector<std::pair<Vertex, Vertex>> match);
  void MatchAllPairs(const Graph &data, const Graph &query,
                       const CandidateSet &cs, Vertex myID);
  int GetPathIndex(size_t size);
  bool CheckVertexCondition(int vertexID, const Graph &data, const Graph &query,
                       const CandidateSet &cs);
};

#endif  // BACKTRACK_H_
