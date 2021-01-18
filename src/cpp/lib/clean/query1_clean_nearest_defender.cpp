#include "clean_queries.h"
#include "query_structs.h"
#include <functional>
#include <iostream>
#include <omp.h>



void find_nearest_defender_at_each_shot_clean(moment_col_store * moments,
                                              shot_col_store * shots,
                                              list<shot_and_player_data> * shots_and_players,
                                              int time_delta_ticks, bool parallel) {
    shot_and_player_data * tmp = new shot_and_player_data[shots->size];
#pragma omp parallel for schedule(dynamic, 20) if(parallel)
    for (int64_t shot_index = 0; shot_index < shots->size; shot_index++) {
        int64_t shooter_moment_index = shots->shot_time[shot_index].time_to_index(shots->period[shot_index]);

        // variables for tracking nearest defender to this shot
        double nearest_defender_distance = 1000000.0;
        int64_t nearest_defender_moment_index = 0;
        int nearest_defender_player_index = 0;
        int shooter_player_id = shots->player_id[shot_index];
        long int shooter_team_id = shots->team_id[shot_index];
        double shooter_x_loc, shooter_y_loc;
        int quarter = shots->quarter[shot_index];
        int game_num = moments->game_num[shooter_moment_index];

        // get data for shooter
        bool found_shooter = false;
        for (int player_index = 1; player_index < 11; player_index++) {
            if (moments->player_id[player_index][shooter_moment_index] == shooter_player_id) {
                shooter_x_loc = moments->x_loc[player_index][shooter_moment_index];
                shooter_y_loc = moments->y_loc[player_index][shooter_moment_index];
                found_shooter = true;
                break;
            }
        }
        // skip this shot if shooter not in the game at the time, which would be bad data
        if (!found_shooter) {
            continue;
        }

        for (int64_t defender_moment_index = shooter_moment_index - time_delta_ticks;
             defender_moment_index < shooter_moment_index + time_delta_ticks + 1;
             defender_moment_index++) {
            // skip out of bounds indices or those in a different quarter from the shooter
            if (defender_moment_index < 0 || defender_moment_index > moments->size ||
                moments->quarter[defender_moment_index] != quarter ||
                moments->game_num != game_num) {
                continue;
            }

            // look for nearest defender at this time step
            for (int player_index = 1; player_index < 11; player_index++) {
                if (moments->team_id[player_index][defender_moment_index] != shooter_team_id) {
                    double defender_distance = hypot(shooter_x_loc - moments->x_loc[player_index][defender_moment_index],
                                                     shooter_y_loc - moments->y_loc[player_index][defender_moment_index]);
                    if (nearest_defender_distance > defender_distance) {
                        nearest_defender_distance = defender_distance;
                        nearest_defender_moment_index = defender_moment_index;
                        nearest_defender_player_index = player_index;
                    }
                }
            }
        }
        // after all time steps for this shot, append the nearest defender
        tmp[shot_index] = {
            shooter_team_id,
            shooter_player_id,
            shooter_x_loc,
            shooter_y_loc,
            moments->team_id[nearest_defender_player_index][nearest_defender_moment_index],
            moments->player_id[nearest_defender_player_index][nearest_defender_moment_index],
            moments->x_loc[nearest_defender_player_index][nearest_defender_moment_index],
            moments->y_loc[nearest_defender_player_index][nearest_defender_moment_index],
            nearest_defender_distance,
            moments->game_clock[shooter_moment_index],
            moments->shot_clock[shooter_moment_index],
            moments->quarter[shooter_moment_index],
            moments->game_id[shooter_moment_index],
            shots->game_event_id[shot_index],
            0, // not worrying about moment in event for right now
            shots->shot_attempted_flag[shot_index],
            shots->shot_made_flag[shot_index]
        };
    }
    for (int i = 0; i < shots->size; i++) {
        shots_and_players->append_node(tmp[i]);
    }
}

