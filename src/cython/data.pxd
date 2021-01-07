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

cpdef clock_fixed_point add_ticks(clock_fixed_point c, int ticks)
cpdef clock_fixed_point sub_ticks(clock_fixed_point c, int ticks)

cpdef size_t time_to_index(clock_fixed_point c, int quarter)

cdef struct cleaned_moment:
    player_data ball
    vector[player_data] players
    clock_fixed_point game_clock
    double shot_clock
    short int quarter
    long int game_id
    vector[event_moment_data] events

cdef vector[player_data] get_all_player_data(cleaned_moment& c)
cpdef shot parse_shot_data(str data)

cdef struct shot:
    string action_type
    int event_time
    string event_type
    string game_date
    long int game_event_id
    long int game_id
    string grid_type
    string htm
    double loc_x
    double loc_y
    int minutes_remaining
    int period
    int player_id
    string player_name
    double quarter
    int seconds_remaining
    bool shot_attempted_flag
    int shot_distance
    bool shot_made_flag
    clock_fixed_point shot_time
    string shot_type
    string shot_zone_area
    string shot_zone_basic
    string shot_zone_range
    long int team_id
    string team_name
    string team_vtm
