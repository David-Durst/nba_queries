#include <cmath>
#include <string>
#include <vector>
#include <istream>
#include <sstream>
#include <iostream>
#include "query_structs.h"
#include "check_distances.h"
using std::vector;
using std::string;


/* For a list of shots (from possibly more than one game) and a list of moments
   from one game, find all the shots for that game and the player data. */
void find_nearest_defender_at_each_shot(vector<moment>& moments,
                                        vector<shot> shots,
                                        vector<shot_and_player_data> shots_and_players) {
    int cur_moment_idx = 0;
    int cur_shot_idx = 0;
    shot cur_shot;
    moment cur_moment;
    while (cur_moment_idx < (int) moments.size() && cur_shot_idx < (int) shots.size()) {
        cur_shot = shots.at(cur_shot_idx);
        cur_moment = moments.at(cur_moment_idx);
        // enter this case if shot matches the ball's entry at time of shot
        if (cur_moment.event_id == cur_shot.game_event_id &&
            cur_moment.game_clock == cur_shot.shot_time &&
            cur_moment.quarter == cur_shot.period &&
            cur_moment.player_id == -1) {
            moment shooter_moment = get_shooter_team(moments, cur_moment_idx, cur_shot);
            moment nearest_forwards = get_nearest_defender(moments, cur_moment_idx,
                                                           shooter_moment, 2.0, true);
            moment nearest_backwards = get_nearest_defender(moments, cur_moment_idx,
                                                            shooter_moment, 2.0, true);
            float forwards_distance = compute_distance(cur_moment, nearest_forwards);
            float backwards_distance = compute_distance(cur_moment, nearest_backwards);
            moment nearest_defender = (forwards_distance <= backwards_distance) ?
                nearest_forwards : nearest_backwards;
            float nearest_distance = (forwards_distance <= backwards_distance) ?
                forwards_distance : backwards_distance;
            shot_and_player_data result;
            result.offense_team_id = cur_moment.team_id;
            result.offense_player_id = cur_moment.player_id;
            result.offense_x_loc = cur_moment.x_loc;
            result.offense_y_loc = cur_moment.y_loc;
            result.defense_team_id = nearest_defender.team_id;
            result.defense_player_id = nearest_defender.player_id;
            result.defense_x_loc = nearest_defender.x_loc;
            result.defense_y_loc = nearest_defender.y_loc;
            result.defender_distance = nearest_distance;
            result.game_clock = cur_moment.game_clock;
            result.shot_clock = cur_moment.shot_clock;
            result.quarter = cur_moment.quarter;
            result.game_id = cur_moment.game_id;
            result.event_id = cur_moment.event_id;
            result.moment_in_event = cur_moment.moment_in_event;
            shots_and_players.push_back(result);
            cur_shot_idx++;
        }
        // if not a match, increment either
        // (a) shot until it matches current game_id
        // (b) cur_moment to the ball's next location
        else {
            if (cur_moment.game_id != cur_shot.game_id) {
                do {
                    cur_shot_idx++;
                } while (cur_moment.game_id != cur_shot.game_id
                         && cur_shot_idx < (int) shots.size());
            }
            else {
                do {
                    cur_moment_idx++;
                } while (moments.at(cur_moment_idx).player_id != -1
                         && cur_moment_idx < (int) moments.size());
            }
        }
    }
}


/* For a shot, find the shooter */
moment get_shooter_team(vector<moment>& moments, int cur_moment_idx,
                        shot cur_shot) {
    for (; moments.at(cur_moment_idx).player_id != cur_shot.player_id &&
             // stop 1 early so don't run off end of array,
             // just return last if no matches
             cur_moment_idx < (int) moments.size() - 1;
         cur_moment_idx++) ;
    return moments.at(cur_moment_idx);
}

/* For a shot, find the nearest defender within time_delta seconds.
   forward_in_time if to search forwards (aka +time_delta) if true or
   backwards (aka -time_delta)
*/
moment get_nearest_defender(vector<moment>& moments, int ball_moment_at_shot_idx,
                            moment shooter_moment, float time_delta,
                            bool forward_in_time) {
    moment closest_defender;
    float closest_distance;
    int cur_moment_idx = ball_moment_at_shot_idx;
    closest_defender.player_id = -1;
    for (moment cur_moment = moments.at(cur_moment_idx);
         std::abs(moments.at(cur_moment_idx).game_clock - shooter_moment.game_clock) <= time_delta &&
             // stop 1 early so don't run off end of array,
             // just return last if no matches
             cur_moment_idx < (int) moments.size() - 1;
         forward_in_time ? cur_moment_idx++ : cur_moment_idx--,
         cur_moment = moments.at(cur_moment_idx)) {
        // get the nearest non-ball and defender
        if (cur_moment.player_id != -1 && cur_moment.team_id != shooter_moment.team_id) {
            float new_distance = compute_distance(cur_moment, shooter_moment);
            // take first defender if closest defender has not been set
            // otherwise take closest defender
            if (closest_defender.player_id == -1 ||
                new_distance < closest_distance) {
                closest_defender = cur_moment;
                closest_distance = new_distance;
            }
        }
    }
    return closest_defender;
}

/*  Compute the euclidean distance between two moments. */
float compute_distance(moment m1, moment m2) {
    return std::hypot(m1.x_loc-m2.x_loc, m1.y_loc-m2.y_loc);
}
