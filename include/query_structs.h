#ifndef QUERY_STRUCTS_H
#define QUERY_STRUCTS_H
#include <string>
using std::string;

struct moment {
    long int team_id;
    int player_id;
    float x_loc;
    float y_loc;
    float radius;
    float game_clock;
    float shot_clock;
    short int quarter;
    long int game_id;
    long int event_id;
    int moment_in_event;
} ;

struct event {
    long int game_id;
    long int event_num;
    int event_msg_type;
    int event_msg_action_type;
    short int period;
    string wc_timestring;
    string pc_timestring;
    string home_description;
    string neutral_description;
    string visitor_description;
    string score;
    string score_margin;
    int person1_type;
    int player1_id;
    string player1_name;
    float player1_team_id;
    string player1_team_city;
    string player1_team_nickname;
    string player1_team_abbreviation;
    int person2_type;
    int player2_id;
    string player2_name;
    float player2_team_id;
    string player2_team_city;
    string player2_team_nickname;
    string player2_team_abbreviation;
    int person3_type;
    int player3_id;
    string player3_name;
    float player3_team_id;
    string player3_team_city;
    string player3_team_nickname;
    string player3_team_abbreviation;
} ;

struct shot {
    string action_type;
    int event_time;
    string event_type;
    string game_date;
    long int game_event_id;
    long int game_id;
    string grid_type;
    string htm;
    float loc_x;
    float loc_y;
    int minutes_remaining;
    int period;
    int player_id;
    string player_name;
    float quarter;
    int seconds_remaining;
    bool shot_attempted_flag;
    int shot_distance;
    bool shot_made_flag;
    float shot_time;
    string shot_type;
    string shot_zone_area;
    string shot_zone_basic;
    string shot_zone_range;
    float team_id;
    string team_name;
    string team_vtm;
} ;
#endif
