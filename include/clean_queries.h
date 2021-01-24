#ifndef CLEAN_QUERIES_H
#define CLEAN_QUERIES_H

#include <map>
#include <memory>
#include <functional>
#include <string>
#include <vector>
#include <algorithm>
#include <list>
#include "query_structs.h"
#include "find_trajectories.h"
#include "col_stores.h"
#include "check_distances.h"
#define NUM_BINS MAX_X*MAX_Y

// query 1
void find_nearest_defender_at_each_shot_clean(moment_col_store * moments,
                                              shot_col_store * shots,
                                              list<shot_and_player_data> * shots_and_players,
                                              vector<extra_game_data>& extra_data,
                                              int time_delta_ticks, bool parallel);

// query 3
void find_trajectories_fixed_origin_clean(moment_col_store * moments, list<trajectory_data> * trajectories,
                                          coordinate_range origin, coordinate_range destination,
                                          int t_offset, int t_delta_ticks, bool parallel);


void find_trajectories_fixed_origin_clean_rowstore(vector<cleaned_moment>& moments, vector<trajectory_data>& trajectories,
                                               coordinate_range origin, coordinate_range destination,
                                               int t_offset, int t_delta_ticks);

class court_bins;
void find_trajectories_fixed_origin_clean_binned(moment_col_store * moments, court_bins * moment_bins,
                                                 list<trajectory_data> * trajectories, coordinate_range origin,
                                                 coordinate_range destination, int t_offset, int t_delta_ticks, bool parallel);

void find_trajectories_fixed_origin_clean_binned_part(moment_col_store * moments, court_bins * moment_bins,
                                                 list<trajectory_data> * trajectories, coordinate_range origin,
                                                 coordinate_range destination, int t_offset, int t_delta_ticks, bool parallel);
void find_trajectories_fixed_origin_clean_binned_part_par(moment_col_store * moments, court_bins * moment_bins,
                                                      list<trajectory_data> * trajectories, coordinate_range origin,
                                                      coordinate_range destination, int t_offset, int t_delta_ticks, bool parallel);

inline bool point_intersect_no_time(coordinate_range * r, double x_loc, double y_loc) {
    bool x_intersects = x_loc >= r->start.x && x_loc <= r->end.x;
    bool y_intersects = y_loc >= r->start.y && y_loc <= r->end.y;
    return x_intersects && y_intersects;
}

inline bool point_intersect_no_time(const coordinate_range& r, const player_data& c) {
    bool x_intersects = c.x_loc >= r.start.x && c.x_loc <= r.end.x;
    bool y_intersects = c.y_loc >= r.start.y && c.y_loc <= r.end.y;
    return x_intersects && y_intersects;
}

struct player_pointer {
    int64_t moment_index;
    int player_index;
};

struct player_pointer_and_id {
    int64_t moment_index;
    int player_index;
    long int player_id;
};

class court_bins {
public:
    court_bins(moment_col_store * moments);

    static inline int get_bin_index(double x, double y) {
        if (x < 0) {
            x = 0;
        }
        if (x >= MAX_X) {
            x = MAX_X-0.001;
        }
        if (y < 0) {
            y = 0;
        }
        if (y >= MAX_Y) {
            y = MAX_Y-0.001;
        }
        return floor(y)*MAX_X + floor(x);
    }

    static inline std::list<int> get_bins_in_region(const coordinate_range& r) {
        std::list<int> result;
        for (int x = floor(r.start.x); x < ceil(r.end.x); x++) {
            for (int y = floor(r.start.y); y < ceil(r.end.y); y++) {
                result.push_back(get_bin_index(x, y));
            }
        }
        return result;
    }

    inline const player_pointer* bin_start(long int player_id, int bin_index) {
        int player_bin_index = players_indices_in_bins.at(player_id);
        int offset = bin_starts[player_bin_index][bin_index];
        return &player_moment_bins[offset];
    }
    inline const player_pointer* bin_end(long int player_id, int bin_index) {
        int player_bin_index = players_indices_in_bins.at(player_id);
        if (bin_index == NUM_BINS) {
            player_bin_index++;
            bin_index = 0;
        }
        int offset = (player_bin_index == players_indices_in_bins.size()) ?
                     num_player_moments : bin_starts[player_bin_index][bin_index + 1];
        return &player_moment_bins[offset];
    }


    // map from player index in moments data set to the bin index for that player
    std::map<long int, int> players_indices_in_bins;
    long int * player_ids;
    // map from player bin index and then bin index to a location in bin_list_indices
    int64_t (* bin_starts)[NUM_BINS];
    // all the moments for each player organized by bin
    player_pointer * player_moment_bins;
    // number of elements in bin_list_indices
    int64_t num_player_moments;
};

#endif //CLEAN_QUERIES_H
