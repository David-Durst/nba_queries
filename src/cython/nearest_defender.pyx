# distutils: language = c++
from data cimport *
from load cimport *
from libcpp.vector cimport vector
from libcpp cimport bool
from libc.math cimport hypot
from math import ceil, floor

cpdef bucket_distances(vector[shot_and_player_data] data):
    cdef double max_distance_f
    max_distance_f = data.at(0).defender_distance
    for d in data:
        if max_distance_f < d.defender_distance:
            max_distance_f = d.defender_distance
    cdef int max_distance = ceil(max_distance_f)

    result = []
    for i in range(max_distance + 1):
        result.append(0)

    print("result length: " + str(len(result)))
    print("max_distance_f: " + str(max_distance_f))

    for d in data:
        result[floor(d.defender_distance)] += 1

    return result

cpdef vector[shot_and_player_data] find_nearest_defender_at_each_shot(int tick_delta = 50):
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

cpdef size_t time_to_index(clock_fixed_point c, int quarter):
    # 720 seconds in a quarter
    return 25 * (720 * (quarter - 1) + 720 - c.seconds) - c.twenty_fifths_of_second

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
