#include <algorithm>
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <thread>
#include <chrono>
#include <numeric>
#include <sstream>
#include "benchmark.h"
#include "load_data.h"
#include "query_structs.h"
#ifdef CALLGRIND
#include <valgrind/callgrind.h>
#endif
using std::string;
int main(int argc, char * argv[]) {
    vector<moment> moments;
    vector<cleaned_moment> cleaned_moments;
    vector<shot> shots;
    if (argc != 6) {
        std::cout << "please call this code with 5 arguments: " << std::endl;
        std::cout << "1. path/to/moments_folder " << std::endl;
        std::cout << "2. moments_file1.csv,moments_file2.csv,... " << std::endl;
        std::cout << "3. path/to/cleaned_moments_file.csv " << std::endl;
        std::cout << "4. path/to/shots_file.csv " << std::endl;
        std::cout << "5. path/to/cleaned_shots_file.csv " << std::endl;
    }
    string moments_folder_path = argv[1], moments_file_names = argv[2], cleaned_moments_file_path = argv[3],
           shots_file_path = argv[4], cleaned_shots_file_path = argv[5];
    std::fstream moments_file, cleaned_moments_file, shots_file, cleaned_shots_file;
    // load the moments
    std::cout << "moment size: " << sizeof(moment) << std::endl;
    std::stringstream moment_ss(moments_file_names);
    string moments_file_name, cur_moments_path;
    while (moment_ss.good()) {
        getline(moment_ss, moments_file_name, ',');
        cur_moments_path = moments_folder_path + moments_file_name;
        std::cout << "loading moments file: " << cur_moments_path << std::endl;
        moments_file.open(cur_moments_path);
        load_moment_rows(moments_file, moments);
        moments_file.close();
    }
    std::cout << "moments size: " << moments.size() << std::endl;
    std::cout << "sorting moments: " << std::endl;
    std::sort(moments.begin(), moments.end(), [](moment m0, moment m1) {
        return (m0.game_id < m1.game_id || (m0.game_id == m1.game_id && m0.quarter < m1.quarter) ||
                (m0.game_id == m1.game_id && m0.quarter == m1.quarter && m0.game_clock > m1.game_clock) ||
                (m0.game_id == m1.game_id && m0.quarter == m1.quarter && m0.game_clock == m1.game_clock &&
                    m0.player_id < m1.player_id) ||
                (m0.game_id == m1.game_id && m0.quarter == m1.quarter && m0.game_clock == m1.game_clock &&
                    m0.player_id == m1.player_id && m0.event_id < m1.event_id) ||
                (m0.game_id == m1.game_id && m0.quarter == m1.quarter && m0.game_clock == m1.game_clock &&
                    m0.player_id == m1.player_id && m0.event_id == m1.event_id && m0.moment_in_event < m1.moment_in_event));
    });
    clean_moment_rows(moments, cleaned_moments);
    std::cout << "writing output cleaned moments file: " << cleaned_moments_file_path << std::endl;
    cleaned_moments_file.open(cleaned_moments_file_path, std::ios::out);
    cleaned_moments_file << "team_id_ball, player_id_ball, x_loc_ball, y_loc_ball, radius_ball";
    for (int i = 0; i < 10; i++) {
        cleaned_moments_file << ", team_id_" << i
                             << ", player_id_" << i
                             << ", x_loc_" << i
                             << ", y_loc_" << i
                             << ", radius_" << i;
    }
    cleaned_moments_file << ", game_clock, shot_clock, quarter, game_id, events" << std::endl;
    for (const auto & c : cleaned_moments) {
        print_cleaned_moment_csv(cleaned_moments_file, c);
        cleaned_moments_file << std::endl;
    }
    cleaned_moments_file.close();


    std::cout << "loading shots file: " << shots_file_path << std::endl;
    shots_file.open(shots_file_path);
    load_shot_rows_vec(shots_file, shots);
    shots_file.close();
    std::cout << "shots size: " << shots.size() << std::endl;
    std::sort(shots.begin(), shots.end(), [](shot s0, shot s1) {
        return (s0.game_id < s1.game_id ||
                (s0.game_id == s1.game_id && s0.period < s1.period) ||
                (s0.game_id == s1.game_id && s0.period == s1.period && s0.shot_time.to_double() > s1.shot_time.to_double()) ||
                (s0.game_id == s1.game_id && s0.period == s1.period && s0.shot_time == s1.shot_time && s0.game_event_id < s1.game_event_id));
    });

    std::cout << "writing output cleaned shots file: " << cleaned_shots_file_path << std::endl;
    cleaned_shots_file.open(cleaned_shots_file_path, std::ios::out);
    cleaned_shots_file << "action_type, event_time, event_type, game_date, game_event_id, game_id, grid_type, htm,"
                       << " loc_x, loc_y, minutes_remaining, period, player_id, player_name, quarter, seconds_remaining,"
                       << " shot_attempted_flag, shot_distance, shot_made_flag, shot_time, shot_type, shot_zone_area,"
                       << " shot_zone_basic, shot_zone_range, team_id, team_name, team_vtm" << std::endl;
    for (const auto & s : shots) {
        print_shot_csv(cleaned_shots_file, s);
        cleaned_shots_file << std::endl;
    }
    cleaned_shots_file.close();

    return 0;
}