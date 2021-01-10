# distutils: language = c++
from libcpp.vector cimport vector
from libcpp cimport bool
from data cimport *

cdef struct shot_and_player_data:
    long int offense_team_id
    int offense_player_id
    double offense_x_loc
    double offense_y_loc
    long int defense_team_id
    int defense_player_id
    double defense_x_loc
    double defense_y_loc
    double defender_distance
    clock_fixed_point game_clock
    double shot_clock
    short int quarter
    long int game_id
    long int event_id
    bool shot_attempted
    bool shot_made
