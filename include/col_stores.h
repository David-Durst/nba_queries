#ifndef COL_STORES_H
#define COL_STORES_H
#include "query_structs.h"

#define MAX_X 100
#define MAX_Y 50
#define NUM_PLAYERS_AND_BALL 11

class player_col_store {
public:
    long int * team_id;
    int * player_id;
    double * x_loc;
    double * y_loc;
    double * radius;

    // player_id doesn't matter if ball is set
    player_col_store(vector<cleaned_moment>& moments, int player_id);
};

class moment_col_store {
public:
    size_t size;
    // the first player is always the ball
    long int * team_id[NUM_PLAYERS_AND_BALL];
    int * player_id[NUM_PLAYERS_AND_BALL];
    double * x_loc[NUM_PLAYERS_AND_BALL];
    double * y_loc[NUM_PLAYERS_AND_BALL];
    double * radius[NUM_PLAYERS_AND_BALL];
    clock_fixed_point * game_clock;
    double * shot_clock;
    short int * quarter;
    long int * game_id;
    vector<event_moment_data> * events;

    moment_col_store(vector<cleaned_moment>& moments);
};

class shot_col_store {
public:
    size_t size;
    string * action_type;
    int * event_time;
    string * event_type;
    string * game_date;
    long int * game_event_id;
    long int * game_id;
    string * grid_type;
    string * htm;
    float * loc_x;
    float * loc_y;
    int * minutes_remaining;
    int * period;
    int * player_id;
    string * player_name;
    float * quarter;
    int * seconds_remaining;
    bool * shot_attempted_flag;
    int * shot_distance;
    bool * shot_made_flag;
    clock_fixed_point * shot_time;
    string * shot_type;
    string * shot_zone_area;
    string * shot_zone_basic;
    string * shot_zone_range;
    long int * team_id;
    string * team_name;
    string * team_vtm;

    shot_col_store(vector<shot>& shots);
};

#endif //COL_STORES_H
