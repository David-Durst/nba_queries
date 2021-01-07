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
#include "benchmark.h"
#include "load_data.h"
#include "query_structs.h"
#ifdef CALLGRIND
#include <valgrind/callgrind.h>
#endif
using std::string;

int main(int argc, char * argv[]) {
    vector<cleaned_moment> moments;
    vector<shot> shots;
    vector<shot_and_player_data> shots_and_players;
    vector<trajectory_data> trajectories;
    if (argc != 3) {
        std::cout << "please call this code with 2 arguments: " << std::endl;
        std::cout << "1. path/to/cleaned_moments_file.csv " << std::endl;
        std::cout << "2. path/to/shots_file.csv " << std::endl;
    }
    string moments_file_path = argv[1], shots_file_path = argv[2];
    std::fstream moments_file, shots_file;

    // load the cleaned moments
    std::cout << "loading cleaned moments file: " << moments_file_path << std::endl;
    moments_file.open(moments_file_path);
    load_cleaned_moment_rows(moments_file, moments);
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
    return 0;
}