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

int stoi_with_default(string& s) {
    if (s.size() == 0) {
        return -1.0;
    }
    else {
        return stoi(s);
    }
}

long int stol_with_default(string& s) {
    if (s.size() == 0) {
        return -1l;
    }
    else {
        return stol(s);
    }
}

float stof_with_default(string& s) {
    if (s.size() == 0) {
        return -1.0f;
    }
    else {
        return stof(s);
    }
}

void load_moment_rows(istream& rows, vector<moment>& ms) {
    string row;
    std::getline(rows, row);
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


void load_event_rows(istream& rows, vector<event>& es) {
    string row;
    std::getline(rows, row);
    while(std::getline(rows, row)) {
        event e;
        load_event_row(row, e);
        es.push_back(e);
    }
}

void load_event_row(string& row, event& e) {
    string col;
    stringstream ss(row);

    std::getline(ss, col, ',');
    e.game_id = stol(col);
    std::getline(ss, col, ',');
    e.event_num = stol(col);
    std::getline(ss, col, ',');
    e.event_msg_type = stoi(col);
    std::getline(ss, col, ',');
    e.event_msg_action_type = stoi(col);
    std::getline(ss, col, ',');
    e.period = stoi(col);
    std::getline(ss, e.wc_timestring, ',');
    std::getline(ss, e.pc_timestring, ',');
    std::getline(ss, e.home_description, ',');
    std::getline(ss, e.neutral_description, ',');
    std::getline(ss, e.visitor_description, ',');
    std::getline(ss, e.score, ',');
    std::getline(ss, e.score_margin, ',');

    // load player 1 data
    std::getline(ss, col, ',');
    e.person1_type = stoi(col);
    std::getline(ss, col, ',');
    e.player1_id = stoi(col);
    std::getline(ss, e.player1_name, ',');
    std::getline(ss, col, ',');
    e.player1_team_id = stof_with_default(col);
    std::getline(ss, e.player1_team_city, ',');
    std::getline(ss, e.player1_team_nickname, ',');
    std::getline(ss, e.player1_team_abbreviation, ',');

    // load player 2 data
    std::getline(ss, col, ',');
    e.person2_type = stoi(col);
    std::getline(ss, col, ',');
    e.player2_id = stoi(col);
    std::getline(ss, e.player2_name, ',');
    std::getline(ss, col, ',');
    e.player2_team_id = stof_with_default(col);
    std::getline(ss, e.player2_team_city, ',');
    std::getline(ss, e.player2_team_nickname, ',');
    std::getline(ss, e.player2_team_abbreviation, ',');

    // load player 3 data
    std::getline(ss, col, ',');
    e.person3_type = stoi(col);
    std::getline(ss, col, ',');
    e.player3_id = stoi(col);
    std::getline(ss, e.player3_name, ',');
    std::getline(ss, col, ',');
    e.player3_team_id = stof_with_default(col);
    std::getline(ss, e.player3_team_city, ',');
    std::getline(ss, e.player3_team_nickname, ',');
    std::getline(ss, e.player3_team_abbreviation, ',');
}
