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
    for i in range(moment_data.size() - t_index_offset):
        trajectory_match(moment_data.at(i), moment_data.at(i+t_index_offset),
                         moment_data.at(i).ball,
                         moment_data.at(i+t_index_offset).ball,
                         x_offset, y_offset, result)
        for j in range(10):
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
