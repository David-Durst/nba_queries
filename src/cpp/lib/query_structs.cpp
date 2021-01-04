#include <iostream>
#include <string>
#include "query_structs.h"

bool operator==(moment const& lhs, moment const& rhs) {
    return
            lhs.team_id == rhs.team_id &&
            lhs.player_id == rhs.player_id &&
            lhs.x_loc == rhs.x_loc &&
            lhs.y_loc == rhs.y_loc &&
            lhs.radius == rhs.radius &&
            lhs.game_clock == rhs.game_clock &&
            lhs.shot_clock == rhs.shot_clock &&
            lhs.quarter == rhs.quarter &&
            lhs.game_id == rhs.game_id &&
            lhs.event_id == rhs.event_id &&
            lhs.moment_in_event == rhs.moment_in_event;
}

std::ostream& operator<<(std::ostream& os, const moment& value) {
    os << "team_id: " << value.team_id
         << ", player_id: " << value.player_id
         << ", x_loc: " << value.x_loc
         << ", y_loc: " << value.y_loc
         << ", radius: " << value.radius
         << ", game_clock: " << value.game_clock
         << ", shot_clock: " << value.shot_clock
         << ", quarter: " << value.quarter
         << ", game_id: " << value.game_id
         << ", event_id: " << value.event_id
         << ", moment_in_event: " << value.moment_in_event;
    return os;
}

void print_moment_csv(std::ostream& os, const moment& value) {
    os << value.team_id
       << "," << value.player_id
       << "," << value.x_loc
       << "," << value.y_loc
       << "," << value.radius
       << "," << value.game_clock
       << "," << value.shot_clock
       << "," << value.quarter
       << "," << value.game_id
       << "," << value.event_id
       << "," << value.moment_in_event;
}

bool operator==(player_data const& lhs, player_data const& rhs) {
    return
            lhs.team_id == rhs.team_id &&
            lhs.player_id == rhs.player_id &&
            lhs.x_loc == rhs.x_loc &&
            lhs.y_loc == rhs.y_loc &&
            lhs.radius == rhs.radius;
}

std::ostream& operator<<(std::ostream& os, const player_data& value) {
    os << "team_id: " << value.team_id
       << ", player_id: " << value.player_id
       << ", x_loc: " << value.x_loc
       << ", y_loc: " << value.y_loc
       << ", radius: " << value.radius;
    return os;
}

void print_player_data_csv(std::ostream& os, const player_data& value) {
    os << value.team_id
       << "," << value.player_id
       << "," << value.x_loc
       << "," << value.y_loc
       << "," << value.radius;
}

bool operator==(event_moment_data const& lhs, event_moment_data const& rhs) {
    return
            lhs.event_id == rhs.event_id &&
            lhs.moment_in_event == rhs.moment_in_event;
}

std::ostream& operator<<(std::ostream& os, const event_moment_data& value) {
    os << "event_id: " << value.event_id
       << ", moment_in_event: " << value.moment_in_event;
    return os;
}

void print_event_moment_data_csv(std::ostream& os, const event_moment_data& value) {
    os << "(" << value.event_id
       << ":" << value.moment_in_event
       << ")";
}

bool operator==(clock_fixed_point const& lhs, clock_fixed_point const& rhs) {
    return
            lhs.seconds == rhs.seconds &&
            lhs.twenty_fifths_of_second == rhs.twenty_fifths_of_second;
}

bool operator!=(clock_fixed_point const& lhs, clock_fixed_point const& rhs) { return !(lhs == rhs); }

bool operator==(cleaned_moment const& lhs, cleaned_moment const& rhs) {
    bool all_players_eq = lhs.ball == rhs.ball;
    for (int i = 0; i < 10; i++) {
        all_players_eq &= lhs.players[i] == rhs.players[i];
    }
    return
            all_players_eq &&
            lhs.game_clock == rhs.game_clock &&
            lhs.shot_clock == rhs.shot_clock &&
            lhs.quarter == rhs.quarter &&
            lhs.game_id == rhs.game_id &&
            lhs.events == rhs.events;
}

std::ostream& operator<<(std::ostream& os, const cleaned_moment& value) {
    os << "team_id_ball: " << value.ball.team_id
       << ", player_id_ball: " << value.ball.player_id
       << ", x_loc_ball: " << value.ball.x_loc
       << ", y_loc_ball: " << value.ball.y_loc
       << ", radius_ball: " << value.ball.radius;
    for (int i = 0; i < 10; i++) {
        os << ", team_id_" << i << ": " << value.players[i].team_id
           << ", player_id_" << i << ": " << value.players[i].player_id
           << ", x_loc_" << i << ": " << value.players[i].x_loc
           << ", y_loc_" << i << ": " << value.players[i].y_loc
           << ", radius_" << i << ": " << value.players[i].radius;
    }
    os << ", game_clock: " << value.game_clock.to_double()
       << ", shot_clock: " << value.shot_clock
       << ", quarter: " << value.quarter
       << ", game_id: " << value.game_id
       << ", events: ";
    for (auto const & e : value.events) {
        os << "(" << e << ")" << ";";
    }
    return os;
}

