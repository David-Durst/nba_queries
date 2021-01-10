#ifndef QUERY3_CLEAN_FIND_TRAJECTORIES_FIXED_ORIGIN_H
#define QUERY3_CLEAN_FIND_TRAJECTORIES_FIXED_ORIGIN_H

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


void find_trajectories_fixed_origin_clean_rowstore(vector<cleaned_moment>& moments, vector<trajectory_data>& trajectories,
                                               coordinate_range origin, coordinate_range destination,
                                               int t_offset, int t_delta_ticks);

#endif //QUERY3_CLEAN_FIND_TRAJECTORIES_FIXED_ORIGIN_H
