#include <iostream>
#include "benchmark.h"
#include "benchmark_ispc.h"
#include <unistd.h>
#define VECTOR_LEN 100000000

using namespace ispc;

static double
toBW(int bytes, double sec) {
    return static_cast<double>(bytes) / (1024. * 1024. * 1024.) / sec;
}

static double
toGFLOPS(int ops, double sec) {
    return static_cast<double>(ops) / 1e9 / sec;
}

int main() {
    int * a, * b, * c, * d, * res;
    a = new int[VECTOR_LEN];
    b = new int[VECTOR_LEN];
    c = new int[VECTOR_LEN];
    d = new int[VECTOR_LEN];
    res = new int[VECTOR_LEN];

    int square_bytes_traffic = 3 * VECTOR_LEN * sizeof(int);
    int square_total_ops = VECTOR_LEN;

    double min_time = Halide::Tools::benchmark(2, 2, [&]() {
        square(a, res, VECTOR_LEN);
    });

    std::cout << "square time (s): " << min_time << std::endl;
    std::cout << "square BW: " << toBW(square_bytes_traffic, min_time) << std::endl;
    std::cout << "square GOPS: " << toGFLOPS(square_total_ops, min_time) << std::endl;
}


