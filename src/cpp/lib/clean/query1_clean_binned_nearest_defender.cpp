#include "clean_queries.h"
#include <list>
#include <set>

inline int court_bins::get_bin_index(double x, double y) {
    return floor(y)*MAX_X + floor(x);
}

court_bins::court_bins(moment_col_store * moments) {
    // first need to collect all the players, as moments just track 10 on the floor and ball
    // and for each player, track how many moments they are in
    std::set<long int> player_ids;
    for (int player_in_game = 0; player_in_game < NUM_PLAYERS_AND_BALL; player_in_game++) {
        for (int64_t moment_index = 0; moment_index < moments->size; moment_index++) {
            player_ids.insert(moments->player_id[player_in_game][moment_index]);
        }
    }
    // for each player, create an entry in bin_list_starts
    int bin_index = 0;
    for (const auto & player_id : player_ids) {
        players_indices_in_bins[player_id] = bin_index++;
    }

    // create all the bins
    // multidimensional new is valid - https://en.cppreference.com/w/cpp/language/new
    bin_list_starts = new int64_t[player_ids.size()][NUM_BINS];
    // there are 11 players in every moment, no matter which player is in the game at that time
    bin_list_indices = new int64_t[moments->size * NUM_PLAYERS_AND_BALL];

    // now create the bin_list_indices with a data structure that is efficient insert, but inefficient lookup (lists)
    // this will store the locations of all player in each bin in lists before inserting into array
    std::list<int64_t> player_locations[players_indices_in_bins.size()][NUM_BINS];

    for (int player = 0; player < NUM_PLAYERS_AND_BALL; player++) {
        for (int64_t i = 0; i < moments->size; i++) {
            player_locations[players_indices_in_bins[moments->player_id[player][i]]]
                [get_bin_index(moments->x_loc[player][i], moments->y_loc[player][i])].push_back(i);
        }
    }

    // convert the lists to arrays for faster lookup
    int64_t total_index = 0;
    for (int player = 0; player < players_indices_in_bins.size(); player++) {
        for (int bin = 0; bin < NUM_BINS; bin++) {
            bin_list_starts[players_indices_in_bins[player]][bin] = total_index;
            for (const auto & bin_elem : player_locations[players_indices_in_bins[player]][bin]) {
                bin_list_indices[total_index++] = bin_elem;
            }
        }
    }
}

