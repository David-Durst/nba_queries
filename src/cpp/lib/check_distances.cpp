#include <cmath>
#include <iostream>
#include <vector>
#include <algorithm>
#include "query_structs.h"
#include "check_distances.h"
using std::vector;
using std::string;


/* For a list of shots (from possibly more than one game) and a list of moments
   from one game, find all the shots for that game and the player data. */
void find_nearest_defender_at_each_shot(vector<moment>& moments,
                                        vector<shot>& shots,
                                        vector<shot_and_player_data>& shots_and_players,
                                        float time_delta) {
    int cur_moment_idx = 0;
    int cur_shot_idx = 0;
    float last_shot_time = 900.0f;
    while (cur_moment_idx < (int) moments.size() && cur_shot_idx < (int) shots.size()) {
        const shot& cur_shot = shots.at(cur_shot_idx);
        const moment& cur_moment = moments.at(cur_moment_idx);
        // enter this case if shot matches the ball's entry at time of shot
        if (cur_moment.game_id == cur_shot.game_id &&
            //cur_moment.event_id == cur_shot.game_event_id &&
            cur_moment.game_clock == cur_shot.shot_time &&
            cur_moment.quarter == cur_shot.period &&
            cur_moment.player_id == -1) {
            const moment & shooter_moment = get_shooter_team(moments, cur_moment_idx, cur_shot);
            const moment & nearest_forwards = get_nearest_defender(moments, cur_moment_idx,
                                                           shooter_moment, time_delta, true);
            const moment & nearest_backwards = get_nearest_defender(moments, cur_moment_idx,
                                                            shooter_moment, time_delta, false);
            float forwards_distance = compute_distance(shooter_moment, nearest_forwards);
            float backwards_distance = compute_distance(shooter_moment, nearest_backwards);
            moment nearest_defender = (forwards_distance <= backwards_distance) ?
                nearest_forwards : nearest_backwards;
            float nearest_distance = (forwards_distance <= backwards_distance) ?
                forwards_distance : backwards_distance;
            shot_and_player_data result;
            result.offense_team_id = shooter_moment.team_id;
            result.offense_player_id = shooter_moment.player_id;
            result.offense_x_loc = shooter_moment.x_loc;
            result.offense_y_loc = shooter_moment.y_loc;
            result.defense_team_id = nearest_defender.team_id;
            result.defense_player_id = nearest_defender.player_id;
            result.defense_x_loc = nearest_defender.x_loc;
            result.defense_y_loc = nearest_defender.y_loc;
            result.defender_distance = nearest_distance;
            result.game_clock = clock_fixed_point(shooter_moment.game_clock);
            result.shot_clock = shooter_moment.shot_clock;
            result.quarter = shooter_moment.quarter;
            result.game_id = shooter_moment.game_id;
            result.event_id = shooter_moment.event_id;
            result.moment_in_event = shooter_moment.moment_in_event;
            result.shot_attempted = cur_shot.shot_attempted_flag;
            result.shot_made = cur_shot.shot_made_flag;
            shots_and_players.push_back(result);
            cur_shot_idx++;
            last_shot_time = cur_shot.shot_time.to_double();
        }
        // skip bugged, repeated shots
        else if (last_shot_time == cur_shot.shot_time) {
            cur_shot_idx++;
        }
        // if not a match, increment either
        // (a) shot until it matches current game_id
        // (b) cur_moment to the ball's next location
        else {
            if (cur_moment.game_id != cur_shot.game_id) {
                do {
                    cur_shot_idx++;
                } while (cur_shot_idx < (int) shots.size() && cur_moment.game_id != shots.at(cur_shot_idx).game_id);
                last_shot_time = cur_shot.shot_time.to_double();
            }
            else if (shot_before_moment(cur_shot, cur_moment)) {
                do {
                    cur_shot_idx++;
                } while (cur_shot_idx < (int) shots.size() && shot_before_moment(shots.at(cur_shot_idx), cur_moment));
                last_shot_time = cur_shot.shot_time.to_double();
            }
            else {
                do {
                    cur_moment_idx++;
                } while (cur_moment_idx < (int) moments.size()
                    && (moments.at(cur_moment_idx).player_id != -1)// || moments.at(cur_moment_idx).event_id != cur_shot.game_event_id)
                    && moment_before_shot(moments.at(cur_moment_idx), cur_shot));
            }
        }
    }
}


