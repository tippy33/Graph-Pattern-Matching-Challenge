/**
 * @file backtrack.cc
 *
 */

#include "backtrack.h"
#include "list"
#include "limits"
#include "algorithm"
#include "map"

std::vector<std::vector<std::pair<Vertex, Vertex>>> g_paths;  // all_paths
std::map<Vertex, bool> g_allParentsMatched;  // map<vertex id, true if all parents matched>
std::map<Vertex, size_t> g_num_parent;  // map<vertex id, num of parent of the vertex>
std::map<Vertex, size_t> g_num_matched_parent;  // map<vertex id, num of parent matched>
std::map<Vertex, bool> g_is_matched;  // map<vertex id, true if the query vertex is matched with data vertex>
bool print_first_call = true; 

Backtrack::Backtrack() {}
Backtrack::~Backtrack() {}

void Backtrack::Preprocess(const Graph &query){
  size_t num_vertices = query.GetNumVertices();
  std::vector<Vertex> vertex_ids = query.GetVertexIDs();
  for(size_t i=0; i<num_vertices; i++){
    Vertex v = vertex_ids[i];
    size_t num_parent = query.GetNumParent(v);
    // std::cout << "Preprocess v is: " << v << ", num_parent is: "<< num_parent <<"\n";
    g_is_matched.insert(std::make_pair(v, false));   // at first, every vertex is not matched
    g_num_parent.insert(std::make_pair(v, num_parent));

    if(num_parent == 0) g_allParentsMatched.insert(std::make_pair(v, true));
    else g_allParentsMatched.insert(std::make_pair(v, false));
  }
  // debug 
  // std::cout << "g_allParentsMatched: ";
  // for(auto it=g_allParentsMatched.begin(); it != g_allParentsMatched.end(); it++){
  //   std::cout << it->second << " ";
  // } std::cout << "\n";
  // std::cout << "g_num_parent: ";
  // for(auto it=g_num_parent.begin(); it != g_num_parent.end(); it++){
  //   std::cout << it->second << " ";
  // } std::cout << "\n";
} 

void Backtrack::BacktrackMain(const Graph &data, const Graph &query,
                                const CandidateSet &cs){
  Preprocess(query);
  // std::cout << "preprocessing done" << "\n";
  std::vector<std::pair<Vertex, Vertex>> match;
  SubgraphSearch(data, query, cs, match);
}

void Backtrack::SubgraphSearch(const Graph &data, const Graph &query,
                                const CandidateSet &cs, std::vector<std::pair<Vertex, Vertex>> match){
  if(match.size()==query.GetNumVertices()){
    PrintAllMatches(query, match);
    //return;
  } else {
    Vertex nextQueryVertex = NextQueryVertex(query, cs);
    //std::cout << "nextQueryVertex: " << nextQueryVertex <<"\n";
    std::vector<Vertex> candidates = GetCandidates(cs, nextQueryVertex);  // vector of v
    
    // std::cout << "candidates: ";
    // for(auto it=candidates.begin(); it != candidates.end(); it++){
    //   std::cout << *it << " ";
    // }   std::cout << "\n";
    
    for(Vertex v : candidates){
      std::pair<Vertex, Vertex> pair = {nextQueryVertex, v};  // new pair 
      if(IsExtendable(pair, match, data, query, cs)==true){
        match.push_back(pair);    // update match
        //printMatch(match);
        // std::cout << "error HERE\n";
        UpdateState(query, nextQueryVertex);  // update global variables 
        SubgraphSearch(data, query, cs, match);  // recursive call
        match.pop_back();  // delete the last element 
        //printMatch(match);
        //std::cout << "error HERE\n";
        RestoreState(query, nextQueryVertex);
      }
    }
  }
}

void Backtrack::printMatch(std::vector<std::pair<Vertex, Vertex>> match){
  std::cout << "printMatch: ";
  for(auto it=match.begin(); it!=match.end(); it++){
    std::pair<Vertex, Vertex> p = *it;
    std::cout << "(" << p.first <<", "<< p.second <<") ";
  }
  std::cout << "\n";
}

void Backtrack::RestoreState(const Graph &query, Vertex u){ 
  std::vector<Vertex> childrenID = query.GetChildrenID(u);
  for(Vertex u_child : childrenID){
    auto it = g_num_matched_parent.find(u_child);
    if(it != g_num_matched_parent.end()) it->second--;
    // size_t n = g_num_matched_parent.find(u_child)->second;
    // n--;
    // g_num_matched_parent.at(u_child) = n;
    if(it->second < g_num_parent.find(u_child)->second) 
      g_allParentsMatched.find(u_child)->second=false;
  }
  g_is_matched.find(u)->second = false;
}

void Backtrack::UpdateState(const Graph &query, Vertex u){
  std::vector<Vertex> childrenID = query.GetChildrenID(u);
  // std::cout << "childrendID: ";
  // for(auto it=childrenID.begin(); it != childrenID.end(); it++){
  //   std::cout << *it << " ";
  // } std::cout << "\n";
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

  // std::cout << "vertex_ids: ";
  // for(it=vertex_ids.begin(); it != vertex_ids.end(); it++){
  //   std::cout << *it << " ";
  // } std::cout << "\n";
  
  for(it=vertex_ids.begin(); it != vertex_ids.end(); it++){
    if(g_is_matched.find(*it)->second==false && g_allParentsMatched.find(*it)->second==true){  // first and second condition
      nextQueryCandidates.push_back(*it);
    } 
  }

  std::cout<< "nextQueryCandidates: ";
  for(it=nextQueryCandidates.begin(); it != nextQueryCandidates.end(); it++){
    std::cout << *it << " ";
  } std::cout << "\n";

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
  // TODO 들어온 candidate가 matching 될 수 있는지 아닌지 판별해서 알려줌
  for(auto it=match.begin(); it!=match.end(); it++){
    std::pair<Vertex, Vertex> p = *it;
    if(pair.second == p.second) return false;  // 중복 여부 확인
  }
  std::vector<Vertex> parentID = query.GetParentID(pair.first);
  for(size_t i=0; i<parentID.size(); i++){
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

// bool Backtrack::cmp(std::pair<Vertex, Vertex> pair1, std::pair<Vertex, Vertex> pair2){
//   return pair1.first < pair2.first;
// }

void Backtrack::PrintAllMatches(const Graph &query, std::vector<std::pair<Vertex, Vertex>> match) {
  // TODO match 프린트하는 것 구현해야 함
  if(print_first_call==true){
    std::cout << "t " << query.GetNumVertices() << "\n";
    print_first_call = false;
  }
  std::cout << "a ";
  for(auto it = match.begin(); it != match.end(); it++){
    std::pair<Vertex, Vertex> p = *it;
    std::cout << p.second << " ";
  } std::cout << "\n";

  // std::vector<std::pair<Vertex, Vertex>> copy;
  // std::copy(match.begin(), match.end(), copy.begin());
  // //std::sort(copy.begin(), copy.end(), cmp);
  // std::vector<std::pair<Vertex, Vertex>>::iterator iter;
  // std::cout << "a ";
  // for(iter = copy.begin(); iter != copy.end(); iter++){
  //   std::pair<Vertex, Vertex> p = *iter;
  //   std::cout << p.second << " ";
  // }
  // std::cout << "\n";
}


