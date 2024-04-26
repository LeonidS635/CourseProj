#include "headers/tester.h"

#include "headers/generator.h"
#include "headers/algorithm.h"
#include "headers/validator.h"

#include <chrono>
#include <iostream>
#include <set>

void Tester::RandomTests() {
    for (size_t n : {10, 15, 20}) {
        for (size_t m : {15, 50, 100}) {
            size_t loops_number;
            if (m == 15 || m == 50) {
                loops_number = 100;
            } else {
                loops_number = 10;
            }

            RandomValidationTest(n, m);
            RandomTest(n, m, loops_number);
        }
    }
}

void Tester::RandomTest(size_t n, size_t m, size_t loops_number) {
    Generator generator;

    size_t min_lightpaths_number = SIZE_MAX;
    size_t mean_ex_time = 0;
    std::unordered_map<size_t, size_t> lightpaths_numbers_frequency;
    for (size_t i = 0; i < loops_number; ++i) {
        size_t lightpath_bandwidth;
        std::vector<std::vector<size_t>> adj_matrix(n, std::vector<size_t>(n, 0));
        std::vector<TrafficDemand> demands(m);

        generator.GenerateInput(n, m, lightpath_bandwidth, adj_matrix, demands);
        Graph network(n, std::move(adj_matrix));

        Algorithm algorithm(n, m, lightpath_bandwidth, demands, network);

        auto start = std::chrono::high_resolution_clock::now();
        min_lightpaths_number = std::min(algorithm.Run().lightpaths_number_, min_lightpaths_number);
        auto stop = std::chrono::high_resolution_clock::now();

        if (lightpaths_numbers_frequency.find(min_lightpaths_number) == lightpaths_numbers_frequency.end()) {
            lightpaths_numbers_frequency[min_lightpaths_number] = 1;
        } else {
            ++lightpaths_numbers_frequency[min_lightpaths_number];
        }

        size_t ex_time = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count();
        mean_ex_time += ex_time;
    }
    mean_ex_time /= loops_number;

    size_t max_frequency = 0;
    size_t most_frequent_lightpaths_number;
    for (const auto& [lp_number, frequency] : lightpaths_numbers_frequency) {
        if (frequency > max_frequency) {
            max_frequency = frequency;
            most_frequent_lightpaths_number = lp_number;
        } else if (frequency == max_frequency) {
            most_frequent_lightpaths_number = std::min(most_frequent_lightpaths_number, lp_number);
        }
    }

    std::cout << "Results for graph with " << n << " vertices and " << m << " traffic demands:" << std::endl;
    std::cout << "Min lightpaths number:\t\t\t" << min_lightpaths_number << std::endl;
    std::cout << "Most frequent lightpaths number:\t" << most_frequent_lightpaths_number << std::endl;
    std::cout << "Mean execution time:\t\t\t" << mean_ex_time << " milliseconds" << std::endl;
    std::cout << std::string(100, '-') << std::endl;
}

void Tester::RandomValidationTest(size_t n, size_t m) {
    size_t lightpath_bandwidth;
    std::vector<std::vector<size_t>> adj_matrix(n, std::vector<size_t>(n, 0));
    std::vector<TrafficDemand> demands(m);

    Generator generator;
    generator.GenerateInput(n, m, lightpath_bandwidth, adj_matrix, demands);

    Graph network(n, std::move(adj_matrix));

    Algorithm algorithm(n, m, lightpath_bandwidth, demands, network);
    Solution solution = algorithm.Run();

    Validator validator(n, m, lightpath_bandwidth, solution, network, demands);
    bool success = validator.Validate();

    std::cout << "Results of validation for graph with " << n << " vertices and " << m << " traffic demands:" << std::endl;
    std::cout << "Validation:\t" << (success ? "Correct :)" : "Incorrect :(") << std::endl;
    std::cout << std::string(100, '-') << std::endl;
}

void Tester::RingTests() {
    RingOneTest();
    RingTwoTest();
}

void Tester::MeshTests() {
    MeshOneTest();
    MeshTwoTest();
    MeshThreeTest();
    MeshFourTest();
    MeshFiveTest();
    MeshSixTest();
}

