#include <iostream>
#include <fstream>
#include "load_data.h"
#include "query_structs.h"
#include "moment.pb.h"

int main(int argc, char * argv[]) {
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    vector<cleaned_moment> moments;
    if (argc != 3) {
        std::cout << "please call this code with 2 arguments: " << std::endl;
        std::cout << "1. path/to/cleaned_moments_file.csv " << std::endl;
        std::cout << "5. path/to/output/cleaned/moments.pb" << std::endl;
        return 0;
    }
    string moments_file_path = argv[1], moments_proto_path = argv[2];
    std::fstream moments_file, proto_file;

    // load the cleaned moments
    std::cout << "loading cleaned moments file: " << moments_file_path << std::endl;
    moments_file.open(moments_file_path);
    load_cleaned_moment_rows_vec(moments_file, moments);
    moments_file.close();
    std::cout << "moments size: " << moments.size() << std::endl;
    return 0;
}