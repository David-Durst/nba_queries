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
    vector<cleaned_shot> cleaned_shots;
    std::map<long int, int> game_id_to_num;
    vector<extra_game_data> extra_data;
    if (argc < 6) {
        std::cout << "please call this code at least 5 arguments: " << std::endl;
        std::cout << "1. path/to/cleaned_moments_file.csv " << std::endl;
        std::cout << "2. path/to/shots_file.csv " << std::endl;
        std::cout << "3. path/to/cleaned_shots_file.csv " << std::endl;
        std::cout << "4. path/to/extra_data_file.csv " << std::endl;
        std::cout << "5. path/to/moments_file1.csv optional/path/to/moments_file2.csv,... " << std::endl;
    }
    string cleaned_moments_file_path = argv[1], shots_file_path = argv[2], cleaned_shots_file_path = argv[3],
        extra_data_file_path = argv[4], cur_moments_file;
    std::fstream moments_file, cleaned_moments_file, shots_file, cleaned_shots_file, extra_data_file;
    // load the moments
    std::cout << "moment size: " << sizeof(moment) << std::endl;
    for (int i = 5; i < argc; i++) {
        cur_moments_file = argv[i];
        std::cout << "loading moments file: " << cur_moments_file << std::endl;
        moments_file.open(cur_moments_file);
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
    clean_moment_rows(moments, cleaned_moments, game_id_to_num, extra_data);
    std::cout << "cleaned moments size: " << cleaned_moments.size() << std::endl;
    for (int64_t i = 0; i < cleaned_moments.size(); i++) {
        clock_fixed_point& c = cleaned_moments.at(i).game_clock;
        int64_t predicted_index = c.time_to_index(extra_data, cleaned_moments.at(i).game_num, cleaned_moments.at(i).quarter);
        if (i != predicted_index) {
            std::cout << "predicted index " << predicted_index << " doesn't match actual index " << i << std::endl;
            return 1;
        }
    }
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
    cleaned_moments_file << ", game_clock, shot_clock, quarter, game_id, game_num, events" << std::endl;
    for (const auto & c : cleaned_moments) {
        print_cleaned_moment_csv(cleaned_moments_file, c);
        cleaned_moments_file << std::endl;
    }
    cleaned_moments_file.close();

    extra_data_file.open(extra_data_file_path, std::ios::out);
    extra_data_file << "game_id, game_num, num_ot_periods" << std::endl;
    for (const auto & e : extra_data) {
        print_extra_game_data_csv(extra_data_file, e);
        extra_data_file << std::endl;
    }
    extra_data_file.close();

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
    clean_shot_rows(shots, cleaned_shots, game_id_to_num);

    std::cout << "writing output cleaned shots file: " << cleaned_shots_file_path << std::endl;
    cleaned_shots_file.open(cleaned_shots_file_path, std::ios::out);
    cleaned_shots_file << "action_type, event_time, event_type, game_date, game_event_id, game_id, game_num, grid_type, htm,"
                       << " loc_x, loc_y, minutes_remaining, period, player_id, player_name, quarter, seconds_remaining,"
                       << " shot_attempted_flag, shot_distance, shot_made_flag, shot_time, shot_type, shot_zone_area,"
                       << " shot_zone_basic, shot_zone_range, team_id, team_name, team_vtm" << std::endl;
    for (const auto & s : cleaned_shots) {
        print_cleaned_shot_csv(cleaned_shots_file, s);
        cleaned_shots_file << std::endl;
    }
    cleaned_shots_file.close();

    return 0;
}