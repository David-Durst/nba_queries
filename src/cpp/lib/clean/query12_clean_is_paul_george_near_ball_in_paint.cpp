#include "clean_queries.h"
#include <set>
#include <utility>
#include <iostream>
#include <omp.h>

void is_paul_george_near_ball_in_paint_clean(moment_col_store * moments, court_bins * moment_bins, vector<shot_and_player_data>& paul_and_ball,
                                             coordinate_range paint0, coordinate_range paint1, long int player_id) {
    const std::vector<int>& paint0_bins = court_bins::get_bins_in_region(paint0);
    const std::vector<int>& paint1_bins = court_bins::get_bins_in_region(paint1);
    std::vector<int> all_bins;
    all_bins.reserve(paint0_bins.size() + paint1_bins.size());
    all_bins.insert(all_bins.end(), paint0_bins.begin(), paint0_bins.end());
    all_bins.insert(all_bins.end(), paint1_bins.begin(), paint1_bins.end());
    int num_threads = omp_get_max_threads();
    vector<shot_and_player_data> temp_locs[num_threads];

    #pragma omp parallel for
    for (int i = 0; i < all_bins.size(); i++) {
        int thread_num = omp_get_thread_num();
        // all trajectory starts for the current player
        int bin = all_bins.at(i);
        for (const player_pointer *p = moment_bins->bin_start(player_id, bin);
             p != moment_bins->bin_end(player_id, bin); p++) {
            if (point_intersect_no_time(&paint0, p->x_loc, p->y_loc) ||
                point_intersect_no_time(&paint1, p->x_loc, p->y_loc)) {
                player_data player;
                player.team_id = moments->team_id[p->player_index][p->moment_index];
                player.player_id = moments->player_id[p->player_index][p->moment_index];
                player.x_loc = p->x_loc;
                player.y_loc = p->y_loc;
                player.radius = moments->radius[p->player_index][p->moment_index];

                player_data ball;
                ball.team_id = moments->team_id[0][p->moment_index];
                ball.player_id = moments->player_id[0][p->moment_index];
                ball.x_loc = moments->player_id[0][p->moment_index];
                ball.y_loc = moments->player_id[0][p->moment_index];
                ball.radius = moments->radius[0][p->moment_index];

                double distance = hypot(moments->x_loc[0][p->moment_index] - p->x_loc,
                                        moments->y_loc[0][p->moment_index] - p->y_loc);
                temp_locs[thread_num].push_back({
                        true,
                        moments->team_id[p->player_index][p->moment_index],
                        moments->player_id[p->player_index][p->moment_index],
                        p->x_loc,
                        p->y_loc,
                        moments->team_id[0][p->moment_index],
                        moments->player_id[0][p->moment_index],
                        moments->x_loc[0][p->moment_index],
                        moments->y_loc[0][p->moment_index],
                        distance,
                        moments->game_clock[p->moment_index],
                        moments->shot_clock[p->moment_index],
                        moments->quarter[p->moment_index],
                        moments->game_id[p->moment_index],
                        0,// this isn't actually a shot
                        0,
                        false,
                        false
                });
            }
        }
    }
    for (int i = 0; i < num_threads; i++) {
        for (const auto & elem : temp_locs[i]) {
            paul_and_ball.push_back(elem);
        }
    }
}