void Tester::RingOneTest() {
    size_t n = 8;
    size_t m = 15;
    size_t lightpath_bandwidth = 8;

    std::vector<std::vector<size_t>> adj_matrix {
            {0, 1, 0, 0, 0, 0, 0, 1},
            {1, 0, 1, 0, 0, 0, 0, 0},
            {0, 1, 0, 1, 0, 0, 0, 0},
            {0, 0, 1, 0, 1, 0, 0, 0},
            {0, 0, 0, 1, 0, 1, 0, 0},
            {0, 0, 0, 0, 1, 0, 1, 0},
            {0, 0, 0, 0, 0, 1, 0, 1},
            {1, 0, 0, 0, 0, 0, 1, 0},
    };
    Graph network(n, adj_matrix);

    std::vector<TrafficDemand> demands(m);

    size_t mean_ex_time = 0;
    size_t min_lightpaths_number = SIZE_MAX;
    std::unordered_map<size_t, size_t> lightpaths_numbers_frequency;
    for (size_t i = 0; i < 100; ++i) {
        Generator generator;
        generator.GenerateDemands(n, m, lightpath_bandwidth, demands);
        lightpath_bandwidth = 8;
        for (size_t j = 0; j < m; ++j) {
            demands[j].bandwidth = 1;
        }

        Algorithm algorithm(n, m, lightpath_bandwidth, demands, network);
        Solution solution = algorithm.Run();

        auto start = std::chrono::high_resolution_clock::now();
        min_lightpaths_number = std::min(algorithm.Run().lightpaths_number_, min_lightpaths_number);
        auto stop = std::chrono::high_resolution_clock::now();

        if (lightpaths_numbers_frequency.find(min_lightpaths_number) == lightpaths_numbers_frequency.end()) {
            lightpaths_numbers_frequency[min_lightpaths_number] = 1;
        } else {
            ++lightpaths_numbers_frequency[min_lightpaths_number];
        }

        size_t ex_time = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count();
        mean_ex_time += ex_time;
    }
    mean_ex_time /= 100;

    size_t max_frequency = 0;
    size_t most_frequent_lightpaths_number;
    for (const auto& [lp_number, frequency] : lightpaths_numbers_frequency) {
        if (frequency > max_frequency) {
            max_frequency = frequency;
            most_frequent_lightpaths_number = lp_number;
        } else if (frequency == max_frequency) {
            most_frequent_lightpaths_number = std::min(most_frequent_lightpaths_number, lp_number);
        }
    }

    std::cout << "Results Ring1 test (" << n << " vertices and " << m << " traffic demands):" << std::endl;
    std::cout << "Min lightpaths number:\t\t\t" << min_lightpaths_number << std::endl;
    std::cout << "Most frequent lightpaths number:\t" << most_frequent_lightpaths_number << std::endl;
    std::cout << "Mean execution time:\t\t\t" << mean_ex_time << " milliseconds" << std::endl;
    std::cout << std::string(100, '-') << std::endl;
}

