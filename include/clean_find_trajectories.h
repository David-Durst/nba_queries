#ifndef CLEAN_FIND_TRAJECTORIES_H
#define CLEAN_FIND_TRAJECTORIES_H

#include <memory>
#include <functional>
#include <string>
#include <vector>
#include "query_structs.h"
#include "find_trajectories.h"
#include "col_stores.h"

void find_trajectories_fixed_origin_clean(moment_col_store * moments, list<trajectory_data> * trajectories,
                                          coordinate_range origin, coordinate_range destination,
                                          int t_offset, int t_delta_ticks);

bool point_intersect_no_time(coordinate_range * r, double x_loc, double y_loc);

#endif //NBA_QUERIES_CLEAN_FIND_TRAJECTORIES_H
