#pragma once

#include "graph.h"
#include "structures.h"

class Algorithm {
public:
    Algorithm(size_t n, size_t m, size_t lightpath_bandwidth, const std::vector<TrafficDemand> &traffic_demands,
          const Graph &network);

    Solution Run();

private:
    void Construct();

    void LightpathMin();

    bool GroomDemand(std::vector<const TrafficDemand *> &demands, size_t demand_number = 0);

    bool Grooming(size_t lp_id);

    bool HasEnoughBandwidth(const std::vector<size_t> &path, size_t bandwidth);

    bool IsSimple(const std::vector<size_t> &path);

private:
    size_t n_;
    size_t lightpath_bandwidth_;

    const Graph &network_;
    Graph virtual_topology_;

    std::vector<const TrafficDemand *> traffic_demands_ptrs_;

    Solution cur_solution_;
    Solution best_solution_;

    std::function<bool(const std::vector<size_t> &, size_t)> is_path_ok_;
};
