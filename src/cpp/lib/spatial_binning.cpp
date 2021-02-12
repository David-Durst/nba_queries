#include "benchmark.h"
#include "clean_queries.h"
#include <set>
#include <functional>
#include <iostream>
#include <omp.h>

court_bins::court_bins(const moment_col_store& moments) {
    // first need to collect all the players, as moments just track 10 on the floor and ball
    // and for each player, track how many moments they are in
    auto start_pre_vec = Halide::Tools::benchmark_now();
    std::set<long int> player_ids_set;
    for (int player_in_game = 0; player_in_game < NUM_PLAYERS_AND_BALL; player_in_game++) {
        for (int64_t moment_index = 0; moment_index < moments.size; moment_index++) {
            player_ids_set.insert(moments.player_id[player_in_game][moment_index]);
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
    bin_starts = new int64_t[player_ids_set.size()][NUM_BINS];
    // there are 11 players in every moment, no matter which player is in the game at that time
    num_player_moments = moments.size * NUM_PLAYERS_AND_BALL;
    player_moment_bins = new player_pointer[num_player_moments];

    std::cout << "creating vectors" << std::endl;

    /*
    // now create the bins with a data structure that is efficient insert, but inefficient lookup (lists)
    // this will store the locations of all player in each bin in lists before inserting into array
    std::vector<std::vector<std::vector<player_pointer>>> bin_data_in_lists;
    for (int64_t i = 0; i < players_indices_in_bins.size(); i++) {
        bin_data_in_lists.push_back(std::vector<std::vector<player_pointer>>());
        for (int64_t j = 0; j < NUM_BINS; j++) {
            bin_data_in_lists[i].push_back(std::vector<player_pointer>());
        }
    }
     */

    auto time_pre_vec = Halide::Tools::benchmark_duration_seconds(start_pre_vec, Halide::Tools::benchmark_now());
    std::cout << "it took " << time_pre_vec << " seconds to pre_vec" << std::endl;

    auto start_vec_count = Halide::Tools::benchmark_now();
    // create bin_starts by couting amount in each bin then summing across bins
    for (int player = 0; player < NUM_PLAYERS_AND_BALL; player++) {
        for (int64_t i = 0; i < moments.size; i++) {
            bin_starts[players_indices_in_bins.at(moments.player_id[player][i])][get_bin_index(moments.x_loc[player][i], moments.y_loc[player][i])]++;
        }
    }
    int64_t total_index = 0;
    int64_t bin_count = 0;
    for (int player = 0; player < player_ids_set.size(); player++) {
        for (int64_t bin = 0; bin < NUM_BINS; bin++) {
            bin_count = bin_starts[player][bin];
            bin_starts[player][bin] = total_index;
            total_index += bin_count;
        }
    }

    auto time_vec_count = Halide::Tools::benchmark_duration_seconds(start_vec_count, Halide::Tools::benchmark_now());
    std::cout << "it took " << time_vec_count << " seconds to compute starts" << std::endl;

    auto start_array_fill = Halide::Tools::benchmark_now();
    int64_t (* entries_per_bin)[NUM_BINS] = new int64_t[player_ids_set.size()][NUM_BINS];
    for (int player = 0; player < NUM_PLAYERS_AND_BALL; player++) {
        for (int64_t i = 0; i < moments.size; i++) {
            auto player_index_in_bin = players_indices_in_bins.at(moments.player_id[player][i]);
            auto bin = get_bin_index(moments.x_loc[player][i], moments.y_loc[player][i]);
            auto cur_bin_entry = entries_per_bin[player_index_in_bin][bin]++;
            player_moment_bins[bin_starts[player_index_in_bin][bin] + cur_bin_entry] = {i, player,
                                                                                        moments.x_loc[player][i],
                                                                                        moments.y_loc[player][i]};
        }
    }
    auto time_array_fill = Halide::Tools::benchmark_duration_seconds(start_array_fill, Halide::Tools::benchmark_now());
    std::cout << "it took " << time_array_fill << " seconds to array_fill" << std::endl;

    /*
    std::cout << "filling vector" << std::endl;
    auto start_vec_fill = Halide::Tools::benchmark_now();

    for (int player = 0; player < NUM_PLAYERS_AND_BALL; player++) {
        for (int64_t i = 0; i < moments.size; i++) {
            bin_data_in_lists[players_indices_in_bins.at(moments.player_id[player][i])]
                             [get_bin_index(moments.x_loc[player][i], moments.y_loc[player][i])].push_back({i, player,
                                                                                                    moments.x_loc[player][i],
                                                                                                    moments.y_loc[player][i]});
        }
    }

    auto time_vec_fill = Halide::Tools::benchmark_duration_seconds(start_vec_fill, Halide::Tools::benchmark_now());
    std::cout << "it took " << time_vec_fill << " seconds to vec_fill" << std::endl;
    std::cout << "moving vectors to arrays" << std::endl;

    auto start_vec_to_array = Halide::Tools::benchmark_now();
    // convert the lists to arrays for faster lookup
    int64_t total_index = 0;
    for (const auto & player_id : player_ids_set) {
        for (int bin = 0; bin < NUM_BINS; bin++) {
            bin_starts[players_indices_in_bins.at(player_id)][bin] = total_index;
            for (const auto & bin_data : bin_data_in_lists[players_indices_in_bins[player_id]][bin]) {
                player_moment_bins[total_index++] = bin_data;
            }
        }
    }
    auto time_vec_to_array = Halide::Tools::benchmark_duration_seconds(start_vec_to_array, Halide::Tools::benchmark_now());
    std::cout << "it took " << time_vec_to_array << " seconds to vec_to_array" << std::endl;
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