void Tester::RingTwoTest() {
    size_t n = 10;
    size_t m = 15;
    size_t lightpath_bandwidth = 8;

    std::vector<std::vector<size_t>> adj_matrix {
            {0, 1, 0, 0, 0, 0, 0, 0, 0, 1},
            {1, 0, 1, 0, 0, 0, 0, 0, 0, 0},
            {0, 1, 0, 1, 0, 0, 0, 0, 0, 0},
            {0, 0, 1, 0, 1, 0, 0, 0, 0, 0},
            {0, 0, 0, 1, 0, 1, 0, 0, 0, 0},
            {0, 0, 0, 0, 1, 0, 1, 0, 0, 0},
            {0, 0, 0, 0, 0, 1, 0, 1, 0, 0},
            {0, 0, 0, 0, 0, 0, 1, 0, 1, 0},
            {0, 0, 0, 0, 0, 0, 0, 1, 0, 1},
            {1, 0, 0, 0, 0, 0, 0, 0, 1, 0},
    };
    Graph network(n, adj_matrix);

    std::vector<TrafficDemand> demands(m);

    size_t mean_ex_time = 0;
    size_t min_lightpaths_number = SIZE_MAX;
    std::unordered_map<size_t, size_t> lightpaths_numbers_frequency;
    for (size_t i = 0; i < 100; ++i) {
        Generator generator;
        generator.GenerateDemands(n, m, lightpath_bandwidth, demands);
        lightpath_bandwidth = 8;
        for (size_t j = 0; j < m; ++j) {
            demands[j].bandwidth = 1;
        }

        Algorithm algorithm(n, m, lightpath_bandwidth, demands, network);
        Solution solution = algorithm.Run();

        auto start = std::chrono::high_resolution_clock::now();
        min_lightpaths_number = std::min(algorithm.Run().lightpaths_number_, min_lightpaths_number);
        auto stop = std::chrono::high_resolution_clock::now();

        if (lightpaths_numbers_frequency.find(min_lightpaths_number) == lightpaths_numbers_frequency.end()) {
            lightpaths_numbers_frequency[min_lightpaths_number] = 1;
        } else {
            ++lightpaths_numbers_frequency[min_lightpaths_number];
        }

        size_t ex_time = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count();
        mean_ex_time += ex_time;
    }
    mean_ex_time /= 100;

    size_t max_frequency = 0;
    size_t most_frequent_lightpaths_number;
    for (const auto& [lp_number, frequency] : lightpaths_numbers_frequency) {
        if (frequency > max_frequency) {
            max_frequency = frequency;
            most_frequent_lightpaths_number = lp_number;
        } else if (frequency == max_frequency) {
            most_frequent_lightpaths_number = std::min(most_frequent_lightpaths_number, lp_number);
        }
    }

    std::cout << "Results Ring2 test (" << n << " vertices and " << m << " traffic demands):" << std::endl;
    std::cout << "Min lightpaths number:\t\t\t" << min_lightpaths_number << std::endl;
    std::cout << "Most frequent lightpaths number:\t" << most_frequent_lightpaths_number << std::endl;
    std::cout << "Mean execution time:\t\t\t" << mean_ex_time << " milliseconds" << std::endl;
    std::cout << std::string(100, '-') << std::endl;
}

void Tester::MeshOneTest() {
    size_t n = 10;
    size_t m = 20;
    size_t lightpath_bandwidth = 8;

    std::vector<std::vector<size_t>> adj_matrix {
            {0, 1, 0, 0, 0, 0, 0, 1, 1, 0},
            {1, 0, 1, 0, 0, 0, 0, 0, 0, 0},
            {0, 1, 0, 1, 0, 0, 0, 0, 0, 0},
            {0, 0, 1, 0, 1, 0, 0, 0, 0, 0},
            {0, 0, 0, 1, 0, 1, 0, 0, 0, 0},
            {0, 0, 0, 0, 1, 0, 1, 0, 0, 1},
            {0, 0, 0, 0, 0, 1, 0, 1, 0, 0},
            {1, 0, 0, 0, 0, 0, 1, 0, 1, 0},
            {1, 0, 0, 0, 0, 0, 0, 1, 0, 1},
            {0, 0, 0, 0, 0, 1, 0, 0, 1, 0},
    };
    Graph network(n, adj_matrix);

    std::vector<TrafficDemand> demands(m);

    size_t mean_ex_time = 0;
    size_t min_lightpaths_number = SIZE_MAX;
    std::unordered_map<size_t, size_t> lightpaths_numbers_frequency;
    for (size_t i = 0; i < 100; ++i) {
        Generator generator;
        generator.GenerateDemands(n, m, lightpath_bandwidth, demands);
        lightpath_bandwidth = 8;
        for (size_t j = 0; j < m; ++j) {
            demands[j].bandwidth = 1;
        }

        Algorithm algorithm(n, m, lightpath_bandwidth, demands, network);
        Solution solution = algorithm.Run();

        auto start = std::chrono::high_resolution_clock::now();
        min_lightpaths_number = std::min(algorithm.Run().lightpaths_number_, min_lightpaths_number);
        auto stop = std::chrono::high_resolution_clock::now();

        if (lightpaths_numbers_frequency.find(min_lightpaths_number) == lightpaths_numbers_frequency.end()) {
            lightpaths_numbers_frequency[min_lightpaths_number] = 1;
        } else {
            ++lightpaths_numbers_frequency[min_lightpaths_number];
        }

        size_t ex_time = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count();
        mean_ex_time += ex_time;
    }
    mean_ex_time /= 100;

    size_t max_frequency = 0;
    size_t most_frequent_lightpaths_number;
    for (const auto& [lp_number, frequency] : lightpaths_numbers_frequency) {
        if (frequency > max_frequency) {
            max_frequency = frequency;
            most_frequent_lightpaths_number = lp_number;
        } else if (frequency == max_frequency) {
            most_frequent_lightpaths_number = std::min(most_frequent_lightpaths_number, lp_number);
        }
    }

    std::cout << "Results Mesh1 test (" << n << " vertices and " << m << " traffic demands):" << std::endl;
    std::cout << "Min lightpaths number:\t\t\t" << min_lightpaths_number << std::endl;
    std::cout << "Most frequent lightpaths number:\t" << most_frequent_lightpaths_number << std::endl;
    std::cout << "Mean execution time:\t\t\t" << mean_ex_time << " milliseconds" << std::endl;
    std::cout << std::string(100, '-') << std::endl;
}

