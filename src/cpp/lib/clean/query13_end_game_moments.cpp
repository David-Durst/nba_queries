#include "clean_queries.h"
#include <set>
#include <utility>
#include <iostream>
#include <omp.h>

void get_end_game_moments(moment_col_store * moments, vector<extra_game_data>& extra_data,
                          vector<ball_height_at_time>& ball_heights, int last_n_seconds) {
    clock_fixed_point start_of_end(last_n_seconds);
    int num_threads = omp_get_max_threads();
    vector<ball_height_at_time> temp_heights[num_threads];

#pragma omp parallel for
   for (int i = 0; i < extra_data.size(); i++) {
       const extra_game_data& game_data = extra_data.at(i);
       int thread_num = omp_get_thread_num();
       for (int quarter = 1; quarter < 5 + game_data.num_ot_periods; quarter++) {
           for (int64_t time = start_of_end.time_to_index(extra_data, game_data.game_num, quarter);
                moments->quarter[time] == quarter; time++) {
               temp_heights[thread_num].push_back({moments->game_clock[time], moments->radius[0][time]});
           }
       }
   }

   for (int i = 0; i < num_threads; i++) {
       for (const auto & elem : temp_heights[i]) {
           ball_heights.push_back(elem);
       }
   }
}
