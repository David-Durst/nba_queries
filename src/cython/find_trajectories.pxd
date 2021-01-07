# distutils: language = c++
from data cimport *
from load cimport *
from libcpp.vector cimport vector
from libcpp cimport bool

cpdef vector[trajectory_data] find_trajectories_no_fixed_origin(double x_offset,
                                                                double y_offset,
                                                                int t_offset)

cpdef vector[trajectory_data] find_trajectories_fixed_origin(coordinate_range start,
                                                             coordinate_range end,
                                                             int t_offset,
                                                             int t_delta_ticks)
cdef struct trajectory_data:
    long int team_id
    int player_id
    double start_x_loc
    double start_y_loc
    clock_fixed_point start_game_clock
    double end_x_loc
    double end_y_loc
    clock_fixed_point end_game_clock
    int quarter

cdef struct coordinate:
    double x_loc
    double y_loc

cdef struct coordinate_range:
    coordinate start
    coordinate end