void print_cleaned_moment_csv(std::ostream& os, const cleaned_moment& value) {
    print_player_data_csv(os, value.ball);
    for (int i = 0; i < 10; i++) {
        os << ",";
        print_player_data_csv(os, value.players[i]);
    }
    os << "," << value.game_clock.to_double()
       << "," << value.shot_clock
       << "," << value.quarter
       << "," << value.game_id << ",";
    for (auto const & e : value.events) {
        print_event_moment_data_csv(os, e);
        os << ";";
    }
}

bool operator==(shot const &lhs, shot const &rhs) {
    return
            lhs.action_type == rhs.action_type &&
            lhs.event_time == rhs.event_time &&
            lhs.event_type == rhs.event_type &&
            lhs.game_date == rhs.game_date &&
            lhs.game_event_id == rhs.game_event_id &&
            lhs.game_id == rhs.game_id &&
            lhs.grid_type == rhs.grid_type &&
            lhs.htm == rhs.htm &&
            lhs.loc_x == rhs.loc_x &&
            lhs.loc_y == rhs.loc_y &&
            lhs.minutes_remaining == rhs.minutes_remaining &&
            lhs.period == rhs.period &&
            lhs.player_id == rhs.player_id &&
            lhs.player_name == rhs.player_name &&
            lhs.quarter == rhs.quarter &&
            lhs.seconds_remaining == rhs.seconds_remaining &&
            lhs.shot_attempted_flag == rhs.shot_attempted_flag &&
            lhs.shot_distance == rhs.shot_distance &&
            lhs.shot_made_flag == rhs.shot_made_flag &&
            lhs.shot_time == rhs.shot_time &&
            lhs.shot_type == rhs.shot_type &&
            lhs.shot_zone_area == rhs.shot_zone_area &&
            lhs.shot_zone_basic == rhs.shot_zone_basic &&
            lhs.shot_zone_range == rhs.shot_zone_range &&
            lhs.team_id == rhs.team_id &&
            lhs.team_name == rhs.team_name &&
            lhs.team_vtm == rhs.team_vtm;
}

std::ostream &operator<<(std::ostream &os, const shot &value) {
    os << "team_id: " << value.team_id
       << ", action_type: " << value.action_type
       << ", event_time: " << value.event_time
       << ", event_type: " << value.event_type
       << ", game_date: " << value.game_date
       << ", game_event_id: " << value.game_event_id
       << ", game_id: " << value.game_id
       << ", grid_type: " << value.grid_type
       << ", htm: " << value.htm
       << ", loc_x: " << value.loc_x
       << ", loc_y: " << value.loc_y
       << ", minutes_remaining: " << value.minutes_remaining
       << ", period: " << value.period
       << ", player_id: " << value.player_id
       << ", player_name: " << value.player_name
       << ", quarter: " << value.quarter
       << ", seconds_remaining: " << value.seconds_remaining
       << ", shot_attempted_flag: " << value.shot_attempted_flag
       << ", shot_distance: " << value.shot_distance
       << ", shot_made_flag: " << value.shot_made_flag
       << ", shot_time: " << value.shot_time
       << ", shot_type: " << value.shot_type
       << ", shot_zone_area: " << value.shot_zone_area
       << ", shot_zone_basic: " << value.shot_zone_basic
       << ", shot_zone_range: " << value.shot_zone_range
       << ", team_id: " << value.team_id
       << ", team_name: " << value.team_name
       << ", team_vtm: " << value.team_vtm;
    return os;
}
void print_shot_csv(std::ostream& os, const shot& value) {
    os << value.team_id
       << "," << value.action_type
       << "," << value.event_time
       << "," << value.event_type
       << "," << value.game_date
       << "," << value.game_event_id
       << "," << value.game_id
       << "," << value.grid_type
       << "," << value.htm
       << "," << value.loc_x
       << "," << value.loc_y
       << "," << value.minutes_remaining
       << "," << value.period
       << "," << value.player_id
       << "," << value.player_name
       << "," << value.quarter
       << "," << value.seconds_remaining
       << "," << value.shot_attempted_flag
       << "," << value.shot_distance
       << "," << value.shot_made_flag
       << "," << value.shot_time
       << "," << value.shot_type
       << "," << value.shot_zone_area
       << "," << value.shot_zone_basic
       << "," << value.shot_zone_range
       << "," << value.team_id
       << "," << value.team_name
       << "," << value.team_vtm;
}

bool shot_before_moment(const shot & s, const moment & m){
    return s.period < m.quarter || (s.period == m.quarter && s.shot_time > m.game_clock);
}

bool moment_before_shot(const moment & m, const shot & s) {
    return m.quarter < s.period || (m.quarter == s.period && m.game_clock > s.shot_time);
}
