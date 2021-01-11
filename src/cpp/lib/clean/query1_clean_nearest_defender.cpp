#include "clean_queries.h"
#include "query_structs.h"
#include <functional>
#include <iostream>



void find_nearest_defender_at_each_shot_clean(moment_col_store * moments,
                                              shot_col_store * shots,
                                              list<shot_and_player_data> * shots_and_players,
                                              int time_delta_ticks) {
    bool first = true;
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
                moments->quarter[defender_moment_index] != quarter) {
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

        if (first) {
            std::cout << "first hit shooter moment index: " << shooter_moment_index
                      << ", nearest defender moment index: " << nearest_defender_moment_index << std::endl;
            first = false;
        }

        // after all time steps for this shot, append the nearest defender
        shots_and_players->append_node({
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
        });
    }
}

/*
cpdef vector[shot_and_player_data] find_nearest_defender_at_each_shot(int tick_delta):
    cdef vector[shot_and_player_data] result
    cdef shot_and_player_data nearest_at_tick
    cdef shot_and_player_data nearest_at_shot
    for s in selected_shot_data:
        shot_moment_index = time_to_index(s.shot_time, s.period)
        for i in range(shot_moment_index - tick_delta,shot_moment_index + tick_delta + 1):
            if i >= moment_data.size():
                break

            nearest_at_tick = nearest_defender_in_moment(moment_data.at(i), s)

            if s.quarter != moment_data.at(i).quarter:
                continue

            if i == shot_moment_index - tick_delta or \
                nearest_at_tick.defender_distance < nearest_at_shot.defender_distance:
                nearest_at_shot = nearest_at_tick

        result.push_back(nearest_at_shot)
    return result

cpdef shot_and_player_data nearest_defender_in_moment(cleaned_moment m, shot s):
    cdef shot_and_player_data nearest
    nearest.offense_team_id = s.team_id
    nearest.offense_player_id = s.player_id
    nearest.defender_distance = 100000.0
    nearest.game_clock = s.shot_time
    nearest.shot_clock = m.shot_clock
    nearest.quarter = m.quarter
    nearest.game_id = m.game_id
    nearest.event_id = s.game_event_id
    nearest.shot_attempted = s.shot_attempted_flag
    nearest.shot_made = s.shot_made_flag

    cdef player_data shooter_data = m.players[0]
    not_assigned = True
    for p in m.players:
        if p.player_id == s.player_id:
            shooter_data = p
            not_assigned = False
            break
    if not_assigned:
        nearest.defense_player_id = -1
        return nearest

    nearest.offense_x_loc = shooter_data.x_loc
    nearest.offense_y_loc = shooter_data.y_loc

    cdef double new_distance
    for p in m.players:
        new_distance = compute_distance(&shooter_data, &p)
        if new_distance < nearest.defender_distance and nearest.offense_team_id != p.team_id:
            nearest.defender_distance = new_distance
            nearest.defense_team_id = p.team_id
            nearest.defense_player_id = p.player_id
            nearest.defense_x_loc = p.x_loc
            nearest.defense_y_loc = p.y_loc
    return nearest

cdef double compute_distance(player_data* p1, player_data* p2):
    return hypot(p1.x_loc-p2.x_loc, p1.y_loc-p2.y_loc)

 */