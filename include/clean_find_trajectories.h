#ifndef CLEAN_FIND_TRAJECTORIES_H
#define CLEAN_FIND_TRAJECTORIES_H

#include <memory>
#include <functional>
#include <string>
#include <vector>
#include "query_structs.h"
#include "find_trajectories.h"

void find_trajectories_fixed_origin_clean(vector<cleaned_moment>& moments, vector<trajectory_data>& trajectories,
                                          coordinate_range origin, coordinate_range destination,
                                          int t_offset, int t_delta_ticks);

bool point_intersect_no_time(const coordinate_range& r, const player_data& c);

#endif //NBA_QUERIES_CLEAN_FIND_TRAJECTORIES_H