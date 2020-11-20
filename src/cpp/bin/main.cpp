#include <iostream>
#include <vector>
#include <fstream>
#include "main.h"
#include "load_data.h"
#include "query_structs.h"

int main(int argc, char * argv[]) {
    vector<moment> moments;
    vector<shot> shots;

    std::fstream moments_file, shots_file;
    // load the moments
    moments_file.open(argv[1]);
    load_moment_rows(moments_file, moments);
    moments_file.close();
    std::cout << "moments size: " << shots.size() << std::endl;
    // load the shots 
    shots_file.open(argv[1]);
    load_shot_rows(shots_file, shots);
    shots_file.close();
    std::cout << "shots size: " << shots.size() << std::endl;

    return 0;
}