/* For a shot, find the shooter */
const moment & get_shooter_team(vector<moment>& moments, int cur_moment_idx,
                        shot cur_shot) {
    for (; moments.at(cur_moment_idx).player_id != cur_shot.player_id &&
             // stop 1 early so don't run off end of array,
             // just return last if no matches
             cur_moment_idx < (int) moments.size() - 1;
         cur_moment_idx++) ;
    return moments.at(cur_moment_idx);
}

// default if no defenders before the current player
moment really_far_player = {-2, -2, 1000000, 100000, 100000, 0, 0, 0, 0, 0, 0};
/* For a shot, find the nearest defender within time_delta seconds.
   forward_in_time if to search forwards (aka +time_delta) if true or
   backwards (aka -time_delta)
*/
const moment & get_nearest_defender(vector<moment>& moments, int ball_moment_at_shot_idx,
                            moment shooter_moment, float time_delta,
                            bool forward_in_time) {
    float closest_distance;
    int closest_defender_idx = -1;
    for (int cur_moment_idx = ball_moment_at_shot_idx;
         std::abs(moments.at(cur_moment_idx).game_clock - shooter_moment.game_clock) <= time_delta &&
             // stop 1 early so don't run off end of array,
             // just return last if no matches
             // need to check differently for backwards and forwards
             (forward_in_time ? cur_moment_idx < (int) moments.size() - 1 : cur_moment_idx > 0);
         forward_in_time ? cur_moment_idx++ : cur_moment_idx--) {
        // get the nearest non-ball and defender
        if (moments.at(cur_moment_idx).player_id != -1 && moments.at(cur_moment_idx).team_id != shooter_moment.team_id) {
            float new_distance = compute_distance(moments.at(cur_moment_idx), shooter_moment);
            // take first defender if closest defender has not been set
            // otherwise take closest defender
            if (closest_defender_idx == -1 ||
                new_distance < closest_distance) {
                closest_defender_idx = cur_moment_idx;
                closest_distance = new_distance;
            }
        }
    }
    if (closest_defender_idx == -1) {
        return really_far_player;
    }
    else {
        return moments.at(closest_defender_idx);
    }
}

