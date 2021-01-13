#include "clean_queries.h"
#include <set>
#include <utility>

void find_trajectories_fixed_origin_clean_binned(moment_col_store * moments, court_bins * moment_bins,
                                                 list<trajectory_data> * trajectories, coordinate_range origin,
                                                 coordinate_range destination, int t_offset, int t_delta_ticks,
                                                 bool parallel) {
    std::list<int> origin_bins = court_bins::get_bins_in_region(origin);
    std::list<int> destination_bins = court_bins::get_bins_in_region(destination);
    int t_index_offset = t_offset * 25;

    #pragma omp parallel for if(parallel)
    for (int player_num = 0; player_num < moment_bins->players_indices_in_bins.size(); player_num++) {
        long int player_id = moment_bins->player_ids[player_num];
        std::list<player_pointer> src_moments;
        // all trajectory starts for the current player
        for (const auto & src_bin : origin_bins) {
            for (const player_pointer* src_moment = moment_bins->bin_start(player_id, src_bin);
                 src_moment != moment_bins->bin_end(player_id, src_bin); src_moment++) {
                if (point_intersect_no_time(&origin, moments->x_loc[src_moment->player_index][src_moment->moment_index],
                                            moments->y_loc[src_moment->player_index][src_moment->moment_index])) {
                    src_moments.push_back(*src_moment);
                }
            }
        }

        // now for each
        for (const auto & src_moment : src_moments) {
            int64_t src_time = src_moment.moment_index;
            // all trajectory ends for the current player
            for (int64_t dst_time = src_time + t_index_offset - t_delta_ticks;
                 dst_time < src_time + t_index_offset + t_delta_ticks + 1; dst_time++) {
                if (dst_time < 0 || dst_time > moments->size) {
                    continue;
                }
                for (int dst_player_index = 0; dst_player_index < 11; dst_player_index++) {
                    if (moments->player_id[src_moment.player_index][src_time] == moments->player_id[dst_player_index][dst_time] &&
                        point_intersect_no_time(&destination, moments->x_loc[dst_player_index][dst_time],
                                                moments->y_loc[dst_player_index][dst_time])) {
                        #pragma omp critical
                        {
                            trajectories->append_node({
                                                              moments->team_id[src_moment.player_index][src_time],
                                                              moments->player_id[src_moment.player_index][src_time],
                                                              moments->x_loc[src_moment.player_index][src_time],
                                                              moments->y_loc[src_moment.player_index][src_time],
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
    }

}

court_bins::court_bins(moment_col_store * moments) {
    // first need to collect all the players, as moments just track 10 on the floor and ball
    // and for each player, track how many moments they are in
    std::set<long int> player_ids_set;
    for (int player_in_game = 0; player_in_game < NUM_PLAYERS_AND_BALL; player_in_game++) {
        for (int64_t moment_index = 0; moment_index < moments->size; moment_index++) {
            player_ids_set.insert(moments->player_id[player_in_game][moment_index]);
        }
    }
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
    num_player_moments = moments->size * NUM_PLAYERS_AND_BALL;
    player_moment_bins = new player_pointer[num_player_moments];

    // now create the bins with a data structure that is efficient insert, but inefficient lookup (lists)
    // this will store the locations of all player in each bin in lists before inserting into array
    std::list<player_pointer> bin_data_in_lists[players_indices_in_bins.size()][NUM_BINS];

    int64_t total_sum = 0;
    for (int player = 0; player < NUM_PLAYERS_AND_BALL; player++) {
        for (int64_t i = 0; i < moments->size; i++) {
            bin_data_in_lists[players_indices_in_bins.at(moments->player_id[player][i])]
                [get_bin_index(moments->x_loc[player][i], moments->y_loc[player][i])].push_back({i, player});
            total_sum++;
        }
    }

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
}

