#include "headers/generator.h"

#include <random>
#include <vector>

Generator::Generator() : gen_(std::random_device{}()) {
}

void Generator::GenerateInput(size_t n, size_t m, size_t &lightpath_bandwidth,
                              std::vector<std::vector<size_t>> &adj_matrix,
                              std::vector<TrafficDemand> &demands) {
    GenerateGraph(n, adj_matrix);
    GenerateDemands(n, m, lightpath_bandwidth, demands);
}

void Generator::InitializeConnectedGraph(size_t n, std::vector<std::vector<size_t>> &adj_matrix) {
    for (size_t i = 0, j = 1; i < n - 1; ++i, ++j) {
        adj_matrix[i][j] = 1;
        adj_matrix[j][i] = 1;
    }
}

void Generator::AddRandomEdges(size_t n, std::vector<std::vector<size_t>> &adj_matrix) {
    distribution_.param(std::uniform_int_distribution<size_t>::param_type(0, 2 * n));
    size_t edges_number = distribution_(gen_);

    distribution_.param(std::uniform_int_distribution<size_t>::param_type(0, n - 1));
    for (size_t i = 0; i < edges_number; ++i) {
        size_t u = distribution_(gen_);
        size_t v = distribution_(gen_);
        if (u != v) {
            adj_matrix[u][v] = 1;
            adj_matrix[v][u] = 1;
        }
    }
}

void Generator::GenerateGraph(size_t n, std::vector<std::vector<size_t>> &adj_matrix) {
    InitializeConnectedGraph(n, adj_matrix);
    AddRandomEdges(n, adj_matrix);
}

void Generator::GenerateDemands(size_t n, size_t m, size_t &lightpath_bandwidth, std::vector<TrafficDemand> &demands) {
    distribution_.param(std::uniform_int_distribution<size_t>::param_type(0, n - 1));

    size_t i = 0;
    while (i < m) {
        size_t source = distribution_(gen_);
        size_t destination = distribution_(gen_);
        if (source != destination) {
            demands[i].source = source;
            demands[i].destination = destination;
            ++i;
        }
    }

    distribution_.param(std::uniform_int_distribution<size_t>::param_type(10, 50));
    lightpath_bandwidth = distribution_(gen_);

    distribution_.param(std::uniform_int_distribution<size_t>::param_type(1, 5));
    for (i = 0; i < m; ++i) {
        demands[i].bandwidth = distribution_(gen_);
    }
}

