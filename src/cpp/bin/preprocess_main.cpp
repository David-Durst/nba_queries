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
    if (argc != 3) {
        std::cout << "please call this code with 2 arguments: " << std::endl;
        std::cout << "1. path/to/moments_file.csv " << std::endl;
        std::cout << "2. path/to/cleaned_moments_file.csv " << std::endl;
    }
    string moments_file_path = argv[1], cleaned_moments_file_path = argv[1];
    std::fstream moments_file;
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
                (m0.quarter == m1.quarter && m0.game_clock == m1.game_clock && m0.event_id < m1.event_id) ||
                (m0.quarter == m1.quarter && m0.game_clock == m1.game_clock && m0.event_id == m1.event_id && m0.player_id < m1.player_id) ||
                (m0.quarter == m1.quarter && m0.game_clock == m1.game_clock && m0.event_id == m1.event_id &&
                 m0.player_id == m1.player_id && m0.moment_in_event < m1.moment_in_event));
    });
    vector<cleaned_moment> cleaned_moments;
    long int last_cleaned_moment_index = 0;
    for (const auto & m : moments) {

    }
}