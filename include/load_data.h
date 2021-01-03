#ifndef LOAD_DATA_H
#define LOAD_DATA_H
#include <memory>
#include <vector>
#include <istream>
#include "query_structs.h"
using std::istream;
using std::vector;

void load_moment_rows(istream& rows, vector<moment>& ms);
void load_moment_row(string& row, moment& m);
void load_cleaned_moment_rows(istream& rows, vector<cleaned_moment>& ms);
void load_cleaned_moment_row(string& row, cleaned_moment& m);
void clean_moment_rows(vector<moment>& src, vector<cleaned_moment>& dst);
void load_event_rows(istream& row, vector<event>& m);
void load_event_row(string& row, event& m);
void load_shot_rows(istream& row, vector<shot>& m);
void load_shot_row(string& row, shot& m);
#endif 
