#include "headers/graph.h"

#include <algorithm>
#include <climits>
#include <queue>

Graph::Graph(size_t n, std::vector<std::vector<size_t>> adj_matrix)
        : n_(n),
          adj_list_(std::vector<std::unordered_set<std::pair<size_t, size_t>, boost::hash<std::pair<size_t, size_t>>>>(
                  n)),
          distances_(std::vector<std::vector<size_t>>(n, std::vector<size_t>(n, SIZE_MAX << 1))) {
    if (!adj_matrix.empty()) {
        for (size_t i = 0; i < n_; ++i) {
            distances_[i][i] = 0;
            for (size_t j = i + 1; j < n_; ++j) {
                if (adj_matrix[i][j]) {
                    distances_[i][j] = 1;
                    distances_[j][i] = 1;

                    adj_list_[i].emplace(j, 0);
                    adj_list_[j].emplace(i, 0);
                }
            }
        }

        CalculateDistances();
    }
}

bool Graph::DFS(std::vector<size_t> &edge_path, std::vector<bool> &visited, size_t cur_v, size_t from,
                size_t to, size_t bandwidth,
                const std::function<bool(const std::vector<size_t> &, size_t)> &is_path_ok) const {
    visited[cur_v] = true;

    if (cur_v == to) {
        return is_path_ok(edge_path, bandwidth);
    }

    for (auto [neighbour, edge_number] : adj_list_[cur_v]) {
        if (!visited[neighbour]) {
            edge_path.push_back(edge_number);
            if (DFS(edge_path, visited, neighbour, from, to, bandwidth, is_path_ok)) {
                return true;
            }
            edge_path.pop_back();
            visited[neighbour] = false;
        }
    }

    visited[cur_v] = false;

    return false;
}

std::vector<size_t> Graph::GetPathEdges(size_t from, size_t to, size_t bandwidth,
                                            const std::function<bool(const std::vector<size_t> &, size_t)> &is_path_ok) const {
    std::vector<bool> visited(n_, false);
    std::vector<size_t> path;
    DFS(path, visited, from, from, to, bandwidth, is_path_ok);
    return path;
}

std::vector<size_t> Graph::GetPathVertices(size_t from, size_t to) const {
    std::vector<size_t> dist(n_, SIZE_MAX);
    std::vector<size_t> parents(n_, SIZE_MAX);
    std::priority_queue<std::pair<size_t, size_t>, std::vector<std::pair<size_t, size_t>>, std::greater<>> queue;

    dist[from] = 0;
    queue.emplace(0, from);

    while (!queue.empty()) {
        size_t cur_dist = queue.top().first;
        size_t cur_vertex = queue.top().second;
        queue.pop();

        if (cur_dist > dist[cur_vertex]) {
            continue;
        }

        for (const auto &edge: adj_list_[cur_vertex]) {
            size_t neighbor = edge.first;

            if (dist[neighbor] > dist[cur_vertex] + 1) {
                dist[neighbor] = dist[cur_vertex] + 1;
                parents[neighbor] = cur_vertex;
                queue.emplace(dist[neighbor], neighbor);
            }
        }
    }

    std::vector<size_t> path;
    if (dist[to] == SIZE_MAX) {
        return path;
    }

    for (size_t v = to; v != SIZE_MAX; v = parents[v]) {
        path.push_back(v);
    }
    std::reverse(path.begin(), path.end());

    return path;
}

size_t Graph::GetDistance(size_t source, size_t destination) const {
    return distances_[source][destination];
}

void Graph::AddEdge(size_t source, size_t destination, size_t id) {
    adj_list_[source].emplace(destination, id);
    adj_list_[destination].emplace(source, id);
}

void Graph::RemoveEdge(size_t source, size_t destination, size_t id) {
    adj_list_[source].erase({destination, id});
    adj_list_[destination].erase({source, id});
}

void Graph::CalculateDistances() {
    for (size_t i = 0; i < n_; ++i) {
        for (size_t u = 0; u < n_; ++u) {
            for (size_t v = 0; v < n_; ++v) {
                distances_[u][v] = std::min(distances_[u][v], distances_[u][i] + distances_[i][v]);
            }
        }
    }
}
