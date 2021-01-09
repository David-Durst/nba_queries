#include <algorithm>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <thread>
#include <chrono>
#include <numeric>
#include "check_distances.h"
#include "find_trajectories.h"
#include "clean_find_trajectories.h"
#include "benchmark.h"
#include "load_data.h"
#include "query_structs.h"
#ifdef CALLGRIND
#include <valgrind/callgrind.h>
#endif
using std::string;

int main(int argc, char * argv[]) {
    cleaned_moment * moments;
    shot * shots;
    vector<shot> shots_vec;
    list<shot_and_player_data> shots_and_players;
    vector<shot_and_player_data> shots_and_players_vec;
    list<trajectory_data> trajectories;
    vector<trajectory_data> trajectories_vec;
    if (argc != 3) {
        std::cout << "please call this code with 2 arguments: " << std::endl;
        std::cout << "1. path/to/cleaned_moments_file.csv " << std::endl;
        std::cout << "2. path/to/shots_file.csv " << std::endl;
    }
    string moments_filepath = argv[1], shots_filepath = argv[2];
    std::fstream shots_file;

    // load the cleaned moments
    std::cout << "loading cleaned moments file: " << moments_filepath << std::endl;
    size_t moments_size = load_cleaned_moment_rows(moments_filepath, &moments);
    std::cout << "moments size: " << moments_size << std::endl;
    // load the shots
    std::cout << "loading shots file: " << shots_filepath << std::endl;
    shots_file.open(shots_filepath);
    load_shot_rows_vec(shots_file, shots_vec);
    shots_file.close();
    std::cout << "shots size: " << shots_vec.size() << std::endl;
    // filter shots, sort by quarter than gametime
    long int target_game_id = moments[0].game_id;
    std::cout << "filtering shots: " << std::endl;
    shots_vec.erase(std::remove_if(shots_vec.begin(), shots_vec.end(), [target_game_id](shot s){
        return s.game_id != target_game_id;
    }), shots_vec.end());
    std::cout << "new shots size: " << shots_vec.size() << ", sorting shots" << std::endl;
    std::sort(shots_vec.begin(), shots_vec.end(), [](shot s0, shot s1) {
        return (s0.period < s1.period || (s0.period == s1.period && s0.shot_time > s1.shot_time) ||
                (s0.period == s1.period && s0.shot_time == s1.shot_time && s0.game_event_id < s1.game_event_id));
    });
    shots = new shot[shots_vec.size()];
    std::copy(shots_vec.begin(), shots_vec.end(), shots);

    std::cout << "running query 3 cleaned" << std::endl;
    coordinate_range origin{{70.0f,16.0f,0}, {90.0f,32.0f, 0}};
    coordinate_range destination{{71.9f,24.9f,0}, {72.1f,25.1f, 0}};
    double min_time = Halide::Tools::benchmark(3, 3, [&]() {
        trajectories.clear();
        find_trajectories_fixed_origin_clean(moments, trajectories, origin, destination, 5, 25);
    });
    printf("compute time: %gms\n", min_time * 1e3);
    std::cout << "trajectories size: " << trajectories.size() << std::endl;

    return 0;
}