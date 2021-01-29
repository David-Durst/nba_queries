#ifndef CLEAN_QUERIES_H
#define CLEAN_QUERIES_H

#include <map>
#include <memory>
#include <functional>
#include <string>
#include <vector>
#include <algorithm>
#include <list>
#include <iostream>
#include "query_structs.h"
#include "find_trajectories.h"
#include "col_stores.h"
#include "check_distances.h"
#define NUM_BINS MAX_X*MAX_Y
#define NUM_BINS_WITH_TIME MAX_X*MAX_Y*72

// query 1
void find_nearest_defender_at_each_shot_clean(moment_col_store * moments,
                                              shot_col_store * shots,
                                              list<shot_and_player_data> * shots_and_players,
                                              vector<extra_game_data>& extra_data,
                                              int time_delta_ticks, bool parallel);

// query 3
void find_trajectories_fixed_origin_clean(moment_col_store * moments, vector<trajectory_data>& trajectories,
                                          coordinate_range origin, coordinate_range destination,
                                          int t_offset, int t_delta_ticks, bool parallel);

void find_trajectories_fixed_origin_clean_just_outer(moment_col_store * moments, vector<trajectory_data>& trajectories,
                                          coordinate_range origin, coordinate_range destination,
                                          int t_offset, int t_delta_ticks, bool parallel);


void find_trajectories_fixed_origin_clean_rowstore(vector<cleaned_moment>& moments, vector<trajectory_data>& trajectories,
                                               coordinate_range origin, coordinate_range destination,
                                               int t_offset, int t_delta_ticks);

class court_bins;
void find_trajectories_fixed_origin_clean_binned(moment_col_store * moments, court_bins * moment_bins,
                                                 vector<trajectory_data>& trajectories, coordinate_range origin,
                                                 coordinate_range destination, int t_offset, int t_delta_ticks, bool parallel);

void find_trajectories_fixed_origin_clean_binned_just_outer(moment_col_store * moments, court_bins * moment_bins,
                                                 vector<trajectory_data>& trajectories, coordinate_range origin,
                                                 coordinate_range destination, int t_offset, int t_delta_ticks, bool parallel);

void find_trajectories_fixed_origin_clean_binned_min_time(moment_col_store * moments, court_bins * moment_bins,
                                                            vector<trajectory_data>& trajectories, coordinate_range origin,
                                                            coordinate_range destination, int t_offset, int t_delta_ticks, bool parallel);
// query 12
void is_paul_george_near_ball_in_paint_clean(moment_col_store * moments, court_bins * moment_bins, vector<shot_and_player_data>& paul_and_ball,
                                             coordinate_range paint0, coordinate_range paint1, long int player_id);

// query 13
struct ball_height_at_time {
    clock_fixed_point game_clock;
    double height;
};
void get_end_game_moments(moment_col_store * moments, vector<extra_game_data>& extra_data,
                          vector<ball_height_at_time>& ball_heights, int last_n_seconds);

// query 14
struct players_in_paint_at_time {
    int64_t moment_index;
    clock_fixed_point game_clock;
    long int player_id;
};
void get_players_in_paint_at_end(moment_col_store * moments, vector<extra_game_data>& extra_data,
                                 vector<players_in_paint_at_time>& players_in_paint,
                                 coordinate_range paint0, coordinate_range paint1, int last_n_seconds);

void get_players_in_paint_at_end_binned(moment_col_store * moments, court_bins * moment_bins, vector<extra_game_data>& extra_data,
                                 vector<players_in_paint_at_time>& players_in_paint,
                                 coordinate_range paint0, coordinate_range paint1, int last_n_seconds);

class court_and_game_clock_bins;
void get_players_in_paint_at_end_binned_with_time(moment_col_store * moments, court_and_game_clock_bins * moment_bins, vector<extra_game_data>& extra_data,
                                                  vector<players_in_paint_at_time>& players_in_paint,
                                                  coordinate_range paint0, coordinate_range paint1, int last_n_seconds);

void get_players_in_paint_at_end_binned_with_time_fix_par(moment_col_store * moments, court_and_game_clock_bins * moment_bins, vector<extra_game_data>& extra_data,
                                                  vector<players_in_paint_at_time>& players_in_paint,
                                                  coordinate_range paint0, coordinate_range paint1, int last_n_seconds);

