#pragma once

#include <vector>
#include <unordered_map>
#include <unordered_set>

struct TrafficDemand {
    TrafficDemand() = default;
    TrafficDemand(size_t source, size_t destination, size_t bandwidth) : source(source), destination(destination),
                                                                         bandwidth(bandwidth) {
    }

    size_t source = 0;
    size_t destination = 0;
    size_t bandwidth = 0;
};

struct Lightpath {
    Lightpath() = default;
    Lightpath(size_t bandwidth, std::vector<size_t> nodes) : unused_bandwidth(bandwidth), nodes(std::move(nodes)) {
    }

    size_t unused_bandwidth = 0;
    std::vector<size_t> nodes;
};

struct Solution {
    size_t lightpaths_number_ = 0;
    std::vector<Lightpath> lightpaths_;
    std::vector<bool> use_of_lightpaths;
    std::unordered_map<const TrafficDemand *, std::vector<size_t>> demand_lightpaths;
    std::vector<std::unordered_set<const TrafficDemand *>> lightpath_demands;

    void Assign(const TrafficDemand *demand_ptr, const std::vector<size_t> &lightpaths_idxes) {
        for (size_t lp_id: lightpaths_idxes) {
            if (lp_id >= use_of_lightpaths.size()) {
                use_of_lightpaths.push_back(false);
                lightpath_demands.emplace_back();
            }
            if (!use_of_lightpaths[lp_id]) {
                use_of_lightpaths[lp_id] = true;
                ++lightpaths_number_;
            }
            lightpaths_[lp_id].unused_bandwidth -= demand_ptr->bandwidth;
            lightpath_demands[lp_id].insert(demand_ptr);
        }
        demand_lightpaths[demand_ptr] = lightpaths_idxes;
    }

    void Unassign(const TrafficDemand *demand_ptr) {
        for (size_t lp_id: demand_lightpaths[demand_ptr]) {
            lightpaths_[lp_id].unused_bandwidth += demand_ptr->bandwidth;
            lightpath_demands[lp_id].erase(demand_ptr);
            if (lightpath_demands[lp_id].empty()) {
                use_of_lightpaths[lp_id] = false;
                --lightpaths_number_;
            }
        }
        demand_lightpaths[demand_ptr].clear();
    }

    void Reset(size_t lightpath_bandwidth) {
        lightpaths_number_ = 0;
        for (size_t lp_id = 0; lp_id < lightpaths_.size(); ++lp_id) {
            lightpaths_[lp_id].unused_bandwidth = lightpath_bandwidth;
            use_of_lightpaths[lp_id] = false;
            lightpath_demands[lp_id].clear();
        }
    }
};