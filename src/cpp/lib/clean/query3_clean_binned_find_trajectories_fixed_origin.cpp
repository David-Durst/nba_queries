#include "clean_queries.h"
#include "benchmark.h"
#include <set>
#include <utility>
#include <iostream>
#include <omp.h>

void find_trajectories_fixed_origin_clean_binned(moment_col_store * moments, court_bins * moment_bins,
                                                 vector<trajectory_data>& trajectories, coordinate_range origin,
                                                 coordinate_range destination, int t_offset, int t_delta_ticks,
                                                 bool parallel) {
    const std::vector<int>& origin_bins = court_bins::get_bins_in_region(origin);
    int t_index_offset = t_offset * 25;
    std::vector<player_pointer_and_id> src_moments;
    int num_threads = omp_get_max_threads();
    vector<player_pointer_and_id> temp_srcs[num_threads];

    #pragma omp parallel for if(parallel)
    for (int player_num = 0; player_num < moment_bins->players_indices_in_bins.size(); player_num++) {
        int thread_num = omp_get_thread_num();
        long int player_id = moment_bins->player_ids[player_num];
        // all trajectory starts for the current player
        for (const auto &src_bin : origin_bins) {
            for (const player_pointer *src_moment = moment_bins->bin_start(player_id, src_bin);
                 src_moment != moment_bins->bin_end(player_id, src_bin); src_moment++) {
                if (src_moment->x_loc >= origin.start.x && src_moment->x_loc <= origin.end.x &&
                    src_moment->y_loc >= origin.start.y && src_moment->y_loc <= origin.end.y) {
                    temp_srcs[thread_num].push_back({*src_moment, player_id});
                }
            }
        }
    }
    for (int i = 0; i < num_threads; i++) {
        for (const auto & elem : temp_srcs[i]) {
            src_moments.push_back(elem);
        }
    }

    std::sort(src_moments.begin(), src_moments.end(), [](player_pointer_and_id p0, player_pointer_and_id p1) {
        return p0.ptr.moment_index < p1.ptr.moment_index || (p0.ptr.moment_index == p1.ptr.moment_index && p0.player_id < p1.player_id);
    });

    vector<trajectory_data> temp_trajs[num_threads];

    #pragma omp parallel for if(parallel)
    for (int src_moment_index = 0; src_moment_index < src_moments.size(); src_moment_index++) {
        int thread_num = omp_get_thread_num();
        const player_pointer_and_id& src_moment = src_moments.at(src_moment_index);
        long int player_id = src_moment.player_id;
        int64_t src_time = src_moment.ptr.moment_index;
        // all trajectory ends for the current player
        for (int64_t dst_time = src_time + t_index_offset - t_delta_ticks;
             dst_time < src_time + t_index_offset + t_delta_ticks + 1; dst_time++) {
            if (dst_time < 0 || dst_time > moments->size) {
                continue;
            }
            for (int dst_player_index = 0; dst_player_index < 11; dst_player_index++) {
                if (moments->player_id[src_moment.ptr.player_index][src_time] == moments->player_id[dst_player_index][dst_time] &&
                        moments->game_num[src_time] == moments->game_num[dst_time] &&
                        moments->quarter[src_time] == moments->quarter[dst_time] &&
                        point_intersect_no_time(destination, moments->x_loc[dst_player_index][dst_time],
                                            moments->y_loc[dst_player_index][dst_time])) {
                    {
                        temp_trajs[thread_num].push_back({
                                                          moments->game_id[src_time],
                                                          moments->game_num[src_time],
                                                          moments->team_id[src_moment.ptr.player_index][src_time],
                                                          moments->player_id[src_moment.ptr.player_index][src_time],
                                                          moments->x_loc[src_moment.ptr.player_index][src_time],
                                                          moments->y_loc[src_moment.ptr.player_index][src_time],
                                                          moments->game_clock[src_time].to_double(),
                                                          moments->x_loc[dst_player_index][dst_time],
                                                          moments->y_loc[dst_player_index][dst_time],
                                                          moments->game_clock[dst_time].to_double(),
                                                          moments->quarter[src_time]
                                                  });
                    }
                    continue;
                }
            }
        }

    }

    for (int i = 0; i < num_threads; i++) {
        for (const auto & elem : temp_trajs[i]) {
            trajectories.push_back(elem);
        }
    }
}

void find_trajectories_fixed_origin_clean_binned_just_outer(moment_col_store * moments, court_bins * moment_bins,
                                                            vector<trajectory_data>& trajectories, coordinate_range origin,
                                                            coordinate_range destination, int t_offset, int t_delta_ticks, bool parallel){
    const std::vector<int> &origin_bins = court_bins::get_bins_in_region(origin);
    int t_index_offset = t_offset * 25;
    std::vector<player_pointer_and_id> src_moments;
    int num_threads = omp_get_max_threads();
    vector<player_pointer_and_id> temp_srcs[num_threads];

    #pragma omp parallel for if(parallel)
    for (int player_num = 0; player_num < moment_bins->players_indices_in_bins.size(); player_num++) {
        int thread_num = omp_get_thread_num();
        long int player_id = moment_bins->player_ids[player_num];
        // all trajectory starts for the current player
        for (const auto &src_bin : origin_bins) {
            for (const player_pointer *src_moment = moment_bins->bin_start(player_id, src_bin);
                 src_moment != moment_bins->bin_end(player_id, src_bin); src_moment++) {
                if (point_intersect_no_time(origin, moments->x_loc[src_moment->player_index][src_moment->moment_index],
                                            moments->y_loc[src_moment->player_index][src_moment->moment_index])) {
                    temp_srcs[thread_num].push_back({*src_moment, player_id});
                }
            }
        }
    }
    for (int i = 0; i < num_threads; i++) {
        for (const auto & elem : temp_srcs[i]) {
            src_moments.push_back(elem);
        }
    }
}

void find_trajectories_fixed_origin_clean_binned_min_time(moment_col_store * moments, court_bins * moment_bins,
                                                            vector<trajectory_data>& trajectories, coordinate_range origin,
                                                            coordinate_range destination, int t_offset, int t_delta_ticks, bool parallel){
    const std::vector<int> &origin_bins = court_bins::get_bins_in_region(origin);
    int t_index_offset = t_offset * 25;
    std::vector<player_pointer_and_id> src_moments;
    int num_threads = omp_get_max_threads();
    vector<player_pointer_and_id> temp_srcs[num_threads];

#pragma omp parallel for if(parallel)
    for (int player_num = 0; player_num < moment_bins->players_indices_in_bins.size(); player_num++) {
    }
}
