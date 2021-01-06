# distutils: language = c++
from libcpp.vector cimport vector
from libcpp.string cimport string
from libcpp cimport bool

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

cpdef shot parse_shot_data(str data)

cdef struct shot:
    string action_type
    int event_time
    char* event_type
    char* game_date
    long int game_event_id
    long int game_id
    char* grid_type
    char* htm
    double loc_x
    double loc_y
    int minutes_remaining
    int period
    int player_id
    char* player_name
    double quarter
    int seconds_remaining
    bool shot_attempted_flag
    int shot_distance
    bool shot_made_flag
    clock_fixed_point shot_time
    char* shot_type
    char* shot_zone_area
    char* shot_zone_basic
    char* shot_zone_range
    long int team_id
    char* team_name
    char* team_vtm
