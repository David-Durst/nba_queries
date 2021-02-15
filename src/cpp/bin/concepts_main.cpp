#include <algorithm>
#include <iostream>
#include <iomanip>
#include <vector>
#include <fstream>
#include <string>
#include <thread>
#include <chrono>
#include <numeric>
#include <limits>
#include <map>
#include <sys/types.h>
#include <omp.h>
#include <unistd.h>
#include <csignal>
#include <functional>
#include "check_distances.h"
#include "find_trajectories.h"
#include "clean_queries.h"
#include "benchmark.h"
#include "load_data.h"
#include "query_structs.h"
#include "col_stores.h"
#include "convert_proto.h"
#include "storage_structs.h"
#include "concepts.h"
#ifdef CALLGRIND
#include <valgrind/callgrind.h>
#endif
using std::string;
using std::placeholders::_1;
using std::placeholders::_2;

struct results {
    double possession_time;
    double stoppage_time;
};

int main(int argc, char * argv[]) {
    vector<cleaned_moment> moments;
    moment_col_store *moments_col;
    vector<cleaned_shot> shots;
    shot_col_store *shots_col;
    vector<extra_game_data> extra_data;
    Possession possession;
    Stoppage stoppage;
    int num_threads = omp_get_max_threads();
    results res;
    if (argc != 7) {
        std::cout << "please call this code with 6 arguments: " << std::endl;
        std::cout << "1. path/to/cleaned_moments_file.csv " << std::endl;
        std::cout << "2. path/to/shots_file.csv " << std::endl;
        std::cout << "3. debug/measure " << std::endl;
        std::cout << "4. path/to/extra_data_file.csv " << std::endl;
        std::cout << "5. path/to/output/timing/file.csv " << std::endl;
        std::cout << "6. path/to/output/samples/ " << std::endl;
    }
    string moments_file_path = argv[1], shots_file_path = argv[2], run_type = argv[3], extra_data_file_path = argv[4],
            timing_file_path = argv[5], samples_dir_path = argv[6];
    int num_samples_and_iterations = (run_type.compare("debug") == 0) ? 1 : 5;
    std::fstream shots_file, timing_file, extra_data_file;

    // load the cleaned moments
    std::cout << "loading cleaned moments file: " << moments_file_path << std::endl;
    auto start_proto = Halide::Tools::benchmark_now();
    moments_proto_to_memory(moments_file_path, moments);
    auto time_proto = Halide::Tools::benchmark_duration_seconds(start_proto, Halide::Tools::benchmark_now());
    std::cout << "moments size: " << moments.size() << std::endl;
    std::cout << "it took " << time_proto << " seconds to read the data in protobuf" << std::endl;

    auto start_col = Halide::Tools::benchmark_now();
    moments_col = new moment_col_store(moments);
    auto time_col = Halide::Tools::benchmark_duration_seconds(start_col, Halide::Tools::benchmark_now());
    std::cout << "it took " << time_col << " seconds to convert to cols" << std::endl;

    // load the shots
    std::cout << "loading shots file: " << shots_file_path << std::endl;
    shots_file.open(shots_file_path);
    load_cleaned_shot_rows_vec(shots_file, shots);
    shots_file.close();
    std::cout << "shots size: " << shots.size() << std::endl;
    shots_col = new shot_col_store(shots);

    // load the extra game data
    std::cout << "loading extra game data file: " << extra_data_file_path << std::endl;
    extra_data_file.open(extra_data_file_path);
    load_extra_game_data_rows(extra_data_file, extra_data);
    extra_data_file.close();
    std::cout << "extra game data size: " << extra_data.size() << std::endl;

    std::cout << "running possession concept" << std::endl;
    double min_time = Halide::Tools::benchmark_with_cleanup(num_samples_and_iterations, num_samples_and_iterations, [&]() {
        possession.compute(*moments_col, *shots_col);
    }, [&]() {
        for (int i = 0; i < num_threads; i++) {
            possession.start_moment_index_unmerged[i].clear();
            possession.possessor_ids_unmerged[i].clear();
            possession.possessor_team_unmerged[i].clear();
        }
    });
    possession.compute(*moments_col, *shots_col);
    std::cout << "possession concept took " << min_time << "s" << std::endl;

    possession.sample(*moments_col, 100, true, samples_dir_path + "/possession.csv",
                      std::bind(&Possession::allow_all, possession, _1, _2));

    std::cout << "running stoppage concept" << std::endl;
    min_time = Halide::Tools::benchmark_with_cleanup(num_samples_and_iterations, num_samples_and_iterations, [&]() {
        stoppage.compute(*moments_col, *shots_col);
    }, [&]() {
        free(stoppage.start_moment_index);
        free(stoppage.is_window_stoppage);
    });
    stoppage.compute(*moments_col, *shots_col);
    std::cout << "stoppage concept took " << min_time << "s" << std::endl;
    stoppage.sample(*moments_col, 100, false, samples_dir_path + "/is_stoppage.csv",
                    std::bind(&Stoppage::get_stoppages, stoppage, _1, _2));
    stoppage.sample(*moments_col, 100, false, samples_dir_path + "/is_not_stoppage.csv",
                    std::bind(&Stoppage::get_non_stoppages, stoppage, _1, _2));
}