#ifndef NBA_QUERIES_CAR_LOAD_DATA_H
#include "car_query_structs.h"
#include <string>
#include <vector>
using std::vector;
using std::string;

void load_data_csv(vector<string> car_file_paths, car_col_store& car_moments, int skip_first_n_rows);
void load_data_pb(vector<string> car_file_paths, car_col_store& car_moments);
void save_data_pb(vector<string> car_file_paths, car_col_store& car_moments);

#define NBA_QUERIES_CAR_LOAD_DATA_H

#endif //NBA_QUERIES_CAR_LOAD_DATA_H
