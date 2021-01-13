# distutils: language = c++
from data cimport *
from load cimport *
from libcpp.vector cimport vector
from libcpp cimport bool

cpdef vector[trajectory_data] find_trajectories_no_fixed_origin(double x_offset,
                                                                double y_offset,
                                                                int t_offset):
    cdef vector[trajectory_data] result
    cdef int t_index_offset = t_offset * 25
    cdef cleaned_moment src, dst
    for i in range(moment_data.size() - t_index_offset):
        src = moment_data.at(i)
        dst = moment_data.at(i+t_index_offset)
        trajectory_match(src, dst, src.ball, dst.ball,
                         x_offset, y_offset, result)
        # j is for players in src, k is for players in dst
        for j in range(10):
            for k in range(10):
                if src.players[j].player_id == dst.players[k].player_id:
                    trajectory_match(moment_data.at(i), moment_data.at(i+t_index_offset),
                                    moment_data.at(i).players[j],
                                    moment_data.at(i+t_index_offset).players[j],
                                    x_offset, y_offset, result)
    return result


space_delta = 0.1

cdef void trajectory_match(cleaned_moment& src_m, cleaned_moment& dst_m,
                           player_data& src_p, player_data& dst_p,
                           double x_offset, double y_offset,
                           vector[trajectory_data]& result):
    if abs(dst_p.x_loc + x_offset - src_p.x_loc) < space_delta and \
        abs(dst_p.y_loc + y_offset - src_p.y_loc) < space_delta:
        result.push_back(trajectory_data(src_p.team_id,
                                         src_p.player_id,
                                         src_p.x_loc,
                                         src_p.y_loc,
                                         src_m.game_clock,
                                         dst_p.x_loc,
                                         dst_p.y_loc,
                                         src_m.game_clock,
                                         src_m.quarter))

cpdef vector[trajectory_data] find_trajectories_fixed_origin(coordinate_range start,
                                                             coordinate_range end,
                                                             int t_offset,
                                                             int t_delta_ticks):
    cdef vector[trajectory_data] result
    cdef int t_index_offset = t_offset * 25
    cdef cleaned_moment src_m, dst_m
    cdef vector[player_data] src_players_unfiltered, src_players, dst_players
    for i in range(moment_data.size() - t_index_offset + t_delta_ticks):
        src_m = moment_data.at(i)
        src_players_unfiltered = get_all_player_data(src_m)
        src_players.clear()
        # check all future players that match the 
        for p in src_players_unfiltered:
            if point_intersect(start, p):
                src_players.push_back(p)
        if src_players.empty():
            continue
        for j in range(t_index_offset - t_delta_ticks, t_index_offset + t_delta_ticks + 1):
            if i + j >= moment_data.size():
                continue
            dst_m = moment_data.at(i+j)
            for p in src_players:
                dst_players = get_all_player_data(dst_m)
                for q in dst_players:
                    if p.player_id == q.player_id and point_intersect(end, q):
                        result.push_back(
                            trajectory_data(
                                p.team_id,
                                p.player_id,
                                p.x_loc,
                                p.y_loc,
                                src_m.game_clock,
                                q.x_loc,
                                q.y_loc,
                                dst_m.game_clock,
                                src_m.quarter
                        ))
                        continue
    return result


cdef bool point_intersect(coordinate_range& r, player_data& c):
    cdef bool x_intersects = c.x_loc >= r.start.x_loc and c.x_loc <= r.end.x_loc
    cdef bool y_intersects = c.y_loc >= r.start.y_loc and c.y_loc <= r.end.y_loc
    return x_intersects and y_intersects;
