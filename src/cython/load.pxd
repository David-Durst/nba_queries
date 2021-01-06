# distutils: language = c++
from data cimport *
from libcpp.vector cimport vector

cdef vector[cleaned_moment] moment_data
cdef vector[shot] shot_data
cdef vector[shot] selected_shot_data

cpdef void parse_file(str moment_file, str shot_file)
cpdef void parse_just_shots_file(str shot_file)
cpdef void get_shots_for_game(long int game_id)
cpdef vector[cleaned_moment] take_moment(int n)
cpdef cleaned_moment get_moment(int n)
cpdef vector[shot] take_shot(int n, int op)
cpdef shot get_shot(int n, int op)
cpdef size_t shot_size(int op)
