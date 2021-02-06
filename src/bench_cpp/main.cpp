#include <iostream>
#include <vector>
#include "benchmark.h"
using std::vector;

int main(int argc, char ** argv) {

    auto start_array_alloc = Halide::Tools::benchmark_now();
    int * arr = new int[7821357];
    auto time_array_alloc = Halide::Tools::benchmark_duration_seconds(start_array_alloc, Halide::Tools::benchmark_now());
    std::cout << "it took " << time_array_alloc * 1e3 << "ms to array_alloc" << std::endl;

    auto start_array_fill = Halide::Tools::benchmark_now();
    for (int i = 0; i < 7821357; i++) {
        arr[i] = i;
    }
    auto time_array_fill = Halide::Tools::benchmark_duration_seconds(start_array_fill, Halide::Tools::benchmark_now());
    std::cout << "it took " << time_array_fill * 1e3 << "ms to array_fill" << std::endl;


    auto start_vec_alloc = Halide::Tools::benchmark_now();
    auto vec = vector<int>();
    vec.resize(7821357);
    auto time_vec_alloc = Halide::Tools::benchmark_duration_seconds(start_vec_alloc, Halide::Tools::benchmark_now());
    std::cout << "it took " << time_vec_alloc * 1e3 << "ms to vec_alloc" << std::endl;

    auto start_vec_fill = Halide::Tools::benchmark_now();
    for (int i = 0; i < 7821357; i++) {
        vec[i] = i;
    }
    auto time_vec_fill = Halide::Tools::benchmark_duration_seconds(start_vec_fill, Halide::Tools::benchmark_now());
    std::cout << "it took " << time_vec_fill * 1e3 << "ms to vec_fill" << std::endl;

    return 0;
}
