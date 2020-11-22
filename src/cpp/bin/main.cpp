#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <thread>
#include <chrono>
#include "check_distances.h"
#include "main.h"
#include "load_data.h"
#include "query_structs.h"
using std::string;

int main(int argc, char * argv[]) {
    vector<moment> moments;
    vector<shot> shots;
    string moments_file_path = argv[1], shots_file_path = argv[2];
    std::fstream moments_file, shots_file;

    // load the moments
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
    find_nearest_defender_at_each_shot(moments, shots, shots_and_players)
    int cur_moment_idx = 0;
    int cur_shot_idx = 0;
    while (cur_moment_idx < (int) moments.size() && cur_shot_idx < (int) shots.size()) {
        shot cur_shot = shots.at(cur_shot_idx);
        if (cur_shot.game_clock == )
        cut_shot_idx++;
    }
    

    return 0;
}
