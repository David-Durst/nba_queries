#include <algorithm>
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <thread>
#include <chrono>
#include <numeric>
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

int main(int argc, char * argv[]) {
    vector<cleaned_moment> moments;
    moment_col_store * moments_col;
    vector<shot> shots;
    shot_col_store * shots_col;
    vector<shot_and_player_data> shots_and_players;
    list<shot_and_player_data> shots_and_players_list;
    vector<trajectory_data> trajectories;
    list<trajectory_data> trajectories_list;
    if (argc != 4) {
        std::cout << "please call this code with 3 arguments: " << std::endl;
        std::cout << "1. path/to/cleaned_moments_file.csv " << std::endl;
        std::cout << "2. path/to/shots_file.csv " << std::endl;
        std::cout << "3. debug/measure " << std::endl;
    }
    string moments_file_path = argv[1], shots_file_path = argv[2], run_type = argv[3];
    int num_samples_and_iterations = (run_type.compare("debug") == 0) ? 1 : 10;
    std::fstream moments_file, shots_file;

    // load the cleaned moments
    std::cout << "loading cleaned moments file: " << moments_file_path << std::endl;
    moments_file.open(moments_file_path);
    load_cleaned_moment_rows_vec(moments_file, moments);
    moments_file.close();
    std::cout << "moments size: " << moments.size() << std::endl;
    moments_col = new moment_col_store(moments);
    // load the shots
    std::cout << "loading shots file: " << shots_file_path << std::endl;
    shots_file.open(shots_file_path);
    load_shot_rows_vec(shots_file, shots);
    shots_file.close();
    std::cout << "shots size: " << shots.size() << std::endl;
    // filter shots, sort by quarter than gametime
    long int target_game_id = moments.at(0).game_id;
    std::cout << "filtering shots: " << std::endl;
    shots.erase(std::remove_if(shots.begin(), shots.end(), [target_game_id](shot s){
        return s.game_id != target_game_id;
    }), shots.end());
    std::cout << "new shots size: " << shots.size() << ", sorting shots" << std::endl;
    std::sort(shots.begin(), shots.end(), [](shot s0, shot s1) {
        return (s0.period < s1.period || (s0.period == s1.period && s0.shot_time > s1.shot_time) ||
                (s0.period == s1.period && s0.shot_time == s1.shot_time && s0.game_event_id < s1.game_event_id));
    });
    shots_col = new shot_col_store(shots);
    std::cout << "running query 1 cleaned" << std::endl;
    double min_time = Halide::Tools::benchmark(num_samples_and_iterations, num_samples_and_iterations, [&]() {
        shots_and_players_list.clear();
        find_nearest_defender_at_each_shot_clean(moments_col, shots_col, shots_and_players_list, 50);
    });

    std::cout << "running query 3 cleaned" << std::endl;
    coordinate_range origin{{70.0f,16.0f,0}, {90.0f,32.0f, 0}};
    coordinate_range destination{{71.9f,24.9f,0}, {72.1f,25.1f, 0}};
    double min_time = Halide::Tools::benchmark(num_samples_and_iterations, num_samples_and_iterations, [&]() {
        trajectories_list.clear();
        find_trajectories_fixed_origin_clean(moments_col, &trajectories_list, origin, destination, 5, 25);
    });
    trajectories_list.to_vector(trajectories);
    printf("compute time: %gms\n", min_time * 1e3);
    std::cout << "trajectories size: " << trajectories.size() << std::endl;
    trajectories.clear();

    std::cout << "running query 3 cleaned with row store" << std::endl;
    min_time = Halide::Tools::benchmark(num_samples_and_iterations, num_samples_and_iterations, [&]() {
        trajectories.clear();
        find_trajectories_fixed_origin_clean_rowstore(moments, trajectories, origin, destination, 5, 25);
    });
    printf("compute time: %gms\n", min_time * 1e3);
    std::cout << "trajectories size: " << trajectories.size() << std::endl;


    return 0;
}