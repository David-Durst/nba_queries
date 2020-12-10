#ifndef FIND_TRAJECTORIES_H
#define FIND_TRAJECTORIES_H
#include <string>
#include <vector>
#include "query_structs.h"
using std::vector;

struct trajectory_data {
    long int team_id;
    int player_id;
    float start_x_loc;
    float start_y_loc;
    float start_game_clock;
    float end_x_loc;
    float end_y_loc;
    float end_game_clock;
    short int quarter;
} ;

/* For a list of moments from one game, find all the trajectories that are over the specified distances in x,y,t. */
void find_trajectories_no_fixed_origin(vector<moment>& moments, vector<trajectory_data>& shots_and_players,
        float x_offset=10, float y_offset=10, float z_offset=10);
#endif
