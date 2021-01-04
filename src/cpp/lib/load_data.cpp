#include <string>
#include <vector>
#include <istream>
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

/* load a CSV file of cleaned_moments with a header row */
void load_cleaned_moment_rows(istream& rows, vector<cleaned_moment>& ms) {
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
    m.ball.x_loc = stof_with_default(col);
    std::getline(ss, col, ',');
    m.ball.y_loc = stof_with_default(col);
    std::getline(ss, col, ',');
    m.ball.radius = stof_with_default(col);

    // load players
    for (int i = 0; i < 10; i++) {
        std::getline(ss, col, ',');
        m.players[i].team_id = stol_with_default(col);
        std::getline(ss, col, ',');
        m.players[i].player_id = stoi_with_default(col);
        std::getline(ss, col, ',');
        m.players[i].x_loc = stof_with_default(col);
        std::getline(ss, col, ',');
        m.players[i].y_loc = stof_with_default(col);
        std::getline(ss, col, ',');
        m.players[i].radius = stof_with_default(col);
    }

    std::getline(ss, col, ',');
    m.game_clock = clock_fixed_point(stof_with_default(col));
    std::getline(ss, col, ',');
    m.shot_clock = stof_with_default(col);
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
            if (dst.size() == 3451) {
                clock_fixed_point c = clock_fixed_point(m.game_clock);
                int x = 0;
            }
            // fix case where skip 0.08 by reinserting last value
            int num_inserted = 0;
            size_t cur_size = dst.size();
            while (!dst.empty() && dst.at(dst.size() - 1).quarter == m.quarter &&
                dst.at(dst.size() - 1).game_clock.abs_diff(clock_fixed_point(m.game_clock)).gt(0.04f)) {
                num_inserted++;
                /*
                if (num_inserted > 300) {
                    clock_fixed_point target_clock = clock_fixed_point(m.game_clock);
                    cleaned_moment init_moment = dst.at(cur_size - 1);
                    int x = 0;
                }
                 */
                cleaned_moment copied_moment = dst.at(dst.size() - 1);
                copied_moment.game_clock = clock_fixed_point(copied_moment.game_clock.to_float() - 0.04f);
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
