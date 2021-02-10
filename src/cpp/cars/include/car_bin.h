//
// Created by durst on 2/9/21.
//

#ifndef NBA_QUERIES_CAR_BIN_H
#define NBA_QUERIES_CAR_BIN_H


class spatial_bins {
public:
    spatial_bins(const moment_col_store& moments);

    static inline __attribute__((always_inline)) int get_bin_index(double x, double y) {
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


    double x_bin_size;
    double y_bin_size;
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

#endif //NBA_QUERIES_CAR_BIN_H
