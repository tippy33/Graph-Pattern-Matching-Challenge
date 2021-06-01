/**
 * @file backtrack.cc
 *
 */

#include "backtrack.h"
#include "list"
#include "limits"

std::vector<std::vector<std::pair<Vertex, Vertex>>> g_paths;  // all_paths
//std::vector<std::pair<Vertex, Vertex>> 
//std::list<int> g_answerList;

Backtrack::Backtrack() {}
Backtrack::~Backtrack() {}

void Backtrack::SubgraphSearch(const Graph &data, const Graph &query,
                                const CandidateSet &cs, std::vector<std::pair<Vertex, Vertex>> match){
  if(match.size()==query.GetNumVertices()){
    PrintAllMatches(match);
  } else {
    Vertex nextQueryVertex = NextQueryVertex();
    std::vector<Vertex> candidates = GetCandidates(cs, nextQueryVertex);
    for(Vertex v : candidates){
      if(IsJoinable(v, match, data, query, cs)==true){
        std::pair<Vertex, Vertex> pair = {nextQueryVertex, v};  // update match
        match.push_back(pair);
        SubgraphSearch(data, query, cs, match);  // recursive call
        match.pop_back();  // delete the last element 
      }
    }
  }
}

bool Backtrack::IsJoinable(Vertex candidate, std::vector<std::pair<Vertex, Vertex>> match, 
                            const Graph &data, const Graph &query, const CandidateSet &cs){
  // TODO 들어온 candidate가 matching 될 수 있는지 아닌지 판별해서 알려줌
}

std::vector<Vertex> Backtrack::GetCandidates(const CandidateSet &cs, Vertex queryVertexID){
  size_t candidateSize = cs.GetCandidateSize(queryVertexID);
  std::vector<Vertex> candidates;
  for(int i=0; i<candidateSize; i++){
    candidates.push_back(cs.GetCandidate(queryVertexID, i));
  }
  return candidates;
}

Vertex Backtrack::NextQueryVertex(){
  // TODO
}

void Backtrack::PrintAllMatches(std::vector<std::pair<Vertex, Vertex>> match) {
  // TODO match 프린트하는 것 구현해야 함
}

///////////////////////////
void Backtrack::BacktrackMain(const Graph &data, const Graph &query,
                                const CandidateSet &cs){
  size_t root_candidate_size = cs.GetCandidateSize(0);  // vertex id 0
  std::vector<std::pair<Vertex, Vertex>> v;
  for(int i=0; i<root_candidate_size; i++){
    std::pair<Vertex, Vertex> p = {0, cs.GetCandidate(0, i)};
    v.push_back(p);
  }
  g_paths.push_back(v);
  // TODO 
}

void Backtrack::MatchAllPairs(const Graph &data, const Graph &query,
                                const CandidateSet &cs, Vertex myID){
  int num_children = query.GetNumChildren(myID);
  std::vector<Vertex> children_id = query.GetChildrenID(myID);
  size_t min = std::numeric_limits<size_t>::max();
  std::vector<std::pair<Vertex, Vertex>> v;
  Vertex next;
  for(int i=0; i<children_id.size(); i++){
    size_t size = cs.GetCandidateSize(children_id[i]);
    for(size_t j =0; j<size; j++){
      v.clear();
      std::pair<Vertex, Vertex> p = {children_id[i], cs.GetCandidate(children_id[i], j)};
      j++;
      v.push_back(p);  // TODO 
    } 
    if(min > size){
      min = size;
      next = cs.GetCandidate(myID, i);
    }
    g_paths.insert(g_paths.begin()+GetPathIndex(size), v);
  }

}

int Backtrack::GetPathIndex(size_t size){
  int i =0;
  std::vector<std::vector<std::pair<Vertex, Vertex>>>::iterator iter = g_paths.begin();
  while(g_paths[i].size() < size){
    iter++;
    i++;
  }
  return i;
}


