#pragma once

#include "graph.h"
#include "structures.h"

class Validator {
public:
    Validator(size_t n, size_t m, size_t lightpath_bandwidth, const Solution &solution, const Graph &network,
              const std::vector<TrafficDemand> &demands);

    bool Validate() const;

private:
    void ConstructMatrices(const Solution &solution, const std::vector<Lightpath> &lightpaths);

    size_t n_;
    size_t m_;
    size_t l_;

    size_t lightpath_bandwidth_;

    const Solution &solution_;
    const std::vector<TrafficDemand> &demands_;
    const Graph &network_;
};
