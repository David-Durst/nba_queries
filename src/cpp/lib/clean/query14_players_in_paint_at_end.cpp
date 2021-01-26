#include "benchmark.h"
#include "clean_queries.h"
#include <functional>
#include <iostream>
#include <omp.h>

void get_players_in_paint_at_end(moment_col_store * moments, vector<extra_game_data>& extra_data,
                                 vector<players_in_paint_at_time>& players_in_paint,
                                 coordinate_range paint0, coordinate_range paint1, int last_n_seconds) {
    clock_fixed_point start_of_end(last_n_seconds);
    int num_threads = omp_get_max_threads();
    vector<players_in_paint_at_time> temp_players[num_threads];

#pragma omp parallel for
    for (int i = 0; i < extra_data.size(); i++) {
        const extra_game_data& game_data = extra_data.at(i);
        int thread_num = omp_get_thread_num();
        for (int quarter = 1; quarter < 5 + game_data.num_ot_periods; quarter++) {
            for (int i = 0; i < NUM_PLAYERS_AND_BALL; i++) {
                for (int64_t time = start_of_end.time_to_index(extra_data, game_data.game_num, quarter);
                 moments->quarter[time] == quarter; time++) {
                    if (point_intersect_no_time(&paint0, moments->x_loc[i][time], moments->y_loc[i][time]) ||
                        point_intersect_no_time(&paint1, moments->x_loc[i][time], moments->y_loc[i][time])) {
                        temp_players[thread_num].push_back({moments->game_clock[time], moments->player_id[i][time]});
                    }
                }
            }
        }
    }

    for (int i = 0; i < num_threads; i++) {
        for (const auto & elem : temp_players[i]) {
            players_in_paint.push_back(elem);
        }
    }

}

void get_players_in_paint_at_end_binned(moment_col_store * moments, court_bins * moment_bins, vector<extra_game_data>& extra_data,
                                        vector<players_in_paint_at_time>& players_in_paint,
                                        coordinate_range paint0, coordinate_range paint1, int last_n_seconds) {
    clock_fixed_point start_of_end(last_n_seconds);
    int num_threads = omp_get_max_threads();
    vector<players_in_paint_at_time> temp_players[num_threads];

    const std::vector<int>& paint0_bins = court_bins::get_bins_in_region(paint0);
    const std::vector<int>& paint1_bins = court_bins::get_bins_in_region(paint1);
    std::vector<int> all_bins;
    all_bins.reserve(paint0_bins.size() + paint1_bins.size());
    all_bins.insert(all_bins.end(), paint0_bins.begin(), paint0_bins.end());
    all_bins.insert(all_bins.end(), paint1_bins.begin(), paint1_bins.end());

#pragma omp parallel for
    for (int i = 0; i < all_bins.size(); i++) {
        int thread_num = omp_get_thread_num();
        // all trajectory starts for the current player
        int bin = all_bins.at(i);
        for (int player_num = 0; player_num < moment_bins->players_indices_in_bins.size(); player_num++) {
            long int player_id = moment_bins->player_ids[i];
            for (const player_pointer *p = moment_bins->bin_start(player_id, bin);
                 p != moment_bins->bin_end(player_id, bin); p++) {
                if ((point_intersect_no_time(&paint0, p->x_loc, p->y_loc) ||
                    point_intersect_no_time(&paint1, p->x_loc, p->y_loc)) &&
                    start_of_end.gt(moments->game_clock[p->moment_index])) {
                    temp_players[thread_num].push_back({moments->game_clock[p->moment_index], moments->player_id[p->player_index][p->moment_index]});
                }
            }

        }
    }

    for (int i = 0; i < num_threads; i++) {
        for (const auto & elem : temp_players[i]) {
            players_in_paint.push_back(elem);
        }
    }

}
