#include <string>
#include <vector>
#include <istream>
#include <sstream>
#include <iostream>
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

/* load a CSV file of moments with a header row */
void load_moment_rows(istream& rows, vector<moment>& ms) {
    string row;
    std::getline(rows, row);
    int row_num = 0;
    while(std::getline(rows, row)) {
        if (row_num >= 880) {
            std::cout << "at row: " << row_num << std::endl;
            std::cout << row << std::endl;
        }
        moment m;
        load_moment_row(row, m);
        ms.push_back(m);
        row_num++;
    }
}

void load_moment_row(string& row, moment& m) {
    string col;
    stringstream ss(row);

    std::getline(ss, col, ',');
    m.team_id = stol_with_default(col);
    std::getline(ss, col, ',');
    m.player_id = stoi_with_default(col);
    std::getline(ss, col, ',');
    m.x_loc = stof_with_default(col);
    std::getline(ss, col, ',');
    m.y_loc = stof_with_default(col);
    std::getline(ss, col, ',');
    m.radius = stof_with_default(col);
    std::getline(ss, col, ',');
    m.game_clock = stof_with_default(col);
    std::getline(ss, col, ',');
    m.shot_clock = stof_with_default(col);
    std::getline(ss, col, ',');
    m.quarter = stoi_with_default(col);
    std::getline(ss, col, ',');
    m.game_id = stol_with_default(col);
    std::getline(ss, col, ',');
    m.event_id = stol_with_default(col);
    std::getline(ss, col, ',');
    m.moment_in_event = stoi_with_default(col);
}

/* load a CSV file of events with a header row */
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
    e.game_id = stol_with_default(col);
    std::getline(ss, col, ',');
    e.event_num = stol_with_default(col);
    std::getline(ss, col, ',');
    e.event_msg_type = stoi_with_default(col);
    std::getline(ss, col, ',');
    e.event_msg_action_type = stoi_with_default(col);
    std::getline(ss, col, ',');
    e.period = stoi_with_default(col);
    std::getline(ss, e.wc_timestring, ',');
    std::getline(ss, e.pc_timestring, ',');
    std::getline(ss, e.home_description, ',');
    std::getline(ss, e.neutral_description, ',');
    std::getline(ss, e.visitor_description, ',');
    std::getline(ss, e.score, ',');
    std::getline(ss, e.score_margin, ',');

    // load player 1 data
    std::getline(ss, col, ',');
    e.person1_type = stoi_with_default(col);
    std::getline(ss, col, ',');
    e.player1_id = stoi_with_default(col);
    std::getline(ss, e.player1_name, ',');
    std::getline(ss, col, ',');
    e.player1_team_id = stof_with_default(col);
    std::getline(ss, e.player1_team_city, ',');
    std::getline(ss, e.player1_team_nickname, ',');
    std::getline(ss, e.player1_team_abbreviation, ',');

    // load player 2 data
    std::getline(ss, col, ',');
    e.person2_type = stoi_with_default(col);
    std::getline(ss, col, ',');
    e.player2_id = stoi_with_default(col);
    std::getline(ss, e.player2_name, ',');
    std::getline(ss, col, ',');
    e.player2_team_id = stof_with_default(col);
    std::getline(ss, e.player2_team_city, ',');
    std::getline(ss, e.player2_team_nickname, ',');
    std::getline(ss, e.player2_team_abbreviation, ',');

    // load player 3 data
    std::getline(ss, col, ',');
    e.person3_type = stoi_with_default(col);
    std::getline(ss, col, ',');
    e.player3_id = stoi_with_default(col);
    std::getline(ss, e.player3_name, ',');
    std::getline(ss, col, ',');
    e.player3_team_id = stof_with_default(col);
    std::getline(ss, e.player3_team_city, ',');
    std::getline(ss, e.player3_team_nickname, ',');
    std::getline(ss, e.player3_team_abbreviation, ',');
}

/* load a CSV file of shots with a header row */
void load_shot_rows(istream& rows, vector<shot>& shs) {
    string row;
    std::getline(rows, row);
    while(std::getline(rows, row)) {
        shot sh;
        load_shot_row(row, sh);
        shs.push_back(sh);
    }
}

void load_shot_row(string& row, shot& sh) {
    string col;
    stringstream ss(row);

    std::getline(ss, sh.action_type, ',');
    std::getline(ss, col, ',');
    sh.event_time = stoi_with_default(col);
    std::getline(ss, sh.event_type, ',');
    std::getline(ss, sh.game_date, ',');
    std::getline(ss, col, ',');
    sh.game_event_id = stol_with_default(col);
    std::getline(ss, col, ',');
    sh.game_id = stol_with_default(col);
    std::getline(ss, sh.grid_type, ',');
    std::getline(ss, sh.htm, ',');
    std::getline(ss, col, ',');
    sh.loc_x = stof_with_default(col);
    std::getline(ss, col, ',');
    sh.loc_y = stof_with_default(col);
    std::getline(ss, col, ',');
    sh.minutes_remaining = stoi_with_default(col);
    std::getline(ss, col, ',');
    sh.period = stoi_with_default(col);
    std::getline(ss, col, ',');
    sh.player_id = stoi_with_default(col);
    std::getline(ss, sh.player_name, ',');
    std::getline(ss, col, ',');
    sh.quarter = stof_with_default(col);
    std::getline(ss, col, ',');
    sh.seconds_remaining = stoi_with_default(col);
    std::getline(ss, col, ',');
    sh.shot_attempted_flag = stoi_with_default(col);
    std::getline(ss, col, ',');
    sh.shot_distance = stoi_with_default(col);
    std::getline(ss, col, ',');
    sh.shot_made_flag = stoi_with_default(col);
    std::getline(ss, col, ',');
    sh.shot_time = stof_with_default(col);
    std::getline(ss, sh.shot_type, ',');
    std::getline(ss, sh.shot_zone_area, ',');
    std::getline(ss, sh.shot_zone_basic, ',');
    std::getline(ss, sh.shot_zone_range, ',');
    std::getline(ss, col, ',');
    sh.team_id = stof_with_default(col);
    std::getline(ss, sh.team_name, ',');
    std::getline(ss, sh.team_vtm, ',');
}
