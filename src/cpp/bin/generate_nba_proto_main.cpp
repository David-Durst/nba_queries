#include <iostream>
#include <fstream>
#include "load_data.h"
#include "query_structs.h"
#include "convert_proto.h"
#include "benchmark.h"
#include "moment.capnp.h"


int main(int argc, char * argv[]) {
    vector<cleaned_moment> moments;
    if (argc != 3) {
        std::cout << "please call this code with 2 arguments: " << std::endl;
        std::cout << "1. path/to/cleaned_moments_file.csv " << std::endl;
        std::cout << "5. path/to/output/cleaned/moments.pb" << std::endl;
        return 0;
    }
    string moments_file_path = argv[1], proto_file_path = argv[2];
    std::fstream moments_file, proto_file;

    // load the cleaned moments
    std::cout << "loading cleaned moments file: " << moments_file_path << std::endl;
    auto start_internal = Halide::Tools::benchmark_now();
    moments_file.open(moments_file_path);
    load_cleaned_moment_rows_vec(moments_file, moments);
    moments_file.close();
    auto time_internal = Halide::Tools::benchmark_duration_seconds(start_internal, Halide::Tools::benchmark_now());
    std::cout << "it took " << time_internal << " seconds to load the data from csv" << std::endl;
    std::cout << "moments size: " << moments.size() << std::endl;
    auto start_proto = Halide::Tools::benchmark_now();
    moments_memory_to_proto(moments, proto_file_path);
    auto time_proto = Halide::Tools::benchmark_duration_seconds(start_proto, Halide::Tools::benchmark_now());
    std::cout << "it took " << time_proto << " seconds to write the data in protobuf" << std::endl;
    return 0;
}