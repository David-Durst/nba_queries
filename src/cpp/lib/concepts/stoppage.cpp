#include "concepts.h"
#include "col_stores.h"
#include "clean_queries.h"
#include <memory>
#include <vector>
#include <omp.h>
#include <random>
#include <set>
#include <utility>
using std::vector;
using std::set;
using std::pair;


void Stoppage::compute(const moment_col_store &moments, const shot_col_store &shots) {
    num_windows = moments.size / this->ticks_in_window;
    is_window_stoppage = (bool *) malloc(num_windows * sizeof(bool));
    start_moment_index = (int64_t *) malloc(num_windows * sizeof(int64_t));
#pragma omp parallel for
    for (int64_t src_time = 0; src_time < moments.size - ticks_in_window; src_time += ticks_in_window) {
        int64_t cur_window = src_time / ticks_in_window;
        double x_last_tick[NUM_PLAYERS_AND_BALL];
        double y_last_tick[NUM_PLAYERS_AND_BALL];
        bool stoppage_found = false;
        for (int i = 0; i < NUM_PLAYERS_AND_BALL; i++) {
            x_last_tick[i] = moments.x_loc[i][src_time];
            y_last_tick[i] = moments.y_loc[i][src_time];
        }
        for (int64_t cur_time = src_time + 1; cur_time < src_time + ticks_in_window; cur_time++) {
            for (int i = 0; i < NUM_PLAYERS_AND_BALL; i++) {
                double distance = hypot(moments.x_loc[i][cur_time] - moments.x_loc[i][cur_time-1],
                                        moments.y_loc[i][cur_time] - moments.y_loc[i][cur_time-1]);
                if (distance <= min_movement_per_tick || distance >= max_movement_per_tick) {
                    stoppage_found = true;
                    break;
                }
            }
            if (stoppage_found) {
                break;
            }
        }
        start_moment_index[cur_window] = src_time;
        is_window_stoppage[cur_window] = stoppage_found;
    }
}
