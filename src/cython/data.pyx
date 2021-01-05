# distutils: language = c++
from libcpp.vector cimport vector
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
