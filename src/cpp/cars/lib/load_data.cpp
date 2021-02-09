#include "csv.hpp"
#include "car_query_structs.h"
#include "car_moment.capnp.h"
#include "car_load_data.h"
#include <vector>
#include <string>
using std::vector;
using std::string;

void load_data_csv(vector<string> car_file_paths, car_col_store& car_moments, int skip_first_n_rows) {
    csv::CSVFormat format;
    std::vector<std::string> col_names = {"t", "x", "y", "z"};
    format.column_names(col_names);

    int64_t file_index = 0;
    for (const auto & car_file_path : car_file_paths) {
        csv::CSVReader reader(car_file_path, format);
        int64_t cur_file_index = 0;
        for (const auto & row : reader) {
            if (cur_file_index < skip_first_n_rows) {
                cur_file_index++;
                continue;
            }
            car_moments.t[file_index][cur_file_index] = row["t"].get<int64_t>();
            car_moments.x[file_index][cur_file_index] = row["x"].get<double>();
            car_moments.x[file_index][cur_file_index] = row["y"].get<double>();
            car_moments.x[file_index][cur_file_index] = row["z"].get<double>();
        }
        file_index++;
    }
}
