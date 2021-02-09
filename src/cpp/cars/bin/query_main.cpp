#include <algorithm>
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <thread>
#include <chrono>
#include <numeric>
#include <sstream>
#include <map>
#include <iomanip>
#include "benchmark.h"
#include "car_query_structs.h"
#include "car_load_data.h"
using std::string;
int main(int argc, char * argv[]) {
    int skip_first_n_rows = 10;
    if (argc < 5) {
        std::cout << "please call this code at least 4 arguments: " << std::endl;
        std::cout << "1. path/to/cleaned_moments_file.csv " << std::endl;
        std::cout << "2. path/to/cleaned_moments_file.pb " << std::endl;
        std::cout << "3. number of ticks " << std::endl;
        std::cout << "4. path/to/moments_file1.csv optional/path/to/moments_file2.csv,... " << std::endl;
        return 0;
    }
    string cleaned_moments_csv_file_path = argv[1], cleaned_moments_pb_file_path = argv[2];
    int64_t num_ticks = std::stoi(argv[3]);
    std::vector<string> input_moment_files;
    for (int i = 4; i < argc; i++) {
        input_moment_files.push_back(argv[i]);
    }
    auto start_internal = Halide::Tools::benchmark_now();
    car_col_store car_moments = car_col_store(num_ticks - skip_first_n_rows, input_moment_files.size());
    load_data_csv(input_moment_files, car_moments, skip_first_n_rows);
    auto time_internal = Halide::Tools::benchmark_duration_seconds(start_internal, Halide::Tools::benchmark_now());
    std::cout << "it took " << time_internal << "s to load the data into csv" << std::endl;

}