bool operator==(shot_and_player_data const & lhs, shot_and_player_data const & rhs) {
    return
            lhs.offense_team_id == rhs.offense_team_id &&
            lhs.offense_player_id == rhs.offense_player_id &&
            lhs.offense_x_loc == rhs.offense_x_loc &&
            lhs.offense_y_loc == rhs.offense_y_loc &&
            lhs.defense_team_id == rhs.defense_team_id &&
            lhs.defense_player_id == rhs.defense_player_id &&
            lhs.defense_x_loc == rhs.defense_x_loc &&
            lhs.defense_y_loc == rhs.defense_y_loc &&
            lhs.defender_distance == rhs.defender_distance &&
            lhs.game_clock == rhs.game_clock &&
            lhs.shot_clock == rhs.shot_clock &&
            lhs.quarter == rhs.quarter &&
            lhs.game_id == rhs.game_id &&
            lhs.event_id == rhs.event_id &&
            lhs.moment_in_event == rhs.moment_in_event &&
            lhs.shot_attempted == rhs.shot_attempted &&
            lhs.shot_made == rhs.shot_made;
}
std::ostream& operator<<(std::ostream& os, shot_and_player_data const& value) {
    os << "offense_team_id: " << value.offense_team_id
       << ", offense_player_id: " << value.offense_player_id
       << ", offense_x_loc: " << value.offense_x_loc
       << ", offense_y_loc: " << value.offense_y_loc
       << ", defense_team_id: " << value.defense_team_id
       << ", defense_player_id: " << value.defense_player_id
       << ", defense_x_loc: " << value.defense_x_loc
       << ", defense_y_loc: " << value.defense_y_loc
       << ", defender_distance: " << value.defender_distance
       << ", game_clock: " << value.game_clock.to_double()
       << ", shot_clock: " << value.shot_clock
       << ", quarter: " << value.quarter
       << ", game_id: " << value.game_id
       << ", event_id: " << value.event_id
       << ", moment_in_event: " << value.moment_in_event
       << ", shot_attempted: " << value.shot_attempted
       << ", shot_made: " << value.shot_made;
    return os;
}
void print_shot_and_player_data_csv(std::ostream& os, const shot_and_player_data& value) {
    os << value.offense_team_id
       << value.offense_player_id
       << value.offense_x_loc
       << value.offense_y_loc
       << value.defense_team_id
       << value.defense_player_id
       << value.defense_x_loc
       << value.defense_y_loc
       << value.defender_distance
       << value.game_clock.to_double()
       << value.shot_clock
       << value.quarter
       << value.game_id
       << value.event_id
       << value.moment_in_event
       << value.shot_attempted
       << value.shot_made;
}

bool operator==(shot_distance_bucket const& lhs, shot_distance_bucket const& rhs) {
    return
            lhs.distance == rhs.distance &&
            lhs.num_shot_made == rhs.num_shot_made &&
            lhs.num_shot_attempts == lhs.num_shot_attempts;
}

std::ostream& operator<<(std::ostream& os, const shot_distance_bucket& value) {
    os << "distance: " << value.distance
       << "num_shot_made: " << value.num_shot_made
       << ", num_shot_attempt: " << value.num_shot_attempts;
    return os;
}

void print_shot_distance_bucket_csv(std::ostream& os, const shot_distance_bucket& value) {
    os << value.distance
       << "," << value.num_shot_made
       << "," << value.num_shot_attempts;
    // adding percent as a helper in csv
    if (value.num_shot_attempts == 0) {
        os << ",0";
    }
    else {
        os << "," << ((float) value.num_shot_made) / value.num_shot_attempts;
    }
}

/* bucket shot_and_player_data by distance, bucket size is 1 foot*/
vector<shot_distance_bucket> bucket_shots_by_distance(vector<shot_and_player_data>& shots_and_players) {
    int max_distance = 0;
    vector<shot_distance_bucket> buckets;
    // always starting with bucket at 0, create buckets for rest of feet out to max distance
    // bucket i is for shots with defender distance (inclusive) (i-1) to i (exclusive). The exception is last bucket,
    // which is include for both sides. This means need buckets from 0 to max_distance-1
    for (const shot_and_player_data & d : shots_and_players) {
        int cur_distance = (int) ceil(d.defender_distance);
        if (cur_distance > max_distance) {
            max_distance = cur_distance;
        }
    }

    // create the buckets
    for (int i = 0; i < max_distance; i++) {
        buckets.push_back(shot_distance_bucket{i, 0, 0});
    }
    buckets.push_back(shot_distance_bucket{-1, 0, 0});


    // fill out the buckets
    for (const shot_and_player_data & d : shots_and_players) {
        if (!d.shot_valid) {
            buckets.at(buckets.size()-1).num_shot_attempts++;
            continue;
        }
        // if max_distance, put in bucket max_distance - 1
        int cur_distance = std::min((int) floor(d.defender_distance), max_distance-1);
        shot_distance_bucket & b = buckets.at(cur_distance);
        if (d.shot_attempted) {
            b.num_shot_attempts++;
        }
        if (d.shot_made) {
            b.num_shot_made++;
        }
    }

    return buckets;
}
