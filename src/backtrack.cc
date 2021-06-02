/**
 * @file backtrack.cc
 *
 */

#include "backtrack.h"
#include "list"
#include "limits"

std::vector<std::vector<std::pair<Vertex, Vertex>>> g_paths;  // all_paths
std::vector<bool> g_allParentsMatched;
std::vector<size_t> g_num_parent;
std::vector<size_t> g_num_matched_parent;  // initialized to 0 already
std::vector<bool> g_is_matched;

Backtrack::Backtrack() {}
Backtrack::~Backtrack() {}

void Backtrack::Preprocess(const Graph &query){
  size_t num_vertices = query.GetNumVertices();
  g_allParentsMatched.assign(num_vertices, false);
  g_num_parent.resize(num_vertices);
  for(size_t i=0; i<num_vertices; i++){
    g_num_parent[i] = query.GetNumParent(i);
    if(g_num_parent[i]==0) g_allParentsMatched[i] = true;   // num of parent = 0, then all parent matched
  }
  g_is_matched.assign(num_vertices, false);
}

void Backtrack::BacktrackMain(const Graph &data, const Graph &query,
                                const CandidateSet &cs){
  Preprocess(query);
  std::vector<std::pair<Vertex, Vertex>> match;
  SubgraphSearch(data, query, cs, match);
}

void Backtrack::SubgraphSearch(const Graph &data, const Graph &query,
                                const CandidateSet &cs, std::vector<std::pair<Vertex, Vertex>> match){
  if(match.size()==query.GetNumVertices()){
    PrintAllMatches(match);
    return;
  } else {
    Vertex nextQueryVertex = NextQueryVertex(query, cs);
    std::vector<Vertex> candidates = GetCandidates(cs, nextQueryVertex);  // vector of v
    for(Vertex v : candidates){
      std::pair<Vertex, Vertex> pair = {nextQueryVertex, v};  // new pair 
      if(IsExtendable(pair, match, data, query, cs)==true){
        match.push_back(pair);    // update match
        UpdateState(query, nextQueryVertex);  // update global variables 
        SubgraphSearch(data, query, cs, match);  // recursive call
        match.pop_back();  // delete the last element 
        RestoreState(query, nextQueryVertex);
      }
    }
  }
}

void Backtrack::RestoreState(const Graph &query, Vertex u){  // TODO  
  std::vector<Vertex> childrenID = query.GetChildrenID(u);
  for(Vertex u_child : childrenID){
    g_num_matched_parent[u_child]--;
    if(g_num_matched_parent[u_child] < g_num_parent[u_child]) {
      g_allParentsMatched[u_child]=false;
    }
  }
  g_is_matched[u] = false;
}

void Backtrack::UpdateState(const Graph &query, Vertex u){
  std::vector<Vertex> childrenID = query.GetChildrenID(u);
  for(Vertex u_child : childrenID){
    g_num_matched_parent[u_child]++;
    if(g_num_matched_parent[u_child]==g_num_parent[u_child]) {
      g_allParentsMatched[u_child]=true;
    }
  }
  g_is_matched[u] = true;
}
/*
void Backtrack::UpdateState(std::pair<Vertex, Vertex> pair){ // pair(u, v)
  g_num_matched_parent[pair.first]++;
  if(g_num_matched_parent[pair.first]==g_num_parent[pair.first]){
    g_allParentsMatched[pair.first] = true;
  }
  g_is_matched[pair.first] = true;  // query vertex u is matched
}*/

Vertex Backtrack::NextQueryVertex(const Graph &query, const CandidateSet &cs){
  // 1) 아직 matching되지 않았고 2) parent들의 매칭이 완료됐고 3) 그 중 candidate size가 제일 작은 vertex
  std::vector<Vertex> nextQueryCandidates;
  for(size_t i=0; i<query.GetNumVertices(); i++){  //TODO 좀 더 효율적인 것으로 바꿀 것
    if(g_is_matched[i]==false && g_allParentsMatched[i]==true){
      nextQueryCandidates.push_back(i);
    }
  }
  size_t min = std::numeric_limits<size_t>::max();
  Vertex next_u;
  for(Vertex u : nextQueryCandidates){
    if(min > cs.GetCandidateSize(u)){
      min = cs.GetCandidateSize(u);
      next_u = u;
    }
  }
  return next_u;
}

bool Backtrack::IsExtendable(std::pair<Vertex, Vertex> candidate, std::vector<std::pair<Vertex, Vertex>> match, 
                            const Graph &data, const Graph &query, const CandidateSet &cs){
  for(size_t i=0; i<match.size(); i++){
    if(candidate.second == match[i].second){
      return false;
    }
  }
  std::vector<Vertex> parent = query.GetParentID(candidate.first);
  for(size_t i=0; i<parent.size(); i++){
    for(size_t j=0; j<match.size(); j++){
      if(match[j].first == parent[i]){
        if(!data.IsNeighbor(match[j].second, candidate.second)){
          return false;
        }
      }
    }
  }
  return true;
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

void Backtrack::PrintAllMatches(std::vector<std::pair<Vertex, Vertex>> match) {
  // TODO match 프린트하는 것 구현해야 함
  
}


/////////////////////////// 이하 무시 /////////////////////////////

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


