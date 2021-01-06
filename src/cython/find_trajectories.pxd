# distutils: language = c++
from data cimport *
from load cimport *
from libcpp.vector cimport vector
from libcpp cimport bool

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
