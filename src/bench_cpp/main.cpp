#include <iostream>
#include <vector>
#include "query_structs.h"
#include "clean_queries.h"
#include "benchmark.h"
#include "storage_structs.h"
using std::vector;

static inline __attribute__((always_inline)) players_in_paint_at_time gen_value(int64_t i) {
    return {i*100l, clock_fixed_point(i*2l), i};
}


int main(int argc, char ** argv) {
    int64_t length = 7821357; 
    std::cout << "size of struct" << sizeof(players_in_paint_at_time) << std::endl;

    auto start_int_array_alloc = Halide::Tools::benchmark_now();
    for (int j = 0; j < 10; j++) {
        int * arr_int = new int[length];
        for (int i = 0; i < length; i++) {
            arr_int[i] = i;
        }
        delete [] arr_int;
    }
    auto time_int_array_alloc = Halide::Tools::benchmark_duration_seconds(start_int_array_alloc, Halide::Tools::benchmark_now());
    std::cout << "it took " << time_int_array_alloc * 1e3 / 10 << "ms to int_array_alloc" << std::endl;



    auto start_int_vec_alloc = Halide::Tools::benchmark_now();
    for (int j = 0; j < 10; j++) {
        auto vec_int = vector<int>();
        vec_int.resize(length);
        for (int i = 0; i < length; i++) {
            vec_int[i] = i;
        }
    }
    auto time_int_vec_alloc = Halide::Tools::benchmark_duration_seconds(start_int_vec_alloc, Halide::Tools::benchmark_now());
    std::cout << "it took " << time_int_vec_alloc * 1e3 / 10 << "ms to int_vec_alloc" << std::endl;



    auto start_int_vec_push = Halide::Tools::benchmark_now();
    for (int j = 0; j < 10; j++) {
        auto push_vec_int = vector<int>();
        for (int i = 0; i < length; i++) {
            push_vec_int.push_back(i);
        }
    }
    auto time_int_vec_push = Halide::Tools::benchmark_duration_seconds(start_int_vec_push, Halide::Tools::benchmark_now());
    std::cout << "it took " << time_int_vec_push * 1e3 / 10 << "ms to int_vec_push" << std::endl;



    auto start_new_struct_array_alloc = Halide::Tools::benchmark_now();
    for (int j = 0; j < 10; j++) {
        auto arr_struct = new players_in_paint_at_time[length];
#pragma omp parallel for
        for (int i = 0; i < length; i++) {
            arr_struct[i] = gen_value(i);
        }
        delete [] arr_struct;
    }
    auto time_new_struct_array_alloc = Halide::Tools::benchmark_duration_seconds(start_new_struct_array_alloc, Halide::Tools::benchmark_now());
    std::cout << "it took " << time_new_struct_array_alloc * 1e3 / 10 << "ms to struct_array_alloc" << std::endl;



    auto start_malloc_struct_array_alloc = Halide::Tools::benchmark_now();
    for (int j = 0; j < 10; j++) {
        auto arr_struct = (players_in_paint_at_time *) malloc(sizeof(players_in_paint_at_time) * length);
#pragma omp parallel for
        for (int i = 0; i < length; i++) {
            arr_struct[i] = gen_value(i);
        }
        free(arr_struct);
    }
    auto time_malloc_struct_array_alloc = Halide::Tools::benchmark_duration_seconds(start_malloc_struct_array_alloc, Halide::Tools::benchmark_now());
    std::cout << "it took " << time_malloc_struct_array_alloc * 1e3 / 10 << "ms to struct_array_alloc" << std::endl;



    auto start_struct_vec_alloc = Halide::Tools::benchmark_now();
    for (int j = 0; j < 10; j++) {
        auto vec_struct = vector<players_in_paint_at_time>();
        vec_struct.resize(length);
#pragma omp parallel for
        for (int i = 0; i < length; i++) {
            vec_struct[i] = gen_value(i);
        }
    }
    auto time_struct_vec_alloc = Halide::Tools::benchmark_duration_seconds(start_struct_vec_alloc, Halide::Tools::benchmark_now());
    std::cout << "it took " << time_struct_vec_alloc * 1e3 / 10 << "ms to struct_vec_alloc" << std::endl;



    auto start_struct_vec_push = Halide::Tools::benchmark_now();
    for (int j = 0; j < 10; j++) {
        auto push_vec = vector<players_in_paint_at_time>();
        for (int i = 0; i < length; i++) {
            push_vec.push_back(gen_value(i));
        }
    }
    auto time_struct_vec_push = Halide::Tools::benchmark_duration_seconds(start_struct_vec_push, Halide::Tools::benchmark_now());
    std::cout << "it took " << time_struct_vec_push * 1e3 / 10 << "ms to struct_vec_push" << std::endl;
    
/*
    auto start_struct_ncv_push = Halide::Tools::benchmark_now();
    auto ncv = noncontiguous_vector<players_in_paint_at_time>(1);
    int64_t * index = &ncv.indices[0];
    for (int i = 0; i < length % 10000; i++) {
        auto tmp_arr = ensure_enough_space(ncv, 10000, 0);
        for (int j = 0; j < length; j++) {
            tmp_arr[*index] = gen_value(i);
            (*index)++;
        }
    }
    auto time_struct_ncv_push = Halide::Tools::benchmark_duration_seconds(start_struct_ncv_push, Halide::Tools::benchmark_now());
    std::cout << "it took " << time_struct_ncv_push * 1e3 << "ms to struct_ncv_push" << std::endl;
*/
    return 0;
}
