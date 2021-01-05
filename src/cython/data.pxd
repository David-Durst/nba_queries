# distutils: language = c++
from libcpp.vector cimport vector

cpdef cleaned_moment parse_cleaned_moment_data(str data)

cdef struct player_data:
    long int team_id
    int player_id
    double x_loc
    double y_loc
    double radius

cdef struct event_moment_data:
    long int event_id
    int moment_in_event


cdef struct clock_fixed_point:
    long int seconds
    int twenty_fifths_of_second

cdef struct cleaned_moment:
    player_data ball
    vector[player_data] players
    clock_fixed_point game_clock
    double shot_clock
    short int quarter
    long int game_id
    vector[event_moment_data] events