void Tester::MeshTwoTest() {
    size_t n = 10;
    size_t m = 40;
    size_t lightpath_bandwidth = 8;

    std::vector<std::vector<size_t>> adj_matrix {
            {0, 1, 0, 0, 0, 0, 0, 1, 1, 0},
            {1, 0, 1, 0, 0, 0, 0, 0, 1, 0},
            {0, 1, 0, 1, 1, 0, 0, 0, 0, 0},
            {0, 0, 1, 0, 1, 0, 0, 0, 0, 0},
            {0, 0, 1, 1, 0, 1, 0, 0, 0, 1},
            {0, 0, 0, 0, 1, 0, 1, 0, 0, 1},
            {0, 0, 0, 0, 0, 1, 0, 1, 0, 0},
            {1, 0, 0, 0, 0, 0, 1, 0, 1, 0},
            {1, 1, 0, 0, 0, 0, 0, 1, 0, 1},
            {0, 0, 0, 0, 1, 1, 0, 0, 1, 0},
    };
    Graph network(n, adj_matrix);

    std::vector<TrafficDemand> demands(m);

    size_t mean_ex_time = 0;
    size_t min_lightpaths_number = SIZE_MAX;
    std::unordered_map<size_t, size_t> lightpaths_numbers_frequency;
    for (size_t i = 0; i < 100; ++i) {
        Generator generator;
        generator.GenerateDemands(n, m, lightpath_bandwidth, demands);
        lightpath_bandwidth = 8;
        for (size_t j = 0; j < m; ++j) {
            demands[j].bandwidth = 1;
        }

        Algorithm algorithm(n, m, lightpath_bandwidth, demands, network);
        Solution solution = algorithm.Run();

        auto start = std::chrono::high_resolution_clock::now();
        min_lightpaths_number = std::min(algorithm.Run().lightpaths_number_, min_lightpaths_number);
        auto stop = std::chrono::high_resolution_clock::now();

        if (lightpaths_numbers_frequency.find(min_lightpaths_number) == lightpaths_numbers_frequency.end()) {
            lightpaths_numbers_frequency[min_lightpaths_number] = 1;
        } else {
            ++lightpaths_numbers_frequency[min_lightpaths_number];
        }

        size_t ex_time = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count();
        mean_ex_time += ex_time;
    }
    mean_ex_time /= 100;

    size_t max_frequency = 0;
    size_t most_frequent_lightpaths_number;
    for (const auto& [lp_number, frequency] : lightpaths_numbers_frequency) {
        if (frequency > max_frequency) {
            max_frequency = frequency;
            most_frequent_lightpaths_number = lp_number;
        } else if (frequency == max_frequency) {
            most_frequent_lightpaths_number = std::min(most_frequent_lightpaths_number, lp_number);
        }
    }

    std::cout << "Results Mesh2 test (" << n << " vertices and " << m << " traffic demands):" << std::endl;
    std::cout << "Min lightpaths number:\t\t\t" << min_lightpaths_number << std::endl;
    std::cout << "Most frequent lightpaths number:\t" << most_frequent_lightpaths_number << std::endl;
    std::cout << "Mean execution time:\t\t\t" << mean_ex_time << " milliseconds" << std::endl;
    std::cout << std::string(100, '-') << std::endl;
}

