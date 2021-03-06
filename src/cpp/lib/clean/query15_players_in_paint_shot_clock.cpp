#include "benchmark.h"
#include "clean_queries.h"
#include <set>
#include <functional>
#include <iostream>
#include <omp.h>
#include <thread>
#include <atomic>

void players_in_paint_inner(moment_col_store * moments, vector<players_in_paint_at_time> * temp_players,
                            coordinate_range paint0, coordinate_range paint1, double end_time,
                            std::atomic<int64_t>* cur_index, int step_size) {
    for (int64_t cur_end = cur_index->fetch_add(step_size); cur_end <= moments->size; cur_end = cur_index->fetch_add(step_size)) {
        for (int64_t time = cur_end - step_size; time < cur_end; time++) {
            if (moments->shot_clock[time] < end_time) {
                for (int i = 0; i < NUM_PLAYERS_AND_BALL; i++) {
                    if (point_intersect_no_time(paint0, moments->x_loc[i][time], moments->y_loc[i][time]) ||
                        point_intersect_no_time(paint1, moments->x_loc[i][time], moments->y_loc[i][time])) {
                        temp_players->push_back({time, moments->game_clock[time], moments->player_id[i][time]});
                    }
                }
            }
        }

    }

}
/*
void test(moment_col_store * moments, vector<players_in_paint_at_time> * temp_players,
          coordinate_range paint0, coordinate_range paint1, double end_time,
          std::atomic<int64_t>* cur_index) {}
          */

void get_players_in_paint_shot_clock_hand_tuned(moment_col_store * moments, vector<players_in_paint_at_time>& players_in_paint,
                                     coordinate_range paint0, coordinate_range paint1, double end_time) {
    int num_threads = omp_get_max_threads();
    vector<players_in_paint_at_time> temp_players[num_threads];
    std::thread threads[num_threads];
    std::atomic<int64_t> cur_index = 100000;
    for (int i = 0; i < num_threads; i++) {
        //players_in_paint_inner(moments, &(temp_players[i]), paint0, paint1, end_time, &cur_index);
        //threads[i] = std::thread(test, moments, &temp_players[i], paint0, paint1, end_time, &cur_index);
        threads[i] = std::thread(players_in_paint_inner, moments, &temp_players[i], paint0, paint1, end_time, &cur_index, 100000);
    }

    for (int i = 0; i < num_threads; i++) {
        threads[i].join();
    }

    for (int i = 0; i < num_threads; i++) {
        for (const auto & elem : temp_players[i]) {
            players_in_paint.push_back(elem);
        }
    }
}

void get_players_in_paint_shot_clock(moment_col_store * moments, vector<players_in_paint_at_time>& players_in_paint,
                                     coordinate_range paint0, coordinate_range paint1, double end_time) {
    int num_threads = omp_get_max_threads();
    vector<players_in_paint_at_time> temp_players[num_threads];
    auto start_par = Halide::Tools::benchmark_now();
#pragma omp parallel for
    for (int64_t time_and_s = 0; time_and_s < moments->size * NUM_PLAYERS_AND_BALL; time_and_s++) {
        int64_t time = time_and_s / NUM_PLAYERS_AND_BALL;
        int i = time_and_s % NUM_PLAYERS_AND_BALL;
        int thread_num = omp_get_thread_num();
        if (moments->shot_clock[time] < end_time) {
            if (point_intersect_no_time(paint0, moments->x_loc[i][time], moments->y_loc[i][time]) ||
                point_intersect_no_time(paint1, moments->x_loc[i][time], moments->y_loc[i][time])) {
                temp_players[thread_num].push_back({time, moments->game_clock[time], moments->player_id[i][time]});
            }
        }
    }
    auto time_par = Halide::Tools::benchmark_duration_seconds(start_par, Halide::Tools::benchmark_now());
    std::cout << "it took " << time_par * 1e3 << "ms to do par part" << std::endl;

    auto start_seq = Halide::Tools::benchmark_now();
    for (int i = 0; i < num_threads; i++) {
        for (const auto & elem : temp_players[i]) {
            players_in_paint.push_back(elem);
        }
    }
    auto time_seq = Halide::Tools::benchmark_duration_seconds(start_seq, Halide::Tools::benchmark_now());
    std::cout << "it took " << time_seq * 1e3 << "ms to do seq seqt" << std::endl;
}

