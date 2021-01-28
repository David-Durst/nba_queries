#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include "query_structs.h"
#include "load_data.h"
#include "csv.hpp"
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
    int64_t row_num = 0;
    while(std::getline(rows, row)) {
        moment m;
        load_moment_row(row, m, row_num);
        ms.push_back(m);
        row_num++;
    }
}

void load_moment_row(string& row, moment& m, int64_t row_num) {
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
    m.internal_id = row_num;
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
    const char * c = row.c_str();
    size_t next_sep, frac_start;

    // load ball
    m.ball.team_id  = std::atol(c);
    next_sep = row.find(',') + 1;
    m.ball.player_id = std::atoi(c + next_sep);
    next_sep = row.find(',', next_sep) + 1;
    frac_start = row.find('.', next_sep) + 1;
    m.ball.x_loc = std::atoi(c + next_sep) + (std::atoi(c + frac_start) / 10000.0);
    next_sep = row.find(',', frac_start) + 1;
    frac_start = row.find('.', next_sep) + 1;
    m.ball.y_loc = std::atoi(c + next_sep) + (std::atoi(c + frac_start) / 10000.0);
    next_sep = row.find(',', frac_start) + 1;
    frac_start = row.find('.', next_sep) + 1;
    m.ball.radius = std::atoi(c + next_sep) + (std::atoi(c + frac_start) / 10000.0);

    // load players
    for (int i = 0; i < 10; i++) {
        next_sep = row.find(',', next_sep) + 1;
        m.players[i].team_id = std::atol(c + next_sep);
        next_sep = row.find(',', next_sep) + 1;
        m.players[i].player_id = std::atoi(c + next_sep);
        next_sep = row.find(',', next_sep) + 1;
        frac_start = row.find('.', next_sep) + 1;
        m.players[i].x_loc = std::atoi(c + next_sep) + (std::atoi(c + frac_start) / 10000.0);
        next_sep = row.find(',', frac_start) + 1;
        frac_start = row.find('.', next_sep) + 1;
        m.players[i].y_loc = std::atoi(c + next_sep) + (std::atoi(c + frac_start) / 10000.0);
        next_sep = row.find(',', frac_start) + 1;
        frac_start = row.find('.', next_sep) + 1;
        m.players[i].radius = std::atoi(c + next_sep) + (std::atoi(c + frac_start) / 10000.0);
    }

    next_sep = row.find(',', next_sep) + 1;
    frac_start = row.find('.', next_sep) + 1;
    m.game_clock = clock_fixed_point(std::atoi(c + next_sep) + (std::atoi(c + frac_start) / 10000.0));
    next_sep = row.find(',', next_sep) + 1;
    frac_start = row.find('.', next_sep) + 1;
    m.shot_clock = std::atoi(c + next_sep) + (std::atoi(c + frac_start) / 10000.0);
    next_sep = row.find(',', next_sep) + 1;
    m.quarter = std::atoi(c + next_sep);
    next_sep = row.find(',', next_sep) + 1;
    m.game_id = std::atol(c + next_sep);
    next_sep = row.find(',', next_sep) + 1;
    m.game_num = std::atol(c + next_sep);

    /*
    // load event data, add 1 after for check so npos is valid comparison
    for (next_sep = row.find(',', next_sep); next_sep != std::string::npos; next_sep = row.find(';', next_sep)) {
        next_sep += 2;
        long int event_id = std::atol(c + next_sep);
        next_sep += 2;
        int moment_in_event = std::atol(c + next_sep);
        m.events.push_back({event_id, moment_in_event});
    }
     */
}

void cleaned_moment_from_moment(const moment& m, cleaned_moment& cm, long int& cur_game_id, int& game_num,
                                std::map<long int, int>& game_id_to_num) {
    cm.game_clock = clock_fixed_point(m.game_clock);
    cm.shot_clock = m.shot_clock;
    cm.quarter = m.quarter;
    cm.game_id = m.game_id;
    cm.events = {event_moment_data{m.event_id, m.moment_in_event}};
    cm.ball.team_id = m.team_id;
    cm.ball.player_id = m.player_id;
    cm.ball.x_loc = m.x_loc;
    cm.ball.y_loc = m.y_loc;
    cm.ball.radius = m.radius;
    if (cm.game_id != cur_game_id) {
        game_num++;
        cur_game_id = cm.game_id;
    }
    cm.game_num = game_num;
    game_id_to_num[m.game_id] = game_num;

}