void Tester::MeshThreeTest() {
    size_t n = 10;
    size_t m = 50;
    size_t lightpath_bandwidth = 8;

    std::vector<std::vector<size_t>> adj_matrix {
            {0, 1, 0, 0, 0, 0, 0, 1, 1, 0},
            {1, 0, 1, 0, 0, 0, 0, 0, 1, 0},
            {0, 1, 0, 1, 0, 0, 1, 0, 0, 0},
            {0, 0, 1, 0, 1, 0, 0, 0, 0, 0},
            {0, 0, 0, 1, 0, 1, 0, 0, 0, 1},
            {0, 0, 0, 0, 1, 0, 1, 0, 0, 1},
            {0, 0, 1, 0, 0, 1, 0, 1, 0, 0},
            {1, 0, 0, 0, 0, 0, 1, 0, 1, 0},
            {1, 1, 0, 0, 0, 0, 0, 1, 0, 1},
            {0, 0, 0, 0, 1, 1, 0, 0, 1, 0},
    };
    Graph network(n, adj_matrix);

    std::vector<TrafficDemand> demands(m);

    size_t mean_ex_time = 0;
    size_t min_lightpaths_number = SIZE_MAX;
    std::unordered_map<size_t, size_t> lightpaths_numbers_frequency;
    for (size_t i = 0; i < 100; ++i) {
        Generator generator;
        generator.GenerateDemands(n, m, lightpath_bandwidth, demands);
        lightpath_bandwidth = 8;
        for (size_t j = 0; j < m; ++j) {
            demands[j].bandwidth = 1;
        }

        Algorithm algorithm(n, m, lightpath_bandwidth, demands, network);
        Solution solution = algorithm.Run();

        auto start = std::chrono::high_resolution_clock::now();
        min_lightpaths_number = std::min(algorithm.Run().lightpaths_number_, min_lightpaths_number);
        auto stop = std::chrono::high_resolution_clock::now();

        if (lightpaths_numbers_frequency.find(min_lightpaths_number) == lightpaths_numbers_frequency.end()) {
            lightpaths_numbers_frequency[min_lightpaths_number] = 1;
        } else {
            ++lightpaths_numbers_frequency[min_lightpaths_number];
        }

        size_t ex_time = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count();
        mean_ex_time += ex_time;
    }
    mean_ex_time /= 100;

    size_t max_frequency = 0;
    size_t most_frequent_lightpaths_number;
    for (const auto& [lp_number, frequency] : lightpaths_numbers_frequency) {
        if (frequency > max_frequency) {
            max_frequency = frequency;
            most_frequent_lightpaths_number = lp_number;
        } else if (frequency == max_frequency) {
            most_frequent_lightpaths_number = std::min(most_frequent_lightpaths_number, lp_number);
        }
    }

    std::cout << "Results Mesh3 test (" << n << " vertices and " << m << " traffic demands):" << std::endl;
    std::cout << "Min lightpaths number:\t\t\t" << min_lightpaths_number << std::endl;
    std::cout << "Most frequent lightpaths number:\t" << most_frequent_lightpaths_number << std::endl;
    std::cout << "Mean execution time:\t\t\t" << mean_ex_time << " milliseconds" << std::endl;
    std::cout << std::string(100, '-') << std::endl;
}

