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

#pragma omp parallel for
    for (int64_t file_index = 0; file_index < car_file_paths.size(); file_index++) {
        csv::CSVReader reader(car_file_paths[file_index], format);
        int64_t row_index = 0;
        for (const auto & row : reader) {
            if (row_index < skip_first_n_rows) {
                row_index++;
                continue;
            }
            if (row_index >= car_moments.num_ticks) {
                break;
            }
            car_moments.t[file_index][row_index] = row["t"].get<int64_t>();
            car_moments.x[file_index][row_index] = row["x"].get<double>();
            car_moments.x[file_index][row_index] = row["y"].get<double>();
            car_moments.x[file_index][row_index] = row["z"].get<double>();
            row_index++;
        }
    }
}
