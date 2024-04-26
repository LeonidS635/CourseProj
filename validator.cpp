#include "headers/validator.h"

#include <functional>

Validator::Validator(size_t n, size_t m, size_t lightpath_bandwidth, const Solution &solution, const Graph &network,
                     const std::vector<TrafficDemand> &demands)
        : n_(n), m_(m), l_(solution.lightpaths_.size()), lightpath_bandwidth_(lightpath_bandwidth),
          solution_(solution), demands_(demands), network_(network) {
}

bool Validator::Validate() const {
    auto is_not_overused = [this](const std::vector<size_t> &path, size_t lightpath_bandwidth) {
        return std::all_of(path.begin(), path.end(),
                           [this, lightpath_bandwidth](size_t lp_id) {
                               return solution_.lightpaths_[lp_id].unused_bandwidth < lightpath_bandwidth;
                           });
    };
    auto is_simple = [this](const std::vector<size_t> &path) {
        std::unordered_set<size_t> nodes;
        if (!path.empty()) {
            nodes.insert(solution_.lightpaths_[path[0]].nodes[0]);
        }
        for (size_t lp_id: path) {
            for (size_t i = 1; i < solution_.lightpaths_[lp_id].nodes.size(); ++i) {
                if (!nodes.insert(solution_.lightpaths_[lp_id].nodes[i]).second) {
                    return false;
                }
            }
        }

        return true;
    };

    size_t lightpaths_number = 0;
    for (size_t lp_id = 0; lp_id < solution_.lightpaths_.size(); ++lp_id) {
        if (solution_.use_of_lightpaths[lp_id]) {
            ++lightpaths_number;
        }
    }
    if (solution_.lightpaths_number_ != lightpaths_number) {
        return false;
    }

    for (size_t lp_id = 0; lp_id < solution_.lightpaths_.size(); ++lp_id) {
        if (network_.GetDistance(solution_.lightpaths_[lp_id].nodes.front(),
                                 solution_.lightpaths_[lp_id].nodes.back()) == (SIZE_MAX << 1)) {
            return false;
        }
    }

    for (const TrafficDemand &demand: demands_) {
        std::vector<size_t> path(solution_.demand_lightpaths.at(&demand));
        if (path.empty() || !is_not_overused(path, lightpath_bandwidth_) || !is_simple(path)) {
            return false;
        }
    }

    return true;
}