void Tester::MeshFourTest() {
    size_t n = 10;
    size_t m = 60;
    size_t lightpath_bandwidth = 8;

    std::vector<std::vector<size_t>> adj_matrix {
            {0, 1, 0, 0, 0, 0, 0, 1, 1, 0},
            {1, 0, 1, 0, 0, 0, 0, 0, 0, 0},
            {0, 1, 0, 1, 0, 0, 0, 0, 0, 0},
            {0, 0, 1, 0, 1, 0, 0, 0, 0, 0},
            {0, 0, 0, 1, 0, 1, 0, 0, 0, 0},
            {0, 0, 0, 0, 1, 0, 1, 0, 0, 1},
            {0, 0, 0, 0, 0, 1, 0, 1, 0, 0},
            {1, 0, 0, 0, 0, 0, 1, 0, 1, 0},
            {1, 0, 0, 0, 0, 0, 0, 1, 0, 1},
            {0, 0, 0, 0, 0, 1, 0, 0, 1, 0},
    };
    Graph network(n, adj_matrix);

    std::vector<TrafficDemand> demands(m);

    size_t mean_ex_time = 0;
    size_t min_lightpaths_number = SIZE_MAX;
    std::unordered_map<size_t, size_t> lightpaths_numbers_frequency;
    for (size_t i = 0; i < 100; ++i) {
        Generator generator;
        generator.GenerateDemands(n, m, lightpath_bandwidth, demands);
        lightpath_bandwidth = 8;
        for (size_t j = 0; j < m; ++j) {
            demands[j].bandwidth = 1;
        }

        Algorithm algorithm(n, m, lightpath_bandwidth, demands, network);
        Solution solution = algorithm.Run();

        auto start = std::chrono::high_resolution_clock::now();
        min_lightpaths_number = std::min(algorithm.Run().lightpaths_number_, min_lightpaths_number);
        auto stop = std::chrono::high_resolution_clock::now();

        if (lightpaths_numbers_frequency.find(min_lightpaths_number) == lightpaths_numbers_frequency.end()) {
            lightpaths_numbers_frequency[min_lightpaths_number] = 1;
        } else {
            ++lightpaths_numbers_frequency[min_lightpaths_number];
        }

        size_t ex_time = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count();
        mean_ex_time += ex_time;
    }
    mean_ex_time /= 100;

    size_t max_frequency = 0;
    size_t most_frequent_lightpaths_number;
    for (const auto& [lp_number, frequency] : lightpaths_numbers_frequency) {
        if (frequency > max_frequency) {
            max_frequency = frequency;
            most_frequent_lightpaths_number = lp_number;
        } else if (frequency == max_frequency) {
            most_frequent_lightpaths_number = std::min(most_frequent_lightpaths_number, lp_number);
        }
    }

    std::cout << "Results Mesh4 test (" << n << " vertices and " << m << " traffic demands):" << std::endl;
    std::cout << "Min lightpaths number:\t\t\t" << min_lightpaths_number << std::endl;
    std::cout << "Most frequent lightpaths number:\t" << most_frequent_lightpaths_number << std::endl;
    std::cout << "Mean execution time:\t\t\t" << mean_ex_time << " milliseconds" << std::endl;
    std::cout << std::string(100, '-') << std::endl;
}

void Tester::MeshFiveTest() {
    size_t n = 10;
    size_t m = 70;
    size_t lightpath_bandwidth = 8;

    std::vector<std::vector<size_t>> adj_matrix {
            {0, 1, 0, 0, 0, 0, 0, 1, 1, 0},
            {1, 0, 1, 0, 0, 0, 0, 0, 0, 0},
            {0, 1, 0, 1, 0, 0, 0, 0, 0, 0},
            {0, 0, 1, 0, 1, 0, 0, 0, 0, 0},
            {0, 0, 0, 1, 0, 1, 0, 0, 0, 0},
            {0, 0, 0, 0, 1, 0, 1, 0, 0, 1},
            {0, 0, 0, 0, 0, 1, 0, 1, 0, 0},
            {1, 0, 0, 0, 0, 0, 1, 0, 1, 0},
            {1, 0, 0, 0, 0, 0, 0, 1, 0, 1},
            {0, 0, 0, 0, 0, 1, 0, 0, 1, 0},
    };
    Graph network(n, adj_matrix);

    std::vector<TrafficDemand> demands(m);

    size_t mean_ex_time = 0;
    size_t min_lightpaths_number = SIZE_MAX;
    std::unordered_map<size_t, size_t> lightpaths_numbers_frequency;
    for (size_t i = 0; i < 100; ++i) {
        Generator generator;
        generator.GenerateDemands(n, m, lightpath_bandwidth, demands);
        lightpath_bandwidth = 8;
        for (size_t j = 0; j < m; ++j) {
            demands[j].bandwidth = 1;
        }

        Algorithm algorithm(n, m, lightpath_bandwidth, demands, network);
        Solution solution = algorithm.Run();

        auto start = std::chrono::high_resolution_clock::now();
        min_lightpaths_number = std::min(algorithm.Run().lightpaths_number_, min_lightpaths_number);
        auto stop = std::chrono::high_resolution_clock::now();

        if (lightpaths_numbers_frequency.find(min_lightpaths_number) == lightpaths_numbers_frequency.end()) {
            lightpaths_numbers_frequency[min_lightpaths_number] = 1;
        } else {
            ++lightpaths_numbers_frequency[min_lightpaths_number];
        }

        size_t ex_time = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count();
        mean_ex_time += ex_time;
    }
    mean_ex_time /= 100;

    size_t max_frequency = 0;
    size_t most_frequent_lightpaths_number;
    for (const auto& [lp_number, frequency] : lightpaths_numbers_frequency) {
        if (frequency > max_frequency) {
            max_frequency = frequency;
            most_frequent_lightpaths_number = lp_number;
        } else if (frequency == max_frequency) {
            most_frequent_lightpaths_number = std::min(most_frequent_lightpaths_number, lp_number);
        }
    }

    std::cout << "Results Mesh5 test (" << n << " vertices and " << m << " traffic demands):" << std::endl;
    std::cout << "Min lightpaths number:\t\t\t" << min_lightpaths_number << std::endl;
    std::cout << "Most frequent lightpaths number:\t" << most_frequent_lightpaths_number << std::endl;
    std::cout << "Mean execution time:\t\t\t" << mean_ex_time << " milliseconds" << std::endl;
    std::cout << std::string(100, '-') << std::endl;
}

