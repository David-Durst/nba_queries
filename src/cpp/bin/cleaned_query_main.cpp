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
#include "check_distances.h"
#include "find_trajectories.h"
#include "clean_queries.h"
#include "benchmark.h"
#include "load_data.h"
#include "query_structs.h"
#include "col_stores.h"
#ifdef CALLGRIND
#include <valgrind/callgrind.h>
#endif
using std::string;

struct results {
    double query1_rowstore_sequential_time;
    double query1_colstore_sequential_time;
    double query1_colstore_parallel_time;
    double query3_rowstore_sequential_time;
    double query3_colstore_sequential_time;
    double query3_colstore_parallel_time;
    double query3_binned_colstore_sequential_time;
    double query3_binned_colstore_parallel_time;
};

int main(int argc, char * argv[]) {
    vector<cleaned_moment> moments;
    moment_col_store * moments_col;
    vector<cleaned_shot> shots;
    shot_col_store * shots_col;
    court_bins * bins;
    vector<shot_and_player_data> shots_and_players_seq, shots_and_players_par;
    list<shot_and_player_data> shots_and_players_list;
    vector<trajectory_data> trajectories;
    list<trajectory_data> trajectories_list;
    vector<extra_game_data> extra_data;
    results res;
    if (argc != 6) {
        std::cout << "please call this code with 4 arguments: " << std::endl;
        std::cout << "1. path/to/cleaned_moments_file.csv " << std::endl;
        std::cout << "2. path/to/shots_file.csv " << std::endl;
        std::cout << "3. debug/measure " << std::endl;
        std::cout << "4. path/to/extra_data_file.csv " << std::endl;
        std::cout << "5. path/to/output/timing/file.csv " << std::endl;
    }
    string moments_file_path = argv[1], shots_file_path = argv[2], run_type = argv[3], extra_data_file_path = argv[4],
        timing_file_path = argv[5];
    int num_samples_and_iterations = (run_type.compare("debug") == 0) ? 1 : 3;
    std::fstream moments_file, shots_file, timing_file, extra_data_file;

    // load the cleaned moments
    std::cout << "loading cleaned moments file: " << moments_file_path << std::endl;
    moments_file.open(moments_file_path);
    load_cleaned_moment_rows_vec(moments_file, moments);
    moments_file.close();
    std::cout << "moments size: " << moments.size() << std::endl;
    moments_col = new moment_col_store(moments);

    // bin the moments
    bins = new court_bins(moments_col);

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


    res.query1_rowstore_sequential_time = std::numeric_limits<double>::quiet_NaN();

    std::cout << "running query 1 cleaned, sequential" << std::endl;
    double min_time = Halide::Tools::benchmark(num_samples_and_iterations, num_samples_and_iterations, [&]() {
        shots_and_players_list.clear();
        find_nearest_defender_at_each_shot_clean(moments_col, shots_col, &shots_and_players_list, extra_data, 50, false);
    });
    shots_and_players_list.to_vector(shots_and_players_seq);
    printf("compute time: %gms\n", min_time * 1e3);
    std::cout << "shot_and_players size: " << shots_and_players_seq.size() << std::endl;
    vector<shot_distance_bucket> buckets = bucket_shots_by_distance(shots_and_players_seq);
    std::cout << "distance,num_shot_made,num_shot_attempt,percent_made" << std::endl;
    int num_buckets = 0;
    for (const auto & b : buckets) {
        print_shot_distance_bucket_csv(std::cout, b);
        std::cout << std::endl;
        if (num_buckets > 30) {
            break;
        }
        num_buckets++;
    }
    std::cout << "first nearest at shot: " << shots_and_players_seq.at(0) << std::endl;
    res.query1_colstore_sequential_time = min_time;

    std::cout << "running query 1 cleaned, parallel" << std::endl;
    min_time = Halide::Tools::benchmark(num_samples_and_iterations, num_samples_and_iterations, [&]() {
        shots_and_players_list.clear();
        find_nearest_defender_at_each_shot_clean(moments_col, shots_col, &shots_and_players_list, extra_data, 50, true);
    });
    shots_and_players_list.to_vector(shots_and_players_par);
    printf("compute time: %gms\n", min_time * 1e3);
    std::cout << "shot_and_players size: " << shots_and_players_par.size() << std::endl;
    buckets = bucket_shots_by_distance(shots_and_players_par);
    std::cout << "distance,num_shot_made,num_shot_attempt,percent_made" << std::endl;
    num_buckets = 0;
    for (const auto & b : buckets) {
        print_shot_distance_bucket_csv(std::cout, b);
        std::cout << std::endl;
        if (num_buckets > 30) {
            break;
        }
        num_buckets++;
    }
    std::cout << "first nearest at shot: " << shots_and_players_par.at(0) << std::endl;
    res.query1_colstore_parallel_time = min_time;
    coordinate_range origin{{64.5f,24.9f,0}, {65.5f,25.1f, 0}};
    coordinate_range destination{{70.0f,16.0f,0}, {90.0f,32.0f, 0}};
    /*
    std::cout << "running query 3 cleaned with colstore, sequential" << std::endl;
    min_time = Halide::Tools::benchmark(num_samples_and_iterations, num_samples_and_iterations, [&]() {
        trajectories_list.clear();
        find_trajectories_fixed_origin_clean(moments_col, &trajectories_list, origin, destination, 5, 25, false);
    });
    trajectories_list.to_vector(trajectories);
    printf("compute time: %gms\n", min_time * 1e3);
    std::cout << "trajectories size: " << trajectories.size() << std::endl;
     */
    res.query3_colstore_sequential_time = -1; //min_time

    std::cout << "running query 3 cleaned with colstore, parallel" << std::endl;
    min_time = Halide::Tools::benchmark(num_samples_and_iterations, num_samples_and_iterations, [&]() {
        trajectories_list.clear();
        find_trajectories_fixed_origin_clean(moments_col, &trajectories_list, origin, destination, 2, 25, true);
    });
    trajectories_list.to_vector(trajectories);
    printf("compute time: %gms\n", min_time * 1e3);
    std::cout << "trajectories size: " << trajectories.size() << std::endl;
    std::cout << "first trajectory: " << trajectories.at(0) << std::endl;
    res.query3_colstore_parallel_time = min_time;

    /*
    std::cout << "running query 3 cleaned and binned with colstore, sequential" << std::endl;
    min_time = Halide::Tools::benchmark(num_samples_and_iterations, num_samples_and_iterations, [&]() {
        trajectories_list.clear();
        find_trajectories_fixed_origin_clean_binned(moments_col, bins, &trajectories_list, origin, destination, 5, 25, false);
    });
    trajectories_list.to_vector(trajectories);
    printf("compute time: %gms\n", min_time * 1e3);
    std::cout << "trajectories size: " << trajectories.size() << std::endl;
     */
    res.query3_binned_colstore_sequential_time = -1; //min_time;

    std::cout << "running query 3 cleaned and binned with colstore, parallel" << std::endl;

#ifdef CALLGRIND
    CALLGRIND_START_INSTRUMENTATION;
    CALLGRIND_TOGGLE_COLLECT;
    std::cout << "starting collect" << std::endl;
#endif
    min_time = Halide::Tools::benchmark(num_samples_and_iterations, num_samples_and_iterations, [&]() {
        trajectories_list.clear();
        find_trajectories_fixed_origin_clean_binned(moments_col, bins, &trajectories_list, origin, destination, 2, 25, true);
    });
#ifdef CALLGRIND
    CALLGRIND_TOGGLE_COLLECT;
    CALLGRIND_STOP_INSTRUMENTATION;
#endif
    trajectories_list.to_vector(trajectories);
    printf("compute time: %gms\n", min_time * 1e3);
    std::cout << "trajectories size: " << trajectories.size() << std::endl;
    std::cout << "first trajectory: " << trajectories.at(0) << std::endl;
    res.query3_binned_colstore_parallel_time = min_time;
    /*
    std::cout << "running query 3 cleaned with row store, sequential" << std::endl;
    min_time = Halide::Tools::benchmark(num_samples_and_iterations, num_samples_and_iterations, [&]() {
        trajectories.clear();
        find_trajectories_fixed_origin_clean_rowstore(moments, trajectories, origin, destination, 5, 25);
    });
    printf("compute time: %gms\n", min_time * 1e3);
    std::cout << "trajectories size: " << trajectories.size() << std::endl;
     */
    res.query3_rowstore_sequential_time = -1; //min_time;

    // write results
    std::cout << "writing to file: " << timing_file_path << std::endl;
    timing_file.open(timing_file_path, std::fstream::out);
    timing_file << "Language,Query1 Rowstore Sequential Time (ms),Query1 Colstore Sequential Time (ms),Query1 Colstore Parallel Time (ms),"
                << "Query3 Rowstore Sequential Time (ms),Query3 Colstore Sequential Time (ms),"
                << "Query3 Colstore Parallel Time (ms),Query3 Binned Colstore Sequential Time (ms),"
                << "Query3 Binned Colstore Parallel Time" << std::endl;
    timing_file << std::fixed << std::setprecision(2)
                << "CPP," << res.query1_rowstore_sequential_time*1e3 << "," << res.query1_colstore_sequential_time*1e3
                << "," << res.query1_colstore_parallel_time*1e3 << ","
                << res.query3_rowstore_sequential_time*1e3 << "," << res.query3_colstore_sequential_time*1e3 << ","
                << res.query3_colstore_parallel_time*1e3 << "," << res.query3_binned_colstore_sequential_time*1e3 << ","
                << res.query3_binned_colstore_parallel_time*1e3 << std::endl;
    timing_file.close();

    return 0;
}