#include <iostream>
#include <random>
#include <string>
#include <fstream>
#include <iomanip>
#include "benchmark.h"
#include "benchmark_ispc.h"
#define VECTOR_LEN 100000000

using namespace ispc;
using std::string;

static double
toBW(int64_t bytes, double sec) {
    return static_cast<double>(bytes) / (1024. * 1024. * 1024.) / sec;
}

static double
toGFLOPS(int64_t ops, double sec) {
    return static_cast<double>(ops) / 1e9 / sec;
}

struct func_result {
    double time;
    double bw;
    double gops;
};

string print_func_header(string func_name) {
    return func_name + " Time (ms)," + func_name + " BW (GB/s)," + func_name + " Throughput (GOPS)";
}

std::ostream& operator<<(std::ostream& os, func_result const& res) {
    return os << res.time << "," << res.bw << "," << res.gops;
}

struct results {
    func_result sequential_square;
    func_result parallel_square;
    func_result four_ops_unfused;
    func_result four_ops_fused;
    func_result four_ops_one_input_unfused;
    func_result four_ops_one_input_fused;
};

int main(int argc, char * argv[]) {
    string results_file_path;
    std::fstream results_file;
    results res_struct;

    if (argc != 2) {
        std::cout << "please call this code with 1 argument: " << std::endl;
        std::cout << "1. path/to/result_file.csv " << std::endl;
    }
    results_file_path = argv[1];

    int * a, * b, * c, * d, * res;
    a = new int[VECTOR_LEN];
    b = new int[VECTOR_LEN];
    c = new int[VECTOR_LEN];
    d = new int[VECTOR_LEN];
    res = new int[VECTOR_LEN];

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, 999);

    for (int i=0; i < VECTOR_LEN; i++) {
        a[i] = distrib(gen);
        b[i] = distrib(gen);
        c[i] = distrib(gen);
        d[i] = distrib(gen);
    }

    int square_bytes_traffic = 3 * VECTOR_LEN * sizeof(int);
    int square_total_ops = VECTOR_LEN;

    double min_time = Halide::Tools::benchmark(10, 10, [&]() {
        square_sequential(VECTOR_LEN, a, res);
    });

    res_struct.sequential_square.time = min_time * 1e3;
    res_struct.sequential_square.bw = toBW(square_bytes_traffic, min_time);
    res_struct.sequential_square.gops = toGFLOPS(square_total_ops, min_time);
    std::cout << "sequential square time (s): " << min_time << std::endl;
    std::cout << "sequential square BW: " << toBW(square_bytes_traffic, min_time) << std::endl;
    std::cout << "sequential square GOPS: " << toGFLOPS(square_total_ops, min_time) << std::endl;

    min_time = Halide::Tools::benchmark(10, 10, [&]() {
        square_parallel(VECTOR_LEN, a, res);
    });

    res_struct.parallel_square.time = min_time * 1e3;
    res_struct.parallel_square.bw = toBW(square_bytes_traffic, min_time);
    res_struct.parallel_square.gops = toGFLOPS(square_total_ops, min_time);
    std::cout << "parallel square time (s): " << min_time << std::endl;
    std::cout << "parallel square BW: " << toBW(square_bytes_traffic, min_time) << std::endl;
    std::cout << "parallel square GOPS: " << toGFLOPS(square_total_ops, min_time) << std::endl;


    int64_t four_ops_unfused_bytes_traffic = 10 * VECTOR_LEN * sizeof(int);
    int64_t four_ops_fused_bytes_traffic = 6 * VECTOR_LEN * sizeof(int);
    int64_t four_ops_total_ops = 4*VECTOR_LEN;

    min_time = Halide::Tools::benchmark(10, 10, [&]() {
        four_ops_unfused(VECTOR_LEN, a, b, c, d, res);
    });

    res_struct.four_ops_unfused.time = min_time * 1e3;
    res_struct.four_ops_unfused.bw = toBW(four_ops_unfused_bytes_traffic, min_time);
    res_struct.four_ops_unfused.gops = toGFLOPS(four_ops_total_ops, min_time);
    std::cout << "four ops unfused time (s): " << min_time << std::endl;
    std::cout << "four ops unfused BW: " << toBW(four_ops_unfused_bytes_traffic, min_time) << std::endl;
    std::cout << "four ops unfused GOPS: " << toGFLOPS(four_ops_total_ops, min_time) << std::endl;

    min_time = Halide::Tools::benchmark(10, 10, [&]() {
        four_ops_fused(VECTOR_LEN, a, b, c, d, res);
    });

    res_struct.four_ops_fused.time = min_time * 1e3;
    res_struct.four_ops_fused.bw = toBW(four_ops_fused_bytes_traffic, min_time);
    res_struct.four_ops_fused.gops = toGFLOPS(four_ops_total_ops, min_time);
    std::cout << "four ops fused time (s): " << min_time << std::endl;
    std::cout << "four ops fused BW: " << toBW(four_ops_fused_bytes_traffic, min_time) << std::endl;
    std::cout << "four ops fused GOPS: " << toGFLOPS(four_ops_total_ops, min_time) << std::endl;



    int64_t four_ops_one_input_unfused_bytes_traffic = 9 * VECTOR_LEN * sizeof(int);
    int64_t four_ops_one_input_fused_bytes_traffic = 3 * VECTOR_LEN * sizeof(int);

    min_time = Halide::Tools::benchmark(10, 10, [&]() {
        four_ops_unfused(VECTOR_LEN, a, a, a, a, res);
    });

    res_struct.four_ops_one_input_unfused.time = min_time * 1e3;
    res_struct.four_ops_one_input_unfused.bw = toBW(four_ops_one_input_unfused_bytes_traffic, min_time);
    res_struct.four_ops_one_input_unfused.gops = toGFLOPS(four_ops_total_ops, min_time);
    std::cout << "four ops one input unfused time (s): " << min_time << std::endl;
    std::cout << "four ops one input unfused BW: " << toBW(four_ops_one_input_unfused_bytes_traffic, min_time) << std::endl;
    std::cout << "four ops one input unfused GOPS: " << toGFLOPS(four_ops_total_ops, min_time) << std::endl;

    min_time = Halide::Tools::benchmark(10, 10, [&]() {
        four_ops_fused(VECTOR_LEN, a, a, a, a, res);
    });

    res_struct.four_ops_one_input_fused.time = min_time * 1e3;
    res_struct.four_ops_one_input_fused.bw = toBW(four_ops_one_input_fused_bytes_traffic, min_time);
    res_struct.four_ops_one_input_fused.gops = toGFLOPS(four_ops_total_ops, min_time);
    std::cout << "four ops one input fused time (s): " << min_time << std::endl;
    std::cout << "four ops one input fused BW: " << toBW(four_ops_one_input_fused_bytes_traffic, min_time) << std::endl;
    std::cout << "four ops one input fused GOPS: " << toGFLOPS(four_ops_total_ops, min_time) << std::endl;

    std::cout << "writing to file: " << results_file_path << std::endl;
    results_file.open(results_file_path, std::fstream::out);
    results_file << "Language," + print_func_header("Sequential Square") + "," +
                     print_func_header("Parallel Square") + "," + print_func_header("Four Ops Unfused") + "," +
                     print_func_header("Four Ops Fused") + "," + print_func_header("Four Ops One Input Unfused") + "," +
                     print_func_header("Four Ops One Input Fused") << std::endl;
    results_file << std::fixed << std::setprecision(2)
                 << "ISPC," << res_struct.sequential_square << "," <<
                    res_struct.parallel_square << "," << res_struct.four_ops_unfused << "," <<
                    res_struct.four_ops_fused << "," << res_struct.four_ops_one_input_unfused << "," <<
                    res_struct.four_ops_one_input_fused << std::endl;
    results_file.close();

}


