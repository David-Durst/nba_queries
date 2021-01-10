#ifndef COL_STORES_H
#define COL_STORES_H
#include "query_structs.h"
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
    // the first player is always the ball
    long int * team_id[11];
    int * player_id[11];
    double * x_loc[11];
    double * y_loc[11];
    double * radius[11];
    clock_fixed_point * game_clock;
    double * shot_clock;
    short int * quarter;
    long int * game_id;
    vector<event_moment_data> * events;

    moment_col_store(vector<cleaned_moment>& moments);
};

#endif //COL_STORES_H
