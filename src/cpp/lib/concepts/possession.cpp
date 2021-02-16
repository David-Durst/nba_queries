#include "concepts.h"
#include "col_stores.h"
#include "clean_queries.h"
#include <vector>
#include <omp.h>
#include <random>
#include <set>
#include <utility>
using std::vector;
using std::set;
using std::pair;


void Possession::compute(const moment_col_store &moments, const shot_col_store &shots) {
    int num_threads = omp_get_max_threads();
#pragma omp parallel for
    for (int64_t src_time = 0; src_time < moments.size - this->ticks_in_window; src_time += this->ticks_in_window) {
        int thread_num = omp_get_thread_num();
        // ball ticked when nobody is nearest
        int num_ticks_player_nearest[] = {0,0,0,0,0,0,0,0,0,0,0};
        for (int64_t cur_time = src_time; cur_time < src_time + ticks_in_window; cur_time++) {
            double min_distance = 10000000.0;
            int min_index = 0;
            for (int i = 1; i < NUM_PLAYERS_AND_BALL; i++) {
                double new_distance = 0;
                if (moments.radius[0][cur_time] > arm_height) {
                    new_distance = std::hypot(moments.x_loc[0][cur_time] - moments.x_loc[i][cur_time],
                                         moments.y_loc[0][cur_time] - moments.y_loc[i][cur_time],
                                         moments.radius[0][cur_time] - arm_height);
                }
                else {
                    new_distance = std::hypot(moments.x_loc[0][cur_time] - moments.x_loc[i][cur_time],
                                             moments.y_loc[0][cur_time] - moments.y_loc[i][cur_time]);
                }
                if (new_distance < min_distance) {
                    min_distance = new_distance;
                    min_index = i;
                }
            }
            if (min_distance > max_distance) {
                num_ticks_player_nearest[min_index]++;
            }
            else {
                num_ticks_player_nearest[min_index]++;
            }
        }
        int max_index = 0;
        for (int i = 1; i < NUM_PLAYERS_AND_BALL; i++) {
            if (num_ticks_player_nearest[i] > num_ticks_player_nearest[max_index]) {
                max_index = i;
            }
        }
        this->start_moment_index_unmerged[thread_num].push_back(src_time);
        this->possessor_indices_unmerged[thread_num].push_back(max_index - 1);
        this->possessor_ids_unmerged[thread_num].push_back(moments.player_id[max_index][src_time]);
        this->possessor_team_unmerged[thread_num].push_back(moments.team_id[max_index][src_time]);
    }
}
