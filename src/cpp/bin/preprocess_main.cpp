#include <algorithm>
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <thread>
#include <chrono>
#include <numeric>
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
    if (argc != 3) {
        std::cout << "please call this code with 2 arguments: " << std::endl;
        std::cout << "1. path/to/moments_file.csv " << std::endl;
        std::cout << "2. path/to/cleaned_moments_file.csv " << std::endl;
    }
    string moments_file_path = argv[1], cleaned_moments_file_path = argv[2];
    std::fstream moments_file, cleaned_moments_file;
    // load the moments
    std::cout << "moment size: " << sizeof(moment) << std::endl;
    std::cout << "loading moments file: " << moments_file_path << std::endl;
    moments_file.open(moments_file_path);
    load_moment_rows(moments_file, moments);
    moments_file.close();
    std::cout << "moments size: " << moments.size() << std::endl;
    std::cout << "sorting moments: " << std::endl;
    std::sort(moments.begin(), moments.end(), [](moment m0, moment m1) {
        return (m0.quarter < m1.quarter || (m0.quarter == m1.quarter && m0.game_clock > m1.game_clock) ||
                (m0.quarter == m1.quarter && m0.game_clock == m1.game_clock && m0.player_id < m1.player_id) ||
                (m0.quarter == m1.quarter && m0.game_clock == m1.game_clock && m0.player_id == m1.player_id && m0.event_id < m1.event_id) ||
                (m0.quarter == m1.quarter && m0.game_clock == m1.game_clock && m0.player_id == m1.player_id &&
                 m0.event_id == m1.event_id && m0.moment_in_event < m1.moment_in_event));
    });
    clean_moment_rows(moments, cleaned_moments);
    for (size_t i = 1; i < cleaned_moments.size(); i++) {
        if (cleaned_moments.at(i-1).game_clock.abs_diff(cleaned_moments.at(i).game_clock) != clock_fixed_point(0.04f)) {
            std::cout << "big jump in cleaned moments " << i-1 << " with quarter " << cleaned_moments.at(i-1).quarter
                      << " and gameclock " << cleaned_moments.at(i-1).game_clock.to_float()
                      << "and " << i << " with quarter " << cleaned_moments.at(i).quarter
                      << " and gameclock " << cleaned_moments.at(i).game_clock.to_float() << std::endl << std::endl;
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
    cleaned_moments_file << ", game_clock, shot_clock, quarter, game_id, events" << std::endl;
    for (const auto & c : cleaned_moments) {
        print_cleaned_moment_csv(cleaned_moments_file, c);
        cleaned_moments_file << std::endl;
    }
    cleaned_moments_file.close();
    return 0;
}