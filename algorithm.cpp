#include "headers/algorithm.h"

#include <algorithm>
#include <queue>

Algorithm::Algorithm(size_t n, size_t m, size_t lightpath_bandwidth, const std::vector<TrafficDemand> &traffic_demands,
                     const Graph &network)
        : n_(n), lightpath_bandwidth_(lightpath_bandwidth), network_(network), virtual_topology_(n),
          is_path_ok_([this](const std::vector<size_t> &path,
                             size_t bandwidth) {
              return HasEnoughBandwidth(path, bandwidth) && IsSimple(path);
          }) {
    traffic_demands_ptrs_.reserve(m);
    cur_solution_.use_of_lightpaths.reserve(m);
    cur_solution_.demand_lightpaths.reserve(m);
    cur_solution_.lightpath_demands.reserve(m);
    for (const TrafficDemand &demand: traffic_demands) {
        cur_solution_.demand_lightpaths[&demand];
        traffic_demands_ptrs_.push_back(&demand);
    }
}

Solution Algorithm::Run() {
    size_t no_changes_counter = 0;
    size_t min_lightpaths_number = SIZE_MAX;
    while (no_changes_counter != 4) {
        Construct();
        LightpathMin();

        size_t lightpaths_number = cur_solution_.lightpaths_number_;
        if (lightpaths_number < min_lightpaths_number) {
            best_solution_ = cur_solution_;
            min_lightpaths_number = lightpaths_number;
            no_changes_counter = 0;
        } else {
            ++no_changes_counter;
        }
    }

    return best_solution_;
}

void Algorithm::Construct() {
    std::sort(traffic_demands_ptrs_.begin(), traffic_demands_ptrs_.end(),
              [this](const TrafficDemand *left, const TrafficDemand *right) {
                  return cur_solution_.demand_lightpaths[left].size() <
                         cur_solution_.demand_lightpaths[right].size();
              });

    cur_solution_.Reset(lightpath_bandwidth_);

    for (const TrafficDemand *demand: traffic_demands_ptrs_) {
        std::vector<size_t> path = virtual_topology_.GetPathEdges(demand->source, demand->destination,
                                                                  demand->bandwidth, is_path_ok_);

        if (path.empty()) {
            size_t distance = network_.GetDistance(demand->source, demand->destination);
            std::deque<size_t> nodes;

            bool is_found = false;
            size_t lp_id = cur_solution_.lightpaths_.size();
            for (size_t node = 0; node < n_; ++node) {
                size_t remaining_distance = network_.GetDistance(node, demand->destination);
                if (remaining_distance < distance) {
                    nodes.push_front(node);
                    distance = remaining_distance;
                } else {
                    nodes.push_back(node);
                }
            }

            for (size_t node: nodes) {
                virtual_topology_.AddEdge(node, demand->destination, lp_id);
                cur_solution_.lightpaths_.emplace_back(lightpath_bandwidth_,
                                                       network_.GetPathVertices(node, demand->destination));
                path = virtual_topology_.GetPathEdges(demand->source, demand->destination, demand->bandwidth,
                                                      is_path_ok_);
                if (path.empty()) {
                    cur_solution_.lightpaths_.pop_back();
                    virtual_topology_.RemoveEdge(node, demand->destination, lp_id);
                } else {
                    is_found = true;
                    break;
                }
            }

            if (!is_found) {
                cur_solution_.lightpaths_.emplace_back(lightpath_bandwidth_,
                                                       network_.GetPathVertices(demand->source, demand->destination));
                path.push_back(lp_id);
                virtual_topology_.AddEdge(demand->source, demand->destination, lp_id);
            }
        }

        cur_solution_.Assign(demand, path);
    }
}

void Algorithm::LightpathMin() {
    Solution best_solution = cur_solution_;

    std::vector<size_t> lp_idxes(cur_solution_.lightpaths_.size());
    for (size_t i = 0; i < lp_idxes.size(); ++i) {
        lp_idxes[i] = i;
    }

    std::sort(lp_idxes.begin(), lp_idxes.end(), [this](size_t left, size_t right) {
        return cur_solution_.lightpaths_[left].nodes.size() > cur_solution_.lightpaths_[right].nodes.size();
    });

    for (size_t lp_id: lp_idxes) {
        if (cur_solution_.use_of_lightpaths[lp_id]) {
            if (Grooming(lp_id)) {
                best_solution = cur_solution_;
            } else {
                cur_solution_ = best_solution;
            }
        }
    }

    cur_solution_ = best_solution;
}

bool Algorithm::GroomDemand(std::vector<const TrafficDemand *> &demands, size_t demand_id) {
    if (demand_id == demands.size()) {
        return true;
    }

    const TrafficDemand *demand_ptr = demands[demand_id];
    std::vector<size_t> path = virtual_topology_.GetPathEdges(demand_ptr->source, demand_ptr->destination,
                                                              demand_ptr->bandwidth, is_path_ok_);
    for (size_t lp_id: path) {
        cur_solution_.lightpaths_[lp_id].unused_bandwidth -= demand_ptr->bandwidth;
    }
    if (!path.empty() && GroomDemand(demands, demand_id + 1)) {
        for (size_t lp_id: path) {
            cur_solution_.lightpaths_[lp_id].unused_bandwidth += demand_ptr->bandwidth;
        }
        cur_solution_.Assign(demand_ptr, path);
        return true;
    }
    for (size_t lp_id: path) {
        cur_solution_.lightpaths_[lp_id].unused_bandwidth += demand_ptr->bandwidth;
    }

    return false;
}

bool Algorithm::Grooming(size_t lp_id) {
    virtual_topology_.RemoveEdge(cur_solution_.lightpaths_[lp_id].nodes.front(),
                                 cur_solution_.lightpaths_[lp_id].nodes.back(), lp_id);
    std::vector<const TrafficDemand *> demands_through_lp(cur_solution_.lightpath_demands[lp_id].begin(),
                                                          cur_solution_.lightpath_demands[lp_id].end());
    for (const TrafficDemand *demand_ptr: demands_through_lp) {
        cur_solution_.Unassign(demand_ptr);
    }

    bool groomed = GroomDemand(demands_through_lp);
    if (!groomed) {
        virtual_topology_.AddEdge(cur_solution_.lightpaths_[lp_id].nodes.front(),
                                  cur_solution_.lightpaths_[lp_id].nodes.back(), lp_id);
    }

    return groomed;
}

bool Algorithm::HasEnoughBandwidth(const std::vector<size_t> &path, size_t bandwidth) {
    return std::all_of(path.begin(), path.end(),
                       [this, bandwidth](size_t lp_id) {
                           return cur_solution_.lightpaths_[lp_id].unused_bandwidth >= bandwidth;
                       });
}

bool Algorithm::IsSimple(const std::vector<size_t> &path) {
    std::unordered_set<size_t> nodes;
    if (!path.empty()) {
        nodes.insert(cur_solution_.lightpaths_[path[0]].nodes[0]);
    }
    for (size_t lp_id: path) {
        for (size_t i = 1; i < cur_solution_.lightpaths_[lp_id].nodes.size(); ++i) {
            if (!nodes.insert(cur_solution_.lightpaths_[lp_id].nodes[i]).second) {
                return false;
            }
        }
    }

    return true;
}
