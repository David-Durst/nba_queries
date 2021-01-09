#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
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

double stod_with_default(string& s) {
    if (s.size() == 0) {
        return -1.0;
    }
    else {
        return stod(s);
    }
}

/* load a CSV file of moments with a header row */
void load_moment_rows(istream& rows, vector<moment>& ms) {
    string row;
    std::getline(rows, row);
    int row_num = 0;
    while(std::getline(rows, row)) {
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
    m.x_loc = stod_with_default(col);
    std::getline(ss, col, ',');
    m.y_loc = stod_with_default(col);
    std::getline(ss, col, ',');
    m.radius = stod_with_default(col);
    std::getline(ss, col, ',');
    m.game_clock = stod_with_default(col);
    std::getline(ss, col, ',');
    m.shot_clock = stod_with_default(col);
    std::getline(ss, col, ',');
    m.quarter = stoi_with_default(col);
    std::getline(ss, col, ',');
    m.game_id = stol_with_default(col);
    std::getline(ss, col, ',');
    m.event_id = stol_with_default(col);
    std::getline(ss, col, ',');
    m.moment_in_event = stoi_with_default(col);
}

/* load a CSV file of cleaned_moments with a header row */
size_t load_cleaned_moment_rows(string filepath, cleaned_moment * ms[]) {
    size_t num_rows = 0;
    std::fstream count_f, load_f;
    count_f.open(filepath);
    string row;
    while(std::getline(count_f, row)) {
        if (row.length() > 0) {
            num_rows++;
        }
    }
    // don't count header rows
    num_rows--;
    count_f.close();
    load_f.open(filepath);
    *ms = new cleaned_moment[num_rows];
    // load first row to skip header
    std::getline(load_f, row);
    for (size_t i = 0; i < num_rows; i++) {
        std::getline(load_f, row);
        load_cleaned_moment_row(row, (*ms)[i]);
    }
    return num_rows;
}

/* load a CSV file of cleaned_moments with a header row */
void load_cleaned_moment_rows_vec(istream& rows, vector<cleaned_moment>& ms) {
    string row;
    std::getline(rows, row);
    int row_num = 0;
    while(std::getline(rows, row)) {
        cleaned_moment m;
        load_cleaned_moment_row(row, m);
        ms.push_back(m);
        row_num++;
    }
}

void load_cleaned_moment_row(string& row, cleaned_moment& m) {
    string col;
    stringstream ss(row);

    // load ball
    std::getline(ss, col, ',');
    m.ball.team_id = stol_with_default(col);
    std::getline(ss, col, ',');
    m.ball.player_id = stoi_with_default(col);
    std::getline(ss, col, ',');
    m.ball.x_loc = stod_with_default(col);
    std::getline(ss, col, ',');
    m.ball.y_loc = stod_with_default(col);
    std::getline(ss, col, ',');
    m.ball.radius = stod_with_default(col);

    // load players
    for (int i = 0; i < 10; i++) {
        std::getline(ss, col, ',');
        m.players[i].team_id = stol_with_default(col);
        std::getline(ss, col, ',');
        m.players[i].player_id = stoi_with_default(col);
        std::getline(ss, col, ',');
        m.players[i].x_loc = stod_with_default(col);
        std::getline(ss, col, ',');
        m.players[i].y_loc = stod_with_default(col);
        std::getline(ss, col, ',');
        m.players[i].radius = stod_with_default(col);
    }

    std::getline(ss, col, ',');
    m.game_clock = clock_fixed_point(stod_with_default(col));
    std::getline(ss, col, ',');
    m.shot_clock = stod_with_default(col);
    std::getline(ss, col, ',');
    m.quarter = stoi_with_default(col);
    std::getline(ss, col, ',');
    m.game_id = stol_with_default(col);

    // load event data
    std::getline(ss, col, ',');
    stringstream es(col);
    string event_subcol;
    while (std::getline(es, event_subcol, ';')) {
        string without_parens = event_subcol.substr(1, event_subcol.size()-2);
        size_t colon_loc = without_parens.find(':');
        string event_id_str = without_parens.substr(0, colon_loc);
        string moment_in_event_str = without_parens.substr(colon_loc+1,without_parens.size()-colon_loc-1);
        event_moment_data emd {stol_with_default(event_id_str),
                               stoi_with_default(moment_in_event_str)};
        m.events.push_back(emd);
    }
}

/* load a CSV file of cleaned_moments with a header row */
void clean_moment_rows(vector<moment>& src, vector<cleaned_moment>& dst) {
    int player_in_moment = 0;
    long int last_player_id = -2;
    for (const auto & m : src) {
        if (dst.empty() ||
            dst.at(dst.size() - 1).game_clock != clock_fixed_point(m.game_clock)) {
            // fix case where skip 0.08 by reinserting last value
            int num_inserted = 0;
            size_t cur_size = dst.size();
            while (!dst.empty() && dst.at(dst.size() - 1).quarter == m.quarter &&
                dst.at(dst.size() - 1).game_clock.abs_diff(clock_fixed_point(m.game_clock)).gt(0.04)) {
                num_inserted++;
                cleaned_moment copied_moment = dst.at(dst.size() - 1);
                copied_moment.game_clock = clock_fixed_point(copied_moment.game_clock.to_double() - 0.04);
                dst.push_back(copied_moment);
            }

            player_in_moment = 0;
            last_player_id = m.player_id;
            // since sorting player_ids, first one in the moment is the ball
            dst.push_back(cleaned_moment());
            cleaned_moment& cur_moment = dst.at(dst.size() - 1);
            cur_moment.game_clock = clock_fixed_point(m.game_clock);
            cur_moment.shot_clock = m.shot_clock;
            cur_moment.quarter = m.quarter;
            cur_moment.game_id = m.game_id;
            cur_moment.events = {event_moment_data{m.event_id, m.moment_in_event}};
            cur_moment.ball.team_id = m.team_id;
            cur_moment.ball.player_id = m.player_id;
            cur_moment.ball.x_loc = m.x_loc;
            cur_moment.ball.y_loc = m.y_loc;
            cur_moment.ball.radius = m.radius;
        }
        else {
            cleaned_moment& cur_moment = dst.at(dst.size() - 1);
            if (m.player_id != last_player_id && player_in_moment < 10) {
                cur_moment.players[player_in_moment].team_id = m.team_id;
                cur_moment.players[player_in_moment].player_id = m.player_id;
                cur_moment.players[player_in_moment].x_loc = m.x_loc;
                cur_moment.players[player_in_moment].y_loc = m.y_loc;
                cur_moment.players[player_in_moment].radius = m.radius;
                player_in_moment++;
                last_player_id = m.player_id;
            }
            // if this event and moment in event not already accounted for, add it
            event_moment_data emd{m.event_id, m.moment_in_event};
            if (std::find(cur_moment.events.begin(), cur_moment.events.end(), emd) == cur_moment.events.end()) {
                cur_moment.events.push_back(emd);
            }
        }
    }
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
    e.player1_team_id = stod_with_default(col);
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
    e.player2_team_id = stod_with_default(col);
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
    e.player3_team_id = stod_with_default(col);
    std::getline(ss, e.player3_team_city, ',');
    std::getline(ss, e.player3_team_nickname, ',');
    std::getline(ss, e.player3_team_abbreviation, ',');
}

/* load a CSV file of shots with a header row */
size_t load_show_rows(string filepath, shot * s_arr[]) {
    size_t num_rows = 0;
    std::fstream count_f, load_f;
    count_f.open(filepath);
    string row;
    while(std::getline(count_f, row)) {
        if (row.length() > 0) {
            num_rows++;
        }
    }
    // don't count header rows
    num_rows--;
    count_f.close();
    load_f.open(filepath);
    *s_arr = new shot[num_rows];
    // load first row to skip header
    std::getline(load_f, row);
    for (size_t i = 0; i < num_rows; i++) {
        std::getline(load_f, row);
        load_shot_row(row, (*s_arr)[i]);
    }
    return num_rows;
}
void load_shot_rows_vec(istream& rows, vector<shot>& shs) {
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
    sh.loc_x = stod_with_default(col);
    std::getline(ss, col, ',');
    sh.loc_y = stod_with_default(col);
    std::getline(ss, col, ',');
    sh.minutes_remaining = stoi_with_default(col);
    std::getline(ss, col, ',');
    sh.period = stoi_with_default(col);
    std::getline(ss, col, ',');
    sh.player_id = stoi_with_default(col);
    std::getline(ss, sh.player_name, ',');
    std::getline(ss, col, ',');
    sh.quarter = stod_with_default(col);
    std::getline(ss, col, ',');
    sh.seconds_remaining = stoi_with_default(col);
    std::getline(ss, col, ',');
    sh.shot_attempted_flag = stoi_with_default(col);
    std::getline(ss, col, ',');
    sh.shot_distance = stoi_with_default(col);
    std::getline(ss, col, ',');
    sh.shot_made_flag = stoi_with_default(col);
    std::getline(ss, col, ',');
    sh.shot_time = stod_with_default(col);
    std::getline(ss, sh.shot_type, ',');
    std::getline(ss, sh.shot_zone_area, ',');
    std::getline(ss, sh.shot_zone_basic, ',');
    std::getline(ss, sh.shot_zone_range, ',');
    std::getline(ss, col, ',');
    sh.team_id = stod_with_default(col);
    std::getline(ss, sh.team_name, ',');
    std::getline(ss, sh.team_vtm, ',');
}