// query 15
void get_players_in_paint_shot_clock(moment_col_store * moments, vector<players_in_paint_at_time>& players_in_paint,
                                     coordinate_range paint0, coordinate_range paint1, double end_time);

void get_players_in_paint_shot_clock_one_paint(moment_col_store * moments, vector<players_in_paint_at_time>& players_in_paint,
                                     coordinate_range paint0, double end_time);

void get_players_in_paint_shot_clock_no_funcs(moment_col_store * moments, vector<players_in_paint_at_time>& players_in_paint,
                                               coordinate_range paint0, coordinate_range paint1, double end_time);

void get_players_in_paint_shot_clock_no_ptr_funcs(moment_col_store * moments, vector<players_in_paint_at_time>& players_in_paint,
                                              coordinate_range paint0, coordinate_range paint1, double end_time);

void get_players_in_paint_shot_clock_one_paint_no_vec(moment_col_store * moments, vector<players_in_paint_at_time>& players_in_paint,
                                               coordinate_range paint0, double end_time);

void get_players_in_paint_shot_clock_binned(moment_col_store * moments, court_bins * moment_bins,
                                            vector<players_in_paint_at_time>& players_in_paint,
                                            coordinate_range paint0, coordinate_range paint1, double end_time);

static inline __attribute__((always_inline)) bool point_intersect_no_time(coordinate_range * r, double x_loc, double y_loc) {
    bool x_intersects = x_loc >= r->start.x && x_loc <= r->end.x;
    bool y_intersects = y_loc >= r->start.y && y_loc <= r->end.y;
    return x_intersects && y_intersects;
}

static inline __attribute__((always_inline)) bool point_intersect_no_time_no_ptr(coordinate_range r, double x_loc, double y_loc) {
    bool x_intersects = x_loc >= r.start.x && x_loc <= r.end.x;
    bool y_intersects = y_loc >= r.start.y && y_loc <= r.end.y;
    return x_intersects && y_intersects;
}

static inline __attribute__((always_inline)) bool point_intersect_no_time(const coordinate_range& r, const player_data& c) {
    bool x_intersects = c.x_loc >= r.start.x && c.x_loc <= r.end.x;
    bool y_intersects = c.y_loc >= r.start.y && c.y_loc <= r.end.y;
    return x_intersects && y_intersects;
}


struct player_pointer {
    int64_t moment_index;
    int player_index;
    double x_loc;
    double y_loc;
};

struct player_pointer_and_id {
    player_pointer ptr;
    long int player_id;
};

class court_bins {
public:
    court_bins(moment_col_store * moments);
    constexpr static double x_bin_size = 10.0;
    constexpr static double y_bin_size = 10.0;

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
        return floor(y / y_bin_size)*MAX_X + floor(x / x_bin_size);
    }

    static inline std::vector<int> get_bins_in_region(const coordinate_range& r) {
        std::vector<int> result;
        for (int x = floor(r.start.x); x < ceil(r.end.x); x += x_bin_size) {
            for (int y = floor(r.start.y); y < ceil(r.end.y); y += y_bin_size) {
                result.push_back(get_bin_index(x, y));
            }
        }
        return result;
    }

    inline __attribute__((always_inline)) const player_pointer* bin_start(long int player_id, int bin_index) {
        int player_bin_index = players_indices_in_bins.at(player_id);
        int offset = bin_starts[player_bin_index][bin_index];
        return &player_moment_bins[offset];
    }
    inline __attribute__((always_inline))  const player_pointer* bin_end(long int player_id, int bin_index) {
        int player_bin_index = players_indices_in_bins.at(player_id);
        if (bin_index == NUM_BINS) {
            player_bin_index++;
            bin_index = 0;
        }
        int offset = (player_bin_index == players_indices_in_bins.size()) ?
                     num_player_moments : bin_starts[player_bin_index][bin_index + 1];
        return &player_moment_bins[offset];
    }

    int64_t get_elems_in_region(const coordinate_range& r) {
        const std::vector<int>& bins = get_bins_in_region(r);
        int64_t num_bins = 0;
        for (int player_num = 0; player_num < this->players_indices_in_bins.size(); player_num++) {
            long int player_id = this->player_ids[player_num];
            // all trajectory starts for the current player
            for (const auto &src_bin : bins) {
                num_bins += this->bin_end(player_id, src_bin) - this->bin_start(player_id, src_bin);
            }
        }
        return num_bins;
    }


    // map from player id in moments data set to the bin index for that player
    std::map<long int, int> players_indices_in_bins;
    long int * player_ids;
    // map from player bin index and then bin index to a location in bin_list_indices
    int64_t (* bin_starts)[NUM_BINS];
    // all the moments for each player organized by bin
    player_pointer * player_moment_bins;
    // number of elements in bin_list_indices
    int64_t num_player_moments;
};

