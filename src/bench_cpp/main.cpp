#include <iostream>
#include <vector>
#include "query_structs.h"
#include "clean_queries.h"
#include "benchmark.h"
using std::vector;

static inline __attribute__((always_inline)) players_in_paint_at_time gen_value(int64_t i) {
    return {i*100l, clock_fixed_point(i*2l), i};
}


int main(int argc, char ** argv) {

    auto start_int_array_alloc = Halide::Tools::benchmark_now();
    int * arr_int = new int[7821357];
    for (int i = 0; i < 7821357; i++) {
        arr_int[i] = i;
    }
    auto time_int_array_alloc = Halide::Tools::benchmark_duration_seconds(start_int_array_alloc, Halide::Tools::benchmark_now());
    std::cout << "it took " << time_int_array_alloc * 1e3 << "ms to int_array_alloc" << std::endl;


    auto start_int_vec_alloc = Halide::Tools::benchmark_now();
    auto vec_int = vector<int>();
    vec_int.resize(7821357);
    for (int i = 0; i < 7821357; i++) {
        vec_int[i] = i;
    }
    auto time_int_vec_alloc = Halide::Tools::benchmark_duration_seconds(start_int_vec_alloc, Halide::Tools::benchmark_now());
    std::cout << "it took " << time_int_vec_alloc * 1e3 << "ms to int_vec_alloc" << std::endl;

    auto start_int_vec_push = Halide::Tools::benchmark_now();
    auto push_vec_int = vector<int>();
    for (int i = 0; i < 7821357; i++) {
        push_vec_int.push_back(i);
    }
    auto time_int_vec_push = Halide::Tools::benchmark_duration_seconds(start_int_vec_push, Halide::Tools::benchmark_now());
    std::cout << "it took " << time_int_vec_push * 1e3 << "ms to int_vec_push" << std::endl;

    auto start_struct_array_alloc = Halide::Tools::benchmark_now();
    auto arr_struct = new players_in_paint_at_time[7821357];
    for (int i = 0; i < 7821357; i++) {
        arr_struct[i] = gen_value(i);
    }
    auto time_struct_array_alloc = Halide::Tools::benchmark_duration_seconds(start_struct_array_alloc, Halide::Tools::benchmark_now());
    std::cout << "it took " << time_struct_array_alloc * 1e3 << "ms to struct_array_alloc" << std::endl;


    auto start_struct_vec_alloc = Halide::Tools::benchmark_now();
    auto vec_struct = vector<players_in_paint_at_time>();
    vec_struct.resize(7821357);
    for (int i = 0; i < 7821357; i++) {
        vec_struct[i] = gen_value(i);
    }
    auto time_struct_vec_alloc = Halide::Tools::benchmark_duration_seconds(start_struct_vec_alloc, Halide::Tools::benchmark_now());
    std::cout << "it took " << time_struct_vec_alloc * 1e3 << "ms to struct_vec_alloc" << std::endl;

    auto start_struct_vec_push = Halide::Tools::benchmark_now();
    auto push_vec = vector<players_in_paint_at_time>();
    for (int i = 0; i < 7821357; i++) {
        push_vec.push_back(gen_value(i));
    }
    auto time_struct_vec_push = Halide::Tools::benchmark_duration_seconds(start_struct_vec_push, Halide::Tools::benchmark_now());
    std::cout << "it took " << time_struct_vec_push * 1e3 << "ms to struct_vec_push" << std::endl;

    return 0;
}