void Tester::MeshSixTest() {
    size_t n = 10;
    size_t m = 70;
    size_t lightpath_bandwidth = 8;

    std::vector<std::vector<size_t>> adj_matrix {
            {0, 1, 0, 0, 0, 0, 0, 1, 1, 0},
            {1, 0, 1, 0, 0, 0, 0, 0, 0, 0},
            {0, 1, 0, 1, 0, 0, 1, 0, 0, 0},
            {0, 0, 1, 0, 1, 0, 0, 0, 0, 0},
            {0, 0, 0, 1, 0, 1, 0, 0, 0, 0},
            {0, 0, 0, 0, 1, 0, 1, 0, 0, 1},
            {0, 0, 1, 0, 0, 1, 0, 1, 0, 0},
            {1, 0, 0, 0, 0, 0, 1, 0, 1, 0},
            {1, 0, 0, 0, 0, 0, 0, 1, 0, 1},
            {0, 0, 0, 0, 0, 1, 0, 0, 1, 0},
    };
    Graph network(n, adj_matrix);

    std::vector<TrafficDemand> demands(m);

    size_t mean_ex_time = 0;
    size_t min_lightpaths_number = SIZE_MAX;
    std::unordered_map<size_t, size_t> lightpaths_numbers_frequency;
    for (size_t i = 0; i < 100; ++i) {
        Generator generator;
        generator.GenerateDemands(n, m, lightpath_bandwidth, demands);
        lightpath_bandwidth = 8;
        for (size_t j = 0; j < m; ++j) {
            demands[j].bandwidth = 1;
        }

        Algorithm algorithm(n, m, lightpath_bandwidth, demands, network);
        Solution solution = algorithm.Run();

        auto start = std::chrono::high_resolution_clock::now();
        min_lightpaths_number = std::min(algorithm.Run().lightpaths_number_, min_lightpaths_number);
        auto stop = std::chrono::high_resolution_clock::now();

        if (lightpaths_numbers_frequency.find(min_lightpaths_number) == lightpaths_numbers_frequency.end()) {
            lightpaths_numbers_frequency[min_lightpaths_number] = 1;
        } else {
            ++lightpaths_numbers_frequency[min_lightpaths_number];
        }

        size_t ex_time = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count();
        mean_ex_time += ex_time;
    }
    mean_ex_time /= 100;

    size_t max_frequency = 0;
    size_t most_frequent_lightpaths_number;
    for (const auto& [lp_number, frequency] : lightpaths_numbers_frequency) {
        if (frequency > max_frequency) {
            max_frequency = frequency;
            most_frequent_lightpaths_number = lp_number;
        } else if (frequency == max_frequency) {
            most_frequent_lightpaths_number = std::min(most_frequent_lightpaths_number, lp_number);
        }
    }

    std::cout << "Results Mesh6 test (" << n << " vertices and " << m << " traffic demands):" << std::endl;
    std::cout << "Min lightpaths number:\t\t\t" << min_lightpaths_number << std::endl;
    std::cout << "Most frequent lightpaths number:\t" << most_frequent_lightpaths_number << std::endl;
    std::cout << "Mean execution time:\t\t\t" << mean_ex_time << " milliseconds" << std::endl;
    std::cout << std::string(100, '-') << std::endl;
}

