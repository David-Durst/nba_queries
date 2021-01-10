#ifndef CLEAN_QUERIES_H
#define CLEAN_QUERIES_H

#include <memory>
#include <functional>
#include <string>
#include <vector>
#include "query_structs.h"
#include "find_trajectories.h"
#include "col_stores.h"

// query 1
void find_nearest_defender_at_each_shot_clean(moment_col_store * moments,
                                              shot_col_store * shots,
                                              list<shot_and_player_data>& shots_and_players,
                                              float time_delta);

// query 3
void find_trajectories_fixed_origin_clean(moment_col_store * moments, list<trajectory_data> * trajectories,
                                          coordinate_range origin, coordinate_range destination,
                                          int t_offset, int t_delta_ticks);


void find_trajectories_fixed_origin_clean_rowstore(vector<cleaned_moment>& moments, vector<trajectory_data>& trajectories,
                                               coordinate_range origin, coordinate_range destination,
                                               int t_offset, int t_delta_ticks);

#endif //CLEAN_QUERIES_H
