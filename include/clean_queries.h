#ifndef CLEAN_QUERIES_H
#define CLEAN_QUERIES_H

#include <map>
#include <memory>
#include <functional>
#include <string>
#include <vector>
#include <algorithm>
#include "query_structs.h"
#include "find_trajectories.h"
#include "col_stores.h"
#include "check_distances.h"
#define NUM_BINS MAX_X*MAX_Y

// query 1
void find_nearest_defender_at_each_shot_clean(moment_col_store * moments,
                                              shot_col_store * shots,
                                              list<shot_and_player_data> * shots_and_players,
                                              int time_delta_ticks);

class court_bins {
public:

    court_bins(moment_col_store * moments);

    static inline int get_bin_index(double x, double y);
    inline const int64_t* bin_start(long );
    inline const int64_t* bin_end();


    // map from player index in moments data set to the bin index for that player
    std::map<long int, int> players_indices_in_bins;
    // map from player bin index and then bin index to a location in bin_list_indices
    int64_t (* bin_list_starts)[NUM_BINS];
    // all the moments for each player organized by bin
    int64_t * bin_list_indices;
};

void find_nearest_defender_at_each_shot_clean_binned(moment_col_store * moments,
                                                     shot_col_store * shots,
                                                     court_bins * bins,
                                                     list<shot_and_player_data> * shots_and_players,
                                                     int time_delta_ticks);

// query 3
void find_trajectories_fixed_origin_clean(moment_col_store * moments, list<trajectory_data> * trajectories,
                                          coordinate_range origin, coordinate_range destination,
                                          int t_offset, int t_delta_ticks);


void find_trajectories_fixed_origin_clean_rowstore(vector<cleaned_moment>& moments, vector<trajectory_data>& trajectories,
                                               coordinate_range origin, coordinate_range destination,
                                               int t_offset, int t_delta_ticks);

#endif //CLEAN_QUERIES_H
