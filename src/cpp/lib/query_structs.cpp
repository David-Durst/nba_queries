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

bool operator==(cleaned_moment const& lhs, cleaned_moment const& rhs) {
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
            lhs.event_ids.compare(rhs.event_ids) == 0 &&
            lhs.moment_in_event_ids.compare(rhs.moment_in_event_ids) == 0;
}

std::ostream& operator<<(std::ostream& os, const cleaned_moment& value) {
    os << "team_id: " << value.team_id
       << ", player_id: " << value.player_id
       << ", x_loc: " << value.x_loc
       << ", y_loc: " << value.y_loc
       << ", radius: " << value.radius
       << ", game_clock: " << value.game_clock
       << ", shot_clock: " << value.shot_clock
       << ", quarter: " << value.quarter
       << ", game_id: " << value.game_id
       << ", event_id: " << value.event_ids
       << ", moment_in_event: " << value.moment_in_event_ids;
    return os;
}

void print_cleaned_moment_csv(std::ostream& os, const cleaned_moment& value) {
    os << value.team_id
       << "," << value.player_id
       << "," << value.x_loc
       << "," << value.y_loc
       << "," << value.radius
       << "," << value.game_clock
       << "," << value.shot_clock
       << "," << value.quarter
       << "," << value.game_id
       << "," << value.event_ids
       << "," << value.moment_in_event_ids;
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
