/**
 * @file graph.cc
 *
 */

#include "graph.h"

namespace {
std::vector<Label> transferred_label;
void TransferLabel(const std::string &filename) {
  std::ifstream fin(filename);

  if (!fin.is_open()) {
    std::cout << "Graph file " << filename << " not found!\n";
    exit(EXIT_FAILURE);
  }

  char type;
  int32_t graph_id_;
  size_t num_vertices_;

  std::set<Label> label_set;

  fin >> type >> graph_id_ >> num_vertices_;

  // preprocessing
  while (fin >> type) {
    if (type == 'v') {
      Vertex id;
      Label l;
      fin >> id >> l;

      label_set.insert(l);
    } else if (type == 'e') {
      Vertex v1, v2;
      Label l;
      fin >> v1 >> v2 >> l;
    }
  }

  fin.close();

  transferred_label.resize(
      *std::max_element(label_set.begin(), label_set.end()) + 1, -1);

  Label new_label = 0;
  for (Label l : label_set) {
    transferred_label[l] = new_label;
    new_label += 1;
  }
}
}  // namespace

Graph::Graph(const std::string &filename, bool is_query) {
  if (!is_query) {
    TransferLabel(filename);
  }
  std::vector<std::vector<Vertex>> adj_list;

  // Load Graph
  std::ifstream fin(filename);
  std::set<Label> label_set;

  if (!fin.is_open()) {
    std::cout << "Graph file " << filename << " not found!\n";
    exit(EXIT_FAILURE);
  }

  char type;

  fin >> type >> graph_id_ >> num_vertices_;

  adj_list.resize(num_vertices_);

  start_offset_.resize(num_vertices_ + 1);
  label_.resize(num_vertices_);

  num_edges_ = 0;

  //num_children.resize(num_vertices_);
  // children_id.resize(num_vertices_);
  //std::map<Vertex, size_t>::iterator it;

  // preprocessing
  while (fin >> type) {
    if (type == 'v') {
      Vertex id;
      Label l;
      fin >> id >> l;

      if (static_cast<size_t>(l) >= transferred_label.size())
        l = -1;
      else
        l = transferred_label[l];

      label_[id] = l;
      label_set.insert(l);
      vertex_ids_.push_back(id);
    } else if (type == 'e') {
      Vertex v1, v2;
      Label l;
      fin >> v1 >> v2 >> l;

      adj_list[v1].push_back(v2);
      adj_list[v2].push_back(v1);

      num_edges_ += 1;

      auto it = num_parent.find(v2);
      if(it != num_parent.end()) {
        // size_t n = it->second;
        // n++;
        // it->second = n;
        it->second++;
        //std::cout << "exist vertex: " << v2 << ", num_parent: " << it->second << "\n";
      } else {
        num_parent.insert(std::make_pair(v2, 1));
        //std::cout << "new vertex: " << v2 << ", num_parent: " << num_parent.find(v2)->second << "\n";
      }

      // if(num_parent.find(v2)==num_parent.end()) num_parent.insert(std::make_pair(v2, 1));  // update num_parent
      // else {
      //   size_t n = num_parent.find(v2)->second;
      //   n++;
      //   num_parent.at(v2) = n;
      // }

      it = num_children.find(v2);
      if(it != num_children.end()) {
        it->second++;
      } else num_children.insert(std::make_pair(v1, 1));

      // if(num_children.find(v1)==num_parent.end()) num_children.insert(std::make_pair(v1, 1));  // update num_children
      // else {
      //   size_t n = num_children.find(v1)->second;
      //   n++;
      //   //num_children.at(v1) = n;
      // }
      auto iter = children_id.find(v1);
      if(iter !=children_id.end()) {
        iter->second.push_back(v2);
        // std::cout << "exist vertex: " << v2 << ", children_id: " << children_id.find(v2)->second << "\n";
      } else {
        std::vector<Vertex> id;
        id.push_back(v2);
        children_id.insert(std::make_pair(v1, id));  // update children_id
      }

      iter = parent_id.find(v2);
      if(iter!=parent_id.end()) {
        iter->second.push_back(v1);
      } else {
        std::vector<Vertex> id;
        id.push_back(v1);
        parent_id.insert(std::make_pair(v2, id));  // update parent_id
      }
    }
  }

  fin.close();

  adj_array_.resize(num_edges_ * 2);

  num_labels_ = label_set.size();

  max_label_ = *std::max_element(label_set.begin(), label_set.end());

  label_frequency_.resize(max_label_ + 1);

  start_offset_by_label_.resize(num_vertices_ * (max_label_ + 1));

  start_offset_[0] = 0;
  for (size_t i = 0; i < adj_list.size(); ++i) {
    start_offset_[i + 1] = start_offset_[i] + adj_list[i].size();
  }

  for (size_t i = 0; i < adj_list.size(); ++i) {
    label_frequency_[GetLabel(i)] += 1;

    auto &neighbors = adj_list[i];

    if (neighbors.size() == 0) continue;

    // sort neighbors by ascending order of label first, and descending order of
    // degree second
    std::sort(neighbors.begin(), neighbors.end(), [this](Vertex u, Vertex v) {
      if (GetLabel(u) != GetLabel(v))
        return GetLabel(u) < GetLabel(v);
      else if (GetDegree(u) != GetDegree(v))
        return GetDegree(u) > GetDegree(v);
      else
        return u < v;
    });

    Vertex v = neighbors[0];
    Label l = GetLabel(v);

    start_offset_by_label_[i * (max_label_ + 1) + l].first = start_offset_[i];

    for (size_t j = 1; j < neighbors.size(); ++j) {
      v = neighbors[j];
      Label next_l = GetLabel(v);

      if (l != next_l) {
        start_offset_by_label_[i * (max_label_ + 1) + l].second =
            start_offset_[i] + j;
        start_offset_by_label_[i * (max_label_ + 1) + next_l].first =
            start_offset_[i] + j;
        l = next_l;
      }
    }

    start_offset_by_label_[i * (max_label_ + 1) + l].second =
        start_offset_[i + 1];

    std::copy(adj_list[i].begin(), adj_list[i].end(),
              adj_array_.begin() + start_offset_[i]);
  }
}

Graph::~Graph() {}
