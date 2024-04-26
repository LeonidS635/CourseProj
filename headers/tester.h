#pragma once

#include <cstdlib>
#include <utility>

class Tester {
public:
    Tester() = default;

    static void RandomTests();
    static void RingTests();
    static void MeshTests();

private:
    static void RandomTest(size_t n, size_t m, size_t loops_number);
    static void RandomValidationTest(size_t n, size_t m);

    static void RingOneTest();
    static void RingTwoTest();

    static void MeshOneTest();
    static void MeshTwoTest();
    static void MeshThreeTest();
    static void MeshFourTest();
    static void MeshFiveTest();
    static void MeshSixTest();
};
