# distutils: language = c++
from data cimport *
from libcpp.vector cimport vector

cdef vector[cleaned_moment] moment_data
cdef vector[shot] shot_data
cdef vector[shot] selected_shot_data

cpdef void get_shots_for_game(long int game_id):
    for i in range(shot_data.size()):
        if shot_data.at(i).game_id == game_id:
            selected_shot_data.push_back(shot_data.at(i))

cpdef void parse_file(str moment_file, str shot_file):
    i = 0
    with open(moment_file, "r") as f:
        for line in f.readlines():
            if i == 0:
                i += 1
                continue
            moment_data.push_back(parse_cleaned_moment_data(line))
    i = 0
    with open(shot_file, "r") as f:
        for line in f.readlines():
            if i == 0:
                i += 1
                continue
            shot_data.push_back(parse_shot_data(line))

cpdef vector[cleaned_moment] take_moment(int n):
    cpdef vector[cleaned_moment] res
    for i in range(n):
        res.push_back(moment_data.at(i))
    return res


cpdef cleaned_moment get_moment(int n):
    return moment_data.at(n)

cpdef vector[shot] take_shot(int n, int op):
    cpdef vector[shot] res
    for i in range(n):
        if op == 0:
            res.push_back(shot_data.at(i))
        else:
            res.push_back(selected_shot_data.at(i))
    return res


cpdef shot get_shot(int n, int op):
    if op == 0:
        return shot_data.at(n)
    else:
        return selected_shot_data.at(n)

cpdef size_t shot_size(int op):
    if op == 0:
        return shot_data.size()
    else:
        return selected_shot_data.size()