static inline __attribute__((always_inline)) const player_pointer* get_bin_start(court_bins * bins, long int player_id, int bin_index) {
    int player_bin_index = bins->players_indices_in_bins.at(player_id);
    int offset = bins->bin_starts[player_bin_index][bin_index];
    return &bins->player_moment_bins[offset];
}
static inline __attribute__((always_inline))  const player_pointer* get_bin_end(court_bins * bins, long int player_id, int bin_index) {
    int player_bin_index = bins->players_indices_in_bins.at(player_id);
    if (bin_index == NUM_BINS) {
        player_bin_index++;
        bin_index = 0;
    }
    int offset = (player_bin_index == bins->players_indices_in_bins.size()) ?
                 bins->num_player_moments : bins->bin_starts[player_bin_index][bin_index + 1];
    return &bins->player_moment_bins[offset];
}

class court_and_game_clock_bins {
public:
    court_and_game_clock_bins(moment_col_store * moments);

    constexpr static double x_bin_size = 10.0;
    constexpr static double y_bin_size = 10.0;
    constexpr static double t_bin_size = 10.0;

    static inline int get_bin_index(double x, double y, double t) {
        if (t < 0) {
            t = 0;
        }
        if (t >= MAX_GAME_CLOCK) {
            t = MAX_GAME_CLOCK - 0.001;
        }
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
        return floor(y / y_bin_size)*MAX_X*72 + floor(x / x_bin_size)*72 + floor(t / t_bin_size);
    }

    static inline std::vector<int> get_bins_in_region(const coordinate_range& r) {
        std::vector<int> result;
        for (int x = floor(r.start.x); x < ceil(r.end.x); x += x_bin_size) {
            for (int y = floor(r.start.y); y < ceil(r.end.y); y += y_bin_size) {
                for (int t = floor(r.start.game_clock); t < ceil(r.end.game_clock); t += t_bin_size) {
                    result.push_back(get_bin_index(x, y, t));
                }
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
        if (bin_index == NUM_BINS_WITH_TIME) {
            player_bin_index++;
            bin_index = 0;
        }
        int offset = (player_bin_index == players_indices_in_bins.size()) ?
                     num_player_moments : bin_starts[player_bin_index][bin_index + 1];
        return &player_moment_bins[offset];
    }

    int64_t get_elems_in_region(const coordinate_range& r) {
        const std::vector<int>& bins = get_bins_in_region(r);
        int64_t num_bins = 0;
        for (int player_num = 0; player_num < this->players_indices_in_bins.size(); player_num++) {
            long int player_id = this->player_ids[player_num];
            // all trajectory starts for the current player
            for (const auto &src_bin : bins) {
                num_bins += this->bin_end(player_id, src_bin) - this->bin_start(player_id, src_bin);
            }
        }
        return num_bins;
    }


    // map from player id in moments data set to the bin index for that player
    std::map<long int, int> players_indices_in_bins;
    long int * player_ids;
    // map from player bin index and then bin index to a location in bin_list_indices
    int64_t (* bin_starts)[NUM_BINS_WITH_TIME];
    // all the moments for each player organized by bin
    player_pointer * player_moment_bins;
    // number of elements in bin_list_indices
    int64_t num_player_moments;
    double seconds_tracked;
};

#include <omp.h>
double * temp_time;

#endif //CLEAN_QUERIES_H
