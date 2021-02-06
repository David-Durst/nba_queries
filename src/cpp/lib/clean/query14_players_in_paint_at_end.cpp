#include "benchmark.h"
#include "clean_queries.h"
#include <set>
#include <functional>
#include <iostream>
#include <omp.h>
void get_players_in_paint_at_end(moment_col_store * moments, vector<extra_game_data>& extra_data,
                                 vector<players_in_paint_at_time>& players_in_paint,
                                 coordinate_range paint0, coordinate_range paint1, int last_n_seconds) {
    clock_fixed_point start_of_end(last_n_seconds);
    int num_threads = omp_get_max_threads();
    vector<players_in_paint_at_time> temp_players[num_threads];
    /*
    double time_taken[num_threads];
    for (int i = 0; i < num_threads; i++) {
        time_taken[i] = 0;
    }
     */

#pragma omp parallel for
    for (int i = 0; i < extra_data.size(); i++) {
        //auto start_t = Halide::Tools::benchmark_now();
        const extra_game_data& game_data = extra_data.at(i);
        int thread_num = omp_get_thread_num();
        for (int quarter = 1; quarter < 5 + game_data.num_ot_periods; quarter++) {
            for (int i = 0; i < NUM_PLAYERS_AND_BALL; i++) {
                for (int64_t time = start_of_end.time_to_index(extra_data, game_data.game_num, quarter);
                 moments->quarter[time] == quarter; time++) {
                    if (((moments->x_loc[i][time] >= paint0.start.x && moments->x_loc[i][time] <= paint0.end.x &&
                          moments->y_loc[i][time] >= paint0.start.y && moments->y_loc[i][time] <= paint0.end.y) ||
                         (moments->x_loc[i][time] >= paint1.start.x && moments->x_loc[i][time] <= paint1.end.x &&
                         moments->y_loc[i][time] >= paint1.start.y && moments->y_loc[i][time] <= paint1.end.y))) {
                        temp_players[thread_num].push_back({time, moments->game_clock[time], moments->player_id[i][time]});
                    }
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
    /*
    double min_t = 10000.0;
    double max_t = 0.0;
    for (int i = 0; i < num_threads; i++) {
        if (time_taken[i] < min_t) {
            min_t = time_taken[i];
        }
        if (time_taken[i] > max_t) {
            max_t = time_taken[i];
        }
    }
    std::cout << "time per thread min " << min_t << " and maax " << max_t << std::endl;
     */
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
            long int player_id = moment_bins->player_ids[player_num];
            for (const player_pointer *p = moment_bins->bin_start(player_id, bin);
                 p != moment_bins->bin_end(player_id, bin); p++) {
                if ((point_intersect_no_time(paint0, p->x_loc, p->y_loc) ||
                    point_intersect_no_time(paint1, p->x_loc, p->y_loc)) &&
                    start_of_end.gt(moments->game_clock[p->moment_index])) {
                    temp_players[thread_num].push_back({p->moment_index, moments->game_clock[p->moment_index], moments->player_id[p->player_index][p->moment_index]});
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

void get_players_in_paint_at_end_binned_with_time(moment_col_store * moments, court_and_game_clock_bins * moment_bins, vector<extra_game_data>& extra_data,
                                        vector<players_in_paint_at_time>& players_in_paint,
                                        coordinate_range paint0, coordinate_range paint1, int last_n_seconds) {
    clock_fixed_point start_of_end(last_n_seconds);
    int num_threads = omp_get_max_threads();
    vector<players_in_paint_at_time> temp_players[num_threads];

    const std::vector<int>& paint0_bins = court_and_game_clock_bins::get_bins_in_region(paint0);
    const std::vector<int>& paint1_bins = court_and_game_clock_bins::get_bins_in_region(paint1);
    std::vector<int> all_bins;
    all_bins.reserve(paint0_bins.size() + paint1_bins.size());
    all_bins.insert(all_bins.end(), paint0_bins.begin(), paint0_bins.end());
    all_bins.insert(all_bins.end(), paint1_bins.begin(), paint1_bins.end());
    vector<player_pointer_and_id> temp_moments[num_threads];
    vector<player_pointer_and_id> flat_temp_moments;

    #pragma omp parallel for
    for (int i = 0; i < all_bins.size(); i++) {
        int thread_num = omp_get_thread_num();
        // all trajectory starts for the current player
        int bin = all_bins.at(i);
        for (int player_num = 0; player_num < moment_bins->players_indices_in_bins.size(); player_num++) {
            long int player_id = moment_bins->player_ids[player_num];
            for (const player_pointer *p = moment_bins->bin_start(player_id, bin);
                 p != moment_bins->bin_end(player_id, bin); p++) {
                temp_moments[thread_num].push_back({*p, player_id});
            }
        }
    }

    for (int i = 0; i < num_threads; i++) {
        for (const auto & elem : temp_moments[i]) {
            flat_temp_moments.push_back(elem);
        }
    }

    // sorting doesn't help
    //std::sort(flat_temp_moments.begin(), flat_temp_moments.end(), [](player_pointer_and_id p0, player_pointer_and_id p1) {
        //return p0.ptr.moment_index < p1.ptr.moment_index || (p0.ptr.moment_index == p1.ptr.moment_index && p0.player_id < p1.player_id);
    //});

    #pragma omp parallel for
    for (int i = 0; i < flat_temp_moments.size(); i++) {
        int thread_num = omp_get_thread_num();
        const player_pointer_and_id& p = flat_temp_moments[i];
        if ((point_intersect_no_time(paint0, p.ptr.x_loc, p.ptr.y_loc) ||
             point_intersect_no_time(paint1, p.ptr.x_loc, p.ptr.y_loc)) &&
            start_of_end.gt(moments->game_clock[p.ptr.moment_index])) {
            temp_players[thread_num].push_back({p.ptr.moment_index, moments->game_clock[p.ptr.moment_index], moments->player_id[p.ptr.player_index][p.ptr.moment_index]});
        }
    }

    for (int i = 0; i < num_threads; i++) {
        for (const auto & elem : temp_players[i]) {
            players_in_paint.push_back(elem);
        }
    }
}

void get_players_in_paint_at_end_binned_with_time_fix_par(moment_col_store * moments, court_and_game_clock_bins * moment_bins, vector<extra_game_data>& extra_data,
                                                  vector<players_in_paint_at_time>& players_in_paint,
                                                  coordinate_range paint0, coordinate_range paint1, int last_n_seconds) {
    clock_fixed_point start_of_end(last_n_seconds);
    int num_threads = omp_get_max_threads();
    vector<players_in_paint_at_time> temp_players[num_threads];
    /*
    double time_taken[num_threads];
    for (int i = 0; i < num_threads; i++) {
        time_taken[i] = 0;
    }
     */

    //auto start = Halide::Tools::benchmark_now();
    const std::vector<int>& paint0_bins = court_and_game_clock_bins::get_bins_in_region(paint0);
    const std::vector<int>& paint1_bins = court_and_game_clock_bins::get_bins_in_region(paint1);
    std::vector<int> all_bins;
    all_bins.reserve(paint0_bins.size() + paint1_bins.size());
    all_bins.insert(all_bins.end(), paint0_bins.begin(), paint0_bins.end());
    all_bins.insert(all_bins.end(), paint1_bins.begin(), paint1_bins.end());
    //time_taken[0] += Halide::Tools::benchmark_duration_seconds(start, Halide::Tools::benchmark_now());

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
            if (((p->x_loc >= paint0.start.x && p->x_loc <= paint0.end.x &&
                 p->y_loc >= paint0.start.y && p->y_loc <= paint0.end.y) ||
                (p->x_loc >= paint1.start.x && p->x_loc <= paint1.end.x &&
                 p->y_loc >= paint1.start.y && p->y_loc <= paint1.end.y)) &&
                start_of_end.gt(moments->game_clock[p->moment_index])) {
                temp_players[thread_num].push_back({p->moment_index, moments->game_clock[p->moment_index], moments->player_id[p->player_index][p->moment_index]});
            }
        }
        //time_taken[thread_num] += Halide::Tools::benchmark_duration_seconds(start_t, Halide::Tools::benchmark_now());
    }

    //double time_taken[num_threads];
    for (int i = 0; i < num_threads; i++) {
        for (const auto & elem : temp_players[i]) {
            players_in_paint.push_back(elem);
        }
    }
    /*
    //time_taken[0] += Halide::Tools::benchmark_duration_seconds(start, Halide::Tools::benchmark_now());
    double min_t = 10000.0;
    double max_t = 0.0;
    for (int i = 0; i < num_threads; i++) {
        if (time_taken[i] < min_t) {
            min_t = time_taken[i];
        }
        if (time_taken[i] > max_t) {
            max_t = time_taken[i];
        }
    }
    std::cout << "time per thread min " << min_t << " and maax " << max_t << std::endl;
     */
}
court_and_game_clock_bins::court_and_game_clock_bins(moment_col_store * moments) {
    // first need to collect all the players, as moments just track 10 on the floor and ball
    // and for each player, track how many moments they are in
    std::set<long int> player_ids_set;
    for (int player_in_game = 0; player_in_game < NUM_PLAYERS_AND_BALL; player_in_game++) {
        for (int64_t moment_index = 0; moment_index < moments->size; moment_index++) {
            player_ids_set.insert(moments->player_id[player_in_game][moment_index]);
        }
    }
    std::cout << "creating player_id map" << std::endl;
    // for each player, track the id and map it to the bin index
    player_ids = new long int[player_ids_set.size()];
    int bin_index = 0;
    for (const auto & player_id : player_ids_set) {
        player_ids[bin_index] = player_id;
        players_indices_in_bins[player_id] = bin_index++;
    }

    // create all the bins
    // multidimensional new is valid - https://en.cppreference.com/w/cpp/language/new
    bin_starts = new int64_t[player_ids_set.size()][NUM_BINS_WITH_TIME];
    // there are 11 players in every moment, no matter which player is in the game at that time
    num_player_moments = moments->size * NUM_PLAYERS_AND_BALL;
    player_moment_bins = new player_pointer[num_player_moments];

    std::cout << "creating vector" << std::endl;

    // now create the bins with a data structure that is efficient insert, but inefficient lookup (lists)
    // this will store the locations of all player in each bin in lists before inserting into array
    std::vector<std::vector<std::vector<player_pointer>>> bin_data_in_lists;
    bin_data_in_lists.resize(players_indices_in_bins.size(), std::vector<std::vector<player_pointer>>());
    for (int64_t i = 0; i < players_indices_in_bins.size(); i++) {
        bin_data_in_lists.push_back(std::vector<std::vector<player_pointer>>());
        bin_data_in_lists.at(i).resize(NUM_BINS_WITH_TIME, std::vector<player_pointer>());
    }

    std::cout << "filling vector" << std::endl;

    for (int player = 0; player < NUM_PLAYERS_AND_BALL; player++) {
        for (int64_t i = 0; i < moments->size; i++) {
            if (bin_index >= NUM_BINS_WITH_TIME) {
                std::cout << "problem with " << moments->x_loc[player][i] << "," << moments->y_loc[player][i] << std::endl;
            }
            int64_t bin_num = get_bin_index(moments->x_loc[player][i], moments->y_loc[player][i], moments->game_clock[i].to_double());
            bin_data_in_lists.at(players_indices_in_bins.at(moments->player_id[player][i]))
                    .at(get_bin_index(moments->x_loc[player][i], moments->y_loc[player][i], moments->game_clock[i].to_double()))
                    .push_back({i, player, moments->x_loc[player][i], moments->y_loc[player][i]});
        }
    }

    std::cout << "moving vectors to arrays" << std::endl;

    // convert the lists to arrays for faster lookup
    int64_t total_index = 0;
    for (const auto & player_id : player_ids_set) {
        for (int bin = 0; bin < NUM_BINS_WITH_TIME; bin++) {
            bin_starts[players_indices_in_bins.at(player_id)][bin] = total_index;
            for (const auto & bin_data : bin_data_in_lists[players_indices_in_bins[player_id]][bin]) {
                player_moment_bins[total_index++] = bin_data;
            }
        }
    }
}

