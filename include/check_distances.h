#ifndef CHECK_DISTANCES_H
#define CHECK_DISTANCES_H
#include <string>
#include <vector>
#include "query_structs.h"
using std::vector;

struct shot_and_player_data {
    long int offense_team_id;
    int offense_player_id;
    float offense_x_loc;
    float offense_y_loc;
    long int defense_team_id;
    int defense_player_id;
    float defense_x_loc;
    float defense_y_loc;
    float defender_distance;
    float game_clock;
    float shot_clock;
    short int quarter;
    long int game_id;
    long int event_id;
    int moment_in_event;
} ;

/* For a list of shots (from possibly more than one game) and a list of moments
   from one game, find all the shots for that game and the player data. */
void find_nearest_defender_at_each_shot(vector<moment>& moments,
                                        vector<shot>& shots,
                                        vector<shot_and_player_data>& shots_and_players,
                                        float time_delta = 2.0);
/* For a shot, find the shooter */
moment get_shooter_team(vector<moment>& moments, int cur_moment_idx,
                        shot cur_shot);
/* For a shot, find the nearest defender within time_delta seconds.
   forward_in_time if to search forwards (aka +time_delta) if true or
   backwards (aka -time_delta)
 */
moment get_nearest_defender(vector<moment>& moments, int ball_moment_at_shot_idx,
                            moment shooter_moment, float time_delta,
                            bool forward_in_time);
/*  Compute the euclidean distance between two moments. */
float compute_distance(moment m1, moment m2);
#endif
