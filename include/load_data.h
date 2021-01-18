#ifndef LOAD_DATA_H
#define LOAD_DATA_H
#include <memory>
#include <vector>
#include <istream>
#include <map>
#include "query_structs.h"
using std::istream;
using std::vector;

void load_moment_rows(istream& rows, vector<moment>& ms);
void load_moment_row(string& row, moment& m);
size_t load_cleaned_moment_rows(string filepath, cleaned_moment * ms[]);
void load_cleaned_moment_rows_vec(istream& rows, vector<cleaned_moment>& ms);
void load_cleaned_moment_row(string& row, cleaned_moment& m);
void clean_moment_rows(vector<moment>& src, vector<cleaned_moment>& dst, std::map<long int, int>& game_id_to_num);
void load_event_rows(istream& row, vector<event>& m);
void load_event_row(string& row, event& m);
size_t load_shot_rows(string filepath, shot * s_arr[]);
void load_shot_rows_vec(istream& row, vector<shot>& m);
void load_shot_row(string& row, shot& m);
void load_cleaned_shot_rows_vec(istream& rows, vector<cleaned_shot>& ms);
void load_cleaned_shot_row(string& row, cleaned_shot& m);
void clean_shot_rows(vector<shot>& src, vector<cleaned_shot>& dst, std::map<long int, int>& game_id_to_num);
#endif
