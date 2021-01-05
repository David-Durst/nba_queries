# distutils: language = c++
from data cimport *
from libcpp.vector cimport vector

cdef vector[cleaned_moment] data

cpdef void parse_file(str filename):
    i = 0
    with open(filename, "r") as f:
        for line in f.readlines():
            if i == 0:
                i += 1
                continue
            data.push_back(parse_cleaned_moment_data(line))

cpdef vector[cleaned_moment] take(int n):
    cpdef vector[cleaned_moment] res
    for i in range(n):
        res.push_back(data.at(i))
    return res


cpdef cleaned_moment get(int n):
    return data.at(n)
