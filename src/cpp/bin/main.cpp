#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <thread>
#include <chrono>
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
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(10));
    }

    return 0;
}