// insert until filled up to (and including) 0.04 before target
void fill_repeated_moments(vector<cleaned_moment>& dst, clock_fixed_point target) {
    while (dst.at(dst.size() - 1).game_clock.abs_diff(clock_fixed_point(target)).gt(0.04)) {
        cleaned_moment copied_moment = dst.at(dst.size() - 1);
        copied_moment.game_clock = clock_fixed_point(copied_moment.game_clock.to_double() - 0.04);
        dst.push_back(copied_moment);
    }
}

/* load a CSV file of cleaned_moments with a header row */
void clean_moment_rows(vector<moment>& src, vector<cleaned_moment>& dst, std::map<long int, int>& game_id_to_num,
                       vector<extra_game_data>& extra_data) {
    if (src.empty()) {
        return;
    }
    int player_in_moment = 0;
    long int last_player_id = -2;
    int game_num = dst.empty() ? 0 : dst.at(dst.size() - 1).game_num + 1;
    long int cur_game_id = src.at(0).game_id;
    for (const auto & m : src) {
        // remove last element if no time is on the clock to simplify indexing logic
        if (!dst.empty() && dst.at(dst.size() - 1).quarter != m.quarter &&
            dst.at(dst.size() - 1).game_clock == clock_fixed_point(0.0)) {
            dst.pop_back();
        }
        if (dst.empty() ||
            dst.at(dst.size() - 1).game_clock != clock_fixed_point(m.game_clock)) {
            // drop moments before quarter starts
            if ((m.quarter < 5 && clock_fixed_point(m.game_clock).gt(clock_fixed_point(720.0))) ||
                    (m.quarter >= 5 && clock_fixed_point(m.game_clock).gt(clock_fixed_point(300.0)))) {
                continue;
            }
            // handle quarters that don't end with 0.04
            if (!dst.empty() && dst.at(dst.size() - 1).quarter != m.quarter) {
                fill_repeated_moments(dst, clock_fixed_point(0.0));
            }
            // handle quarters that start after 720 or 300 (regulation or ot)
            // only need to insert 1 here, as skips after first insertion will be handled by following for loop
            if ((dst.empty() || dst.at(dst.size() - 1).quarter != m.quarter) &&
                    ((m.quarter < 5 && clock_fixed_point(m.game_clock) != clock_fixed_point(720.0)) ||
                    // ot starts at 5 minutes, not 12
                    (m.quarter >= 5 && clock_fixed_point(m.game_clock) != clock_fixed_point(300.0)))) {
                dst.push_back(cleaned_moment());
                cleaned_moment& game_start = dst.at(dst.size() - 1);
                cleaned_moment_from_moment(m, game_start, cur_game_id, game_num, game_id_to_num);
                if (m.quarter < 5) {
                    game_start.game_clock = clock_fixed_point(720.0);
                }
                else {
                    game_start.game_clock = clock_fixed_point(300.0);
                }
            }
            // handle skips after first time in the middle of a quarter
            if (!dst.empty() && dst.at(dst.size() - 1).quarter == m.quarter) {
                fill_repeated_moments(dst, m.game_clock);
            }

            player_in_moment = 0;
            last_player_id = m.player_id;
            // since sorting player_ids, first one in the moment is the ball
            dst.push_back(cleaned_moment());
            cleaned_moment& cur_moment = dst.at(dst.size() - 1);
            cleaned_moment_from_moment(m, cur_moment, cur_game_id, game_num, game_id_to_num);

            // track extra data for this game, like how many ot periods see so far
            if (extra_data.size() == game_num) {
                extra_data.push_back({m.game_id, game_num, 0});
            }
            extra_data.at(game_num).num_ot_periods = std::max(extra_data.at(game_num).num_ot_periods, m.quarter - 4);
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
    // remove last element if no time is on the clock to simplify indexing logic
    // need to repeat this and next check since last time step is never checked
    if (!dst.empty() && dst.at(dst.size() - 1).game_clock == clock_fixed_point(0.0)) {
        dst.pop_back();
    }
    // fill last quarter of last game
    if (!dst.empty()) {
        fill_repeated_moments(dst, clock_fixed_point(0.0));
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
    sh.shot_time = clock_fixed_point(stod_with_default(col));
    std::getline(ss, sh.shot_type, ',');
    std::getline(ss, sh.shot_zone_area, ',');
    std::getline(ss, sh.shot_zone_basic, ',');
    std::getline(ss, sh.shot_zone_range, ',');
    std::getline(ss, col, ',');
    sh.team_id = std::round(stod_with_default(col));
    std::getline(ss, sh.team_name, ',');
    std::getline(ss, sh.team_vtm, ',');
}


void load_cleaned_shot_rows_vec(istream& rows, vector<cleaned_shot>& shs) {
    string row;
    std::getline(rows, row);
    while(std::getline(rows, row)) {
        cleaned_shot sh;
        load_cleaned_shot_row(row, sh);
        shs.push_back(sh);
    }
}

void load_cleaned_shot_row(string& row, cleaned_shot& sh) {
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
    std::getline(ss, col, ',');
    sh.game_num = stoi_with_default(col);
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
    sh.shot_time = clock_fixed_point(stod_with_default(col));
    std::getline(ss, sh.shot_type, ',');
    std::getline(ss, sh.shot_zone_area, ',');
    std::getline(ss, sh.shot_zone_basic, ',');
    std::getline(ss, sh.shot_zone_range, ',');
    std::getline(ss, col, ',');
    sh.team_id = std::round(stod_with_default(col));
    std::getline(ss, sh.team_name, ',');
    std::getline(ss, sh.team_vtm, ',');
}

void clean_shot_rows(vector<shot>& src, vector<cleaned_shot>& dst, std::map<long int, int>& game_id_to_num) {
    for (const auto & s : src) {
        if (game_id_to_num.count(s.game_id) == 0) {
            continue;
        }
        cleaned_shot cs;
        cs.action_type = s.action_type;
        cs.event_time = s.event_time;
        cs.event_type = s.event_type;
        cs.game_date = s.game_date;
        cs.game_event_id = s.game_event_id;
        cs.game_id = s.game_id;
        cs.game_num = game_id_to_num.at(s.game_id);
        cs.grid_type = s.grid_type;
        cs.htm = s.htm;
        cs.loc_x = s.loc_x;
        cs.loc_y = s.loc_y;
        cs.minutes_remaining = s.minutes_remaining;
        cs.period = s.period;
        cs.player_id = s.player_id;
        cs.player_name = s.player_name;
        cs.quarter = std::round(s.quarter);
        cs.seconds_remaining = s.seconds_remaining;
        cs.shot_attempted_flag = s.shot_attempted_flag;
        cs.shot_distance = s.shot_distance;
        cs.shot_made_flag = s.shot_made_flag;
        cs.shot_time = s.shot_time;
        cs.shot_type = s.shot_type;
        cs.shot_zone_area = s.shot_zone_area;
        cs.shot_zone_basic = s.shot_zone_basic;
        cs.shot_zone_range = s.shot_zone_range;
        cs.team_id = s.team_id;
        cs.team_name = s.team_name;
        cs.team_vtm = s.team_vtm;
        dst.push_back(cs);
    }
}

void load_extra_game_data_rows(istream& rows, vector<extra_game_data>& shs) {
    string row;
    std::getline(rows, row);
    while(std::getline(rows, row)) {
        extra_game_data sh;
        load_extra_game_data_row(row, sh);
        shs.push_back(sh);
    }
}

void load_extra_game_data_row(string& row, extra_game_data& sh) {
    string col;
    stringstream ss(row);

    std::getline(ss, col, ',');
    sh.game_id = stol_with_default(col);
    std::getline(ss, col, ',');
    sh.game_num = stoi_with_default(col);
    std::getline(ss, col, ',');
    sh.num_ot_periods = stoi_with_default(col);
}
