#include "benchmark.h"
#include "clean_queries.h"
#include <set>
#include <functional>
#include <iostream>
#include <omp.h>

void get_players_in_paint_shot_clock(moment_col_store * moments, vector<players_in_paint_at_time>& players_in_paint,
                                     coordinate_range paint0, coordinate_range paint1, double end_time) {
    int num_threads = omp_get_max_threads();
    vector<players_in_paint_at_time> temp_players[num_threads];
#pragma omp parallel for
    for (int64_t time = 0; time < moments->size; time++) {
        //auto start_t = Halide::Tools::benchmark_now();
        int thread_num = omp_get_thread_num();
        if (moments->shot_clock[time] < end_time) {
            for (int i = 0; i < NUM_PLAYERS_AND_BALL; i++) {
                if (point_intersect_no_time(&paint0, moments->x_loc[i][time], moments->y_loc[i][time]) ||
                    point_intersect_no_time(&paint1, moments->x_loc[i][time], moments->y_loc[i][time])) {
                    temp_players[thread_num].push_back({time, moments->game_clock[time], moments->player_id[i][time]});
                }
            }
        }
        //time_taken[thread_num] += Halide::Tools::benchmark_duration_seconds(start_t, Halide::Tools::benchmark_now());
    }

    for (int i = 0; i < num_threads; i++) {
        for (const auto & elem : temp_players[i]) {
            players_in_paint.push_back(elem);
        }
    }
}

void get_players_in_paint_shot_clock_binned(moment_col_store * moments, court_bins * moment_bins,
                                            vector<players_in_paint_at_time>& players_in_paint,
                                            coordinate_range paint0, coordinate_range paint1, double end_time) {
    int num_threads = omp_get_max_threads();
    vector<players_in_paint_at_time> temp_players[num_threads];

    const std::vector<int>& paint0_bins = court_bins::get_bins_in_region(paint0);
    const std::vector<int>& paint1_bins = court_bins::get_bins_in_region(paint1);
    std::vector<int> all_bins;
    all_bins.reserve(paint0_bins.size() + paint1_bins.size());
    all_bins.insert(all_bins.end(), paint0_bins.begin(), paint0_bins.end());
    all_bins.insert(all_bins.end(), paint1_bins.begin(), paint1_bins.end());

#pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < all_bins.size()* moment_bins->players_indices_in_bins.size(); i++) {
        // auto start_t = Halide::Tools::benchmark_now();
        int thread_num = omp_get_thread_num();
        // all trajectory starts for the current player
        int bin = all_bins[i / moment_bins->players_indices_in_bins.size()];
        int player_num = i % moment_bins->players_indices_in_bins.size();
        long int player_id = moment_bins->player_ids[player_num];
        for (const player_pointer *p = moment_bins->bin_start(player_id, bin);
             p != moment_bins->bin_end(player_id, bin); p++) {
            if ((point_intersect_no_time(&paint0, p->x_loc, p->y_loc) ||
                 point_intersect_no_time(&paint1, p->x_loc, p->y_loc)) &&
                 moments->shot_clock[p->moment_index] < end_time) {
                temp_players[thread_num].push_back({p->moment_index, moments->game_clock[p->moment_index], moments->player_id[p->player_index][p->moment_index]});
            }
        }
    }

    for (int i = 0; i < num_threads; i++) {
        for (const auto & elem : temp_players[i]) {
            players_in_paint.push_back(elem);
        }
    }
}