void get_players_in_paint_shot_clock_no_funcs(moment_col_store * moments, vector<players_in_paint_at_time>& players_in_paint,
                                                        coordinate_range paint0, coordinate_range paint1, double end_time) {
    int num_threads = omp_get_max_threads();
    vector<players_in_paint_at_time> temp_players[num_threads];
    #pragma omp parallel for
    for (int64_t time = 0; time < moments->size; time++) {
        //auto start_t = Halide::Tools::benchmark_now();
        int thread_num = omp_get_thread_num();
        if (moments->shot_clock[time] < end_time) {
            for (int i = 0; i < NUM_PLAYERS_AND_BALL; i++) {
                if ((moments->x_loc[i][time] >= paint0.start.x && moments->x_loc[i][time] <= paint0.end.x &&
                     moments->y_loc[i][time] >= paint0.start.y && moments->y_loc[i][time] <= paint0.end.y) ||
                    (moments->x_loc[i][time] >= paint1.start.x && moments->x_loc[i][time] <= paint1.end.x &&
                     moments->y_loc[i][time] >= paint1.start.y && moments->y_loc[i][time] <= paint1.end.y)) {
                    temp_players[thread_num].push_back({time, moments->game_clock[time], moments->player_id[i][time]});
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
}

void get_players_in_paint_shot_clock_no_ptr_funcs(moment_col_store * moments, vector<players_in_paint_at_time>& players_in_paint,
                                     coordinate_range paint0, coordinate_range paint1, double end_time) {
    int num_threads = omp_get_max_threads();
    vector<players_in_paint_at_time> temp_players[num_threads];
#pragma omp parallel for
    for (int64_t time = 0; time < moments->size; time++) {
        //auto start_t = Halide::Tools::benchmark_now();
        int thread_num = omp_get_thread_num();
        if (moments->shot_clock[time] < end_time) {
            for (int i = 0; i < NUM_PLAYERS_AND_BALL; i++) {
                if (point_intersect_no_time_no_ptr(paint0, moments->x_loc[i][time], moments->y_loc[i][time]) ||
                    point_intersect_no_time_no_ptr(paint1, moments->x_loc[i][time], moments->y_loc[i][time])) {
                    temp_players[thread_num].push_back({time, moments->game_clock[time], moments->player_id[i][time]});
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
}

void get_players_in_paint_shot_clock_one_paint(moment_col_store * moments, vector<players_in_paint_at_time>& players_in_paint,
                                     coordinate_range paint0, double end_time) {
    int num_threads = omp_get_max_threads();
    vector<players_in_paint_at_time> temp_players[num_threads];
#pragma omp parallel for
    for (int64_t time = 0; time < moments->size; time++) {
        //auto start_t = Halide::Tools::benchmark_now();
        int thread_num = omp_get_thread_num();
        if (moments->shot_clock[time] < end_time) {
            for (int i = 0; i < NUM_PLAYERS_AND_BALL; i++) {
                if (point_intersect_no_time(paint0, moments->x_loc[i][time], moments->y_loc[i][time])) {
                    temp_players[thread_num].push_back({time, moments->game_clock[time], moments->player_id[i][time]});
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
}

void get_players_in_paint_shot_clock_one_paint_no_vec(moment_col_store * moments, vector<players_in_paint_at_time>& players_in_paint,
                                               coordinate_range paint0, double end_time) {
    int num_threads = omp_get_max_threads();
    int temp_players[num_threads];
#pragma omp parallel for
    for (int64_t time = 0; time < moments->size; time++) {
        //auto start_t = Halide::Tools::benchmark_now();
        int thread_num = omp_get_thread_num();
        if (moments->shot_clock[time] < end_time) {
            for (int i = 0; i < NUM_PLAYERS_AND_BALL; i++) {
                if (point_intersect_no_time(paint0, moments->x_loc[i][time], moments->y_loc[i][time])) {
                    temp_players[thread_num]++;
                }
            }
        }
        //time_taken[thread_num] += Halide::Tools::benchmark_duration_seconds(start_t, Halide::Tools::benchmark_now());
    }

}

void get_players_in_paint_shot_clock_binned(moment_col_store * moments, court_bins * moment_bins,
                                            vector<players_in_paint_at_time>& players_in_paint,
                                            coordinate_range paint0, coordinate_range paint1, double end_time) {
    int num_threads = omp_get_max_threads();
    vector<players_in_paint_at_time> temp_players[num_threads];

    const std::vector<int>& paint0_bins = court_bins::get_bins_in_region(paint0);
    const std::vector<int>& paint1_bins = court_bins::get_bins_in_region(paint1);
    std::vector<int> all_bins;
    all_bins.reserve(paint0_bins.size() + paint1_bins.size());
    all_bins.insert(all_bins.end(), paint0_bins.begin(), paint0_bins.end());
    all_bins.insert(all_bins.end(), paint1_bins.begin(), paint1_bins.end());

#pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < all_bins.size()* moment_bins->players_indices_in_bins.size(); i++) {
        // auto start_t = Halide::Tools::benchmark_now();
        int thread_num = omp_get_thread_num();
        // all trajectory starts for the current player
        int bin = all_bins[i / moment_bins->players_indices_in_bins.size()];
        int player_num = i % moment_bins->players_indices_in_bins.size();
        long int player_id = moment_bins->player_ids[player_num];
        for (const player_pointer *p = &moment_bins->player_moment_bins[moment_bins->bin_starts[player_num][bin]];
             p != get_bin_end(moment_bins, player_id, bin); p++) {
            if (((p->x_loc >= paint0.start.x && p->x_loc < paint0.end.x &&
                  p->y_loc >= paint0.start.y && p->y_loc < paint0.end.y) ||
                 (p->x_loc >= paint1.start.x && p->x_loc < paint1.end.x &&
                  p->y_loc >= paint1.start.y && p->y_loc < paint1.end.y)) &&
                 moments->shot_clock[p->moment_index] < end_time) {
                temp_players[thread_num].push_back({p->moment_index, moments->game_clock[p->moment_index], moments->player_id[p->player_index][p->moment_index]});
            }
        }
    }

    for (int i = 0; i < num_threads; i++) {
        for (const auto & elem : temp_players[i]) {
            players_in_paint.push_back(elem);
        }
    }
}
