#pragma once

#include <vector>
#include <random>
#include "structures.h"

class Generator {
public:
    Generator();

    void GenerateInput(size_t n, size_t m, size_t &lightpath_bandwidth,
                       std::vector<std::vector<size_t>> &adjacent_matrix, std::vector<TrafficDemand> &demands);

    void GenerateGraph(size_t n, std::vector<std::vector<size_t>> &adj_matrix);

    void GenerateDemands(size_t n, size_t m, size_t &lightpath_bandwidth, std::vector<TrafficDemand> &demands);

private:
    static void InitializeConnectedGraph(size_t n, std::vector<std::vector<size_t>> &adj_matrix);

    void AddRandomEdges(size_t n, std::vector<std::vector<size_t>> &adj_matrix);

private:
    std::mt19937 gen_;
    std::uniform_int_distribution<size_t> distribution_;
};