/**
 * @file backtrack.cc
 *
 */

#include "backtrack.h"
#include "list"
#include "limits"
#include "algorithm"
#include "map"
#include "time.h"

std::vector<std::vector<std::pair<Vertex, Vertex>>> g_paths;  // all_paths
std::map<Vertex, bool> g_allParentsMatched;  // map<vertex id, true if all parents matched>
std::map<Vertex, size_t> g_num_parent;  // map<vertex id, num of parent of the vertex>
std::map<Vertex, size_t> g_num_matched_parent;  // map<vertex id, num of parent matched>
std::map<Vertex, bool> g_is_matched;  // map<vertex id, true if the query vertex is matched with data vertex>
std::map<Vertex, Vertex> g_match;   // map of matched pairs, called at IsExtendable
bool print_first_call = true; 
int g_cnt = 0;

Backtrack::Backtrack() {}
Backtrack::~Backtrack() {}

void Backtrack::Preprocess(const Graph &query){
  size_t num_vertices = query.GetNumVertices();
  std::vector<Vertex> vertex_ids = query.GetVertexIDs();
  for(size_t i=0; i<num_vertices; i++){
    Vertex v = vertex_ids[i];
    size_t num_parent = query.GetNumParent(v);
    g_is_matched.insert(std::make_pair(v, false));   // at first, every vertex is not matched
    g_num_parent.insert(std::make_pair(v, num_parent));

    if(num_parent == 0) g_allParentsMatched.insert(std::make_pair(v, true));
    else g_allParentsMatched.insert(std::make_pair(v, false));
  }
} 

void Backtrack::BacktrackMain(const Graph &data, const Graph &query,
                                const CandidateSet &cs){
  Preprocess(query);
  std::vector<std::pair<Vertex, Vertex>> match;
  SubgraphSearch(data, query, cs, match);
}

void Backtrack::SubgraphSearch(const Graph &data, const Graph &query,
                                const CandidateSet &cs, std::vector<std::pair<Vertex, Vertex>> match){
  if(match.size()==query.GetNumVertices()){  // if all pairs matched
    PrintAllMatches(query, match);
  } else {
    Vertex nextQueryVertex = NextQueryVertex(query, cs);
    //std::cout << "nextQueryVertex: " << nextQueryVertex <<"\n";
    std::vector<Vertex> candidates = GetCandidates(cs, nextQueryVertex);  // vector of v
    
    for(Vertex v : candidates){
      std::pair<Vertex, Vertex> pair = {nextQueryVertex, v};  // new pair 
      if(IsExtendable(pair, match, data, query, cs)==true){
        match.push_back(pair);    // update match
        g_match.insert(std::make_pair(nextQueryVertex, v));  // update g_match
        UpdateState(query, nextQueryVertex);  // update global variables 
        SubgraphSearch(data, query, cs, match);  // recursive call
        match.pop_back();  // delete the last element 
        g_match.erase(nextQueryVertex);
        RestoreState(query, nextQueryVertex);
      }
    }
  }
}

void Backtrack::RestoreState(const Graph &query, Vertex u){ 
  std::vector<Vertex> childrenID = query.GetChildrenID(u);
  for(Vertex u_child : childrenID){
    auto it = g_num_matched_parent.find(u_child);
    if(it != g_num_matched_parent.end()) it->second--;
    if(it->second < g_num_parent.find(u_child)->second) 
      g_allParentsMatched.find(u_child)->second=false;
  }
  g_is_matched.find(u)->second = false;
}

void Backtrack::UpdateState(const Graph &query, Vertex u){
  std::vector<Vertex> childrenID = query.GetChildrenID(u);
  for (Vertex u_child : childrenID) {
    auto it = g_num_matched_parent.find(u_child);
    if (it != g_num_matched_parent.end()) 
      it->second++;
    else 
      g_num_matched_parent.insert(std::make_pair(u_child, 1));
    if (g_num_matched_parent.find(u_child)->second == g_num_parent.find(u_child)->second)
      g_allParentsMatched.find(u_child)->second = true;
  }
  g_is_matched.find(u)->second = true;
}

Vertex Backtrack::NextQueryVertex(const Graph &query, const CandidateSet &cs){
  // 1) 아직 matching되지 않았고 2) parent들의 매칭이 완료됐고 3) 그 중 candidate size가 제일 작은 vertex
  std::vector<Vertex> nextQueryCandidates;
  std::vector<Vertex> vertex_ids = query.GetVertexIDs();
  std::vector<Vertex>::iterator it;

  
  for(it=vertex_ids.begin(); it != vertex_ids.end(); it++){
    if(g_is_matched.find(*it)->second==false && g_allParentsMatched.find(*it)->second==true){  // first and second condition
      nextQueryCandidates.push_back(*it);
    } 
  }

  Vertex next = nextQueryCandidates[0];
  size_t min = std::numeric_limits<size_t>::max();
  for(it=nextQueryCandidates.begin(); it!=nextQueryCandidates.end(); it++){
    if(min > cs.GetCandidateSize(*it)) {
      min = cs.GetCandidateSize(*it);
      next = *it;
    }
  }
  return next;
}

bool Backtrack::IsExtendable(std::pair<Vertex, Vertex> pair, std::vector<std::pair<Vertex, Vertex>> match, 
                            const Graph &data, const Graph &query, const CandidateSet &cs){
  for(auto it=match.begin(); it!=match.end(); it++){
    std::pair<Vertex, Vertex> p = *it;
    if(pair.second == p.second) return false;  // 중복 여부 확인
  }
  std::vector<Vertex> parentID = query.GetParentID(pair.first);
  for(size_t i=0; i<parentID.size(); i++){
    // auto it = g_match.find(parentID[i]);
    // if(it==g_match.end()) exit(1);
    // else {
    //   Vertex matched_data_vertex = g_match.find(parentID[i])->second;
    //   if(!data.IsNeighbor(matched_data_vertex, pair.second)) return false;
    // }
    for(size_t j=0; j<match.size(); j++){
      if(match[j].first==parentID[i]) {
        if(!data.IsNeighbor(match[j].second, pair.second)) return false;
        break;
      } 
    }
  }
  return true; 
}

std::vector<Vertex> Backtrack::GetCandidates(const CandidateSet &cs, Vertex queryVertexID){
  size_t candidateSize = cs.GetCandidateSize(queryVertexID);
  std::vector<Vertex> candidates;
  for(size_t i=0; i<candidateSize; i++){
    candidates.push_back(cs.GetCandidate(queryVertexID, i));
  }
  return candidates;
}

void Backtrack::PrintAllMatches(const Graph &query, std::vector<std::pair<Vertex, Vertex>> match) {
  g_cnt++;
  
  if(print_first_call==true){
    printf("t %u", query.GetNumVertices());
    printf("\n");
    print_first_call = false;
  }
  
  printf("a ");
  for(auto it = match.begin(); it != match.end(); it++){
    std::pair<Vertex, Vertex> p = *it;
    printf("%d ", p.second);
  } printf("\n");

  if(g_cnt==100000) exit(0);
}


