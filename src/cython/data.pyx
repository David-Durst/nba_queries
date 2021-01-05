# distutils: language = c++
from libcpp.vector cimport vector
from libcpp cimport bool
from math import floor

cpdef player_data parse_player_data(list data):
    return player_data(int(data[0]),
                       int(data[1]),
                       float(data[2]),
                       float(data[3]),
                       float(data[4]))

cpdef event_moment_data parse_event_moment_data(str data):
    # remove the braces
    sp_str = data[1:-1].split(":")
    return event_moment_data(int(sp_str[0]),
                             int(sp_str[1]))

cpdef clock_fixed_point parse_clock_fixed_point_data(str data):
    f = float(data)

    seconds = floor(f)
    twenty_fifths_of_second = round((f - seconds) * 25)
    if twenty_fifths_of_second == 25:
        seconds += 1
        twenty_fifths_of_second = 0
    return clock_fixed_point(seconds,
                             twenty_fifths_of_second)

cpdef cleaned_moment parse_cleaned_moment_data(str data):
    sp_str = data.split(",")
    ball = parse_player_data(sp_str[0:5])
    cdef vector[player_data] players
    for i in range(10):
        players.push_back(parse_player_data(sp_str[(i+1)*5:(i+2)*5]))
    cdef vector[event_moment_data] events
    # drop the last element as it's going to be nothing
    for e_str in sp_str[59].split(";")[:-1]:
        events.push_back(parse_event_moment_data(e_str))
    return cleaned_moment(ball, players,
                          parse_clock_fixed_point_data(sp_str[55]),
                          float(sp_str[56]),
                          int(sp_str[57]),
                          int(sp_str[58]),
                          events)

cdef int int_def(str data):
    if len(data) == 0:
        return -1
    else:
        return int(data)


cpdef shot parse_shot_data(str data):
    sp_str = data.split(",")
    return shot(
        #char* action_type;
        str.encode(sp_str[0]),
        #int event_time;
        int_def(sp_str[1]),
        #char* event_type;
        str.encode(sp_str[2]),
        #char* game_date;
        str.encode(sp_str[3]),
        #long int game_event_id;
        int_def(sp_str[4]),
        #long int game_id;
        int_def(sp_str[5]),
        #char* grid_type;
        str.encode(sp_str[6]),
        #char* htm;
        str.encode(sp_str[7]),
        #double loc_x;
        float(sp_str[8]),
        #double loc_y;
        float(sp_str[9]),
        #int minutes_remaining;
        int_def(sp_str[10]),
        #int period;
        int_def(sp_str[11]),
        #int player_id;
        int_def(sp_str[12]),
        #char* player_name;
        str.encode(sp_str[13]),
        #double quarter;
        float(sp_str[14]),
        #int seconds_remaining;
        int_def(sp_str[15]),
        #bool shot_attempted_flag;
        int_def(sp_str[16]) != 0,
        #int shot_distance;
        int_def(sp_str[17]),
        #bool shot_made_flag;
        int_def(sp_str[18]) != 0,
        #double shot_time;
        float(sp_str[19]),
        #char* shot_type;
        str.encode(sp_str[20]),
        #char* shot_zone_area;
        str.encode(sp_str[21]),
        #char* shot_zone_basic;
        str.encode(sp_str[22]),
        #char* shot_zone_range;
        str.encode(sp_str[23]),
        #double team_id;
        float(sp_str[24]),
        #char* team_name;
        str.encode(sp_str[25]),
        #char* team_vtm;
        str.encode(sp_str[26])
    )
