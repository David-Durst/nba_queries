# distutils: language = c++
from libcpp.vector cimport vector

cpdef read_file(str filename):
    res = []

    with open(filename, "r") as f:
        for line in f.readlines():
            res.append(res)

    return res

cpdef player_data parse_player_data(str data):
    return player_data(1,2,2.0,2.0,2.0)
    
