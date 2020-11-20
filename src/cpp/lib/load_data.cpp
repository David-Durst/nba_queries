#include <string>
#include <vector>
#include <istream>
#include <sstream>
#include "query_structs.h"
#include "load_data.h"
using std::vector;
using std::string;
using std::istream;
using std::stringstream;

void load_moment_rows(istream& rows, vector<moment>& ms) {
    string row;
    while(std::getline(rows, row)) {
        moment m;
        load_moment_row(row, m);
        ms.push_back(m);
    }
}

void load_moment_row(string& row, moment& m) {
    string col;
    stringstream ss(row);

    std::getline(ss, col, ',');
    m.team_id = stol(col);
    std::getline(ss, col, ',');
    m.player_id = stoi(col);
    std::getline(ss, col, ',');
    m.x_loc = stof(col);
    std::getline(ss, col, ',');
    m.y_loc = stof(col);
    std::getline(ss, col, ',');
    m.radius = stof(col);
    std::getline(ss, col, ',');
    m.game_clock = stof(col);
    std::getline(ss, col, ',');
    m.shot_clock = stof(col);
    std::getline(ss, col, ',');
    m.quarter = stoi(col);
    std::getline(ss, col, ',');
    m.game_id = stol(col);
    std::getline(ss, col, ',');
    m.event_id = stol(col);
    std::getline(ss, col, ',');
    m.moment_in_event = stoi(col);
}
