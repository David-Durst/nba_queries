#include <algorithm>
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <thread>
#include <chrono>
#include "check_distances.h"
#include "find_trajectories.h"
#include "main.h"
#include "load_data.h"
#include "query_structs.h"
#ifdef CALLGRIND
#include <valgrind/callgrind.h>
#endif
using std::string;

int main(int argc, char * argv[]) {
    vector<moment> moments;
    vector<shot> shots;
    vector<shot_and_player_data> shots_and_players;
    shots_and_players.reserve(200);
    vector<trajectory_data> trajectories;
    if (argc != 4) {
        std::cout << "please call this code with 3 arguments: " << std::endl;
        std::cout << "1. path/to/moments_file.csv " << std::endl;
        std::cout << "2. path/to/shots_file.csv " << std::endl;
        std::cout << "3. query to run: \"1\" for nearest defender within 2 seconds of each shot, ";
        std::cout << "\"2a\" for trajectories from (x,y,t) to (x+10,y+10,t+5)" << std::endl;
    }
    string moments_file_path = argv[1], shots_file_path = argv[2], query = argv[3];
    std::fstream moments_file, shots_file;

    // load the moments
    std::cout << "moment size: " << sizeof(moment) << std::endl;
    std::cout << "loading moments file: " << moments_file_path << std::endl;
    moments_file.open(moments_file_path);
    load_moment_rows(moments_file, moments);
    moments_file.close();
    std::cout << "moments size: " << moments.size() << std::endl;
    // load the shots 
    std::cout << "loading shots file: " << shots_file_path << std::endl;
    shots_file.open(shots_file_path);
    load_shot_rows(shots_file, shots);
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
    for (int i = 0; i < 5; i++) {
        std::cout << "shot " << i << ": " << shots.at(i) << std::endl;
    }
    std::cout << "sorting moments: " << std::endl;
    std::sort(moments.begin(), moments.end(), [](moment m0, moment m1) {
        return (m0.quarter < m1.quarter || (m0.quarter == m1.quarter && m0.game_clock > m1.game_clock) ||
                (m0.quarter == m1.quarter && m0.game_clock == m1.game_clock && m0.event_id < m1.event_id) ||
                (m0.quarter == m1.quarter && m0.game_clock == m1.game_clock && m0.event_id == m1.event_id && m0.moment_in_event < m1.moment_in_event) ||
                (m0.quarter == m1.quarter && m0.game_clock == m1.game_clock && m0.event_id == m1.event_id &&
                    m0.moment_in_event == m1.moment_in_event && m0.player_id < m1.player_id));
    });


    auto start_compute = std::chrono::high_resolution_clock::now();
#ifdef CALLGRIND
    CALLGRIND_START_INSTRUMENTATION;
    CALLGRIND_TOGGLE_COLLECT;
#endif
    if (query.compare("1") == 0) {
        find_nearest_defender_at_each_shot(moments, shots, shots_and_players);
    }
    else if (query.compare("2a") == 0) {
        find_trajectories_no_fixed_origin(moments, trajectories);
    }
#ifdef CALLGRIND
    CALLGRIND_TOGGLE_COLLECT;
    CALLGRIND_STOP_INSTRUMENTATION;
#endif
    auto end_compute = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end_compute - start_compute;
    std::cout << "shot_and_players size: " << shots_and_players.size() << std::endl;
    vector<shot_distance_bucket> buckets = bucket_shots_by_distance(shots_and_players);
    std::cout << "compute time: " << duration.count() << "s" << std::endl;
    std::cout << "distance,num_shot_made,num_shot_attempt,percent_made" << std::endl;
    for (const auto & b : buckets) {
        print_shot_distance_bucket_csv(std::cout, b);
        std::cout << std::endl;
    }
    return 0;
}
