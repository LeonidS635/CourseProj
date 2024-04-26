#pragma once

#include <boost/functional/hash.hpp>
#include <functional>
#include <vector>
#include <unordered_set>

class Graph {
public:
    explicit Graph(size_t n, std::vector<std::vector<size_t>> adj_matrix = {});

    std::vector<size_t> GetPathEdges(size_t from, size_t to, size_t bandwidth,
                                     const std::function<bool(const std::vector<size_t> &, size_t)> &is_path_ok) const;
    std::vector<size_t> GetPathVertices(size_t from, size_t to) const;

    size_t GetDistance(size_t source, size_t destination) const;

    void AddEdge(size_t source, size_t destination, size_t id);
    void RemoveEdge(size_t source, size_t destination, size_t id);

private:
    void CalculateDistances();

    bool DFS(std::vector<size_t> &edge_path, std::vector<bool> &visited, size_t cur_v, size_t from, size_t to,
             size_t bandwidth, const std::function<bool(const std::vector<size_t> &, size_t)> &is_path_ok) const;

    size_t n_;
    std::vector<std::unordered_set<std::pair<size_t, size_t>, boost::hash<std::pair<size_t, size_t>>>> adj_list_;
    std::vector<std::vector<size_t>> distances_;
};

