#include "concepts.h"
#include "col_stores.h"
#include "clean_queries.h"
#include <vector>
#include <omp.h>
#include <random>
#include <set>
#include <utility>
#include <string>
#include <fstream>
using std::vector;
using std::set;
using std::pair;
using std::string;

void print_sample_window_csv(std::ostream& os, const SampleWindow& sw) {
    os << START_SAMPLE_WINDOW_TOKEN << "," << sw.sample_length
        << "," << sw.window_length
        << "," << sw.window_start
        << "," << sw.window_html
        << std::endl;
    for (const auto & c : sw.moments_in_sample) {
        print_cleaned_moment_csv(os, c);
        os << std::endl;
    }
}

void add_sample_tick(const moment_col_store &moments, vector<cleaned_moment> &samples, int64_t cur_time) {
    cleaned_moment c;
    c.ball.team_id = moments.team_id[0][cur_time];
    c.ball.player_id = moments.player_id[0][cur_time];
    c.ball.x_loc = moments.x_loc[0][cur_time];
    c.ball.y_loc = moments.y_loc[0][cur_time];
    c.ball.radius = moments.radius[0][cur_time];

    for (int i = 1; i < NUM_PLAYERS_AND_BALL; i++) {
        c.players[i - 1].team_id = moments.team_id[i][cur_time];
        c.players[i - 1].player_id = moments.player_id[i][cur_time];
        c.players[i - 1].x_loc = moments.x_loc[i][cur_time];
        c.players[i - 1].y_loc = moments.y_loc[i][cur_time];
        c.players[i - 1].radius = moments.radius[i][cur_time];
    }

    c.game_clock = moments.game_clock[cur_time];
    c.shot_clock = moments.shot_clock[cur_time];
    c.quarter = moments.quarter[cur_time];
    c.game_id = moments.game_id[cur_time];
    c.game_num = moments.game_num[cur_time];

    samples.push_back(c);
}

void Concept::sample(const moment_col_store& moments, int64_t num_samples, bool sample_unmerged, string sample_file_path,
                     const std::function<bool(const moment_col_store&, bool, int64_t, int64_t, int64_t)> &filter) {
    std::random_device random_device;
    std::mt19937 random_engine(random_device());
    vector<SampleWindow> samples;
    int num_threads = omp_get_max_threads();
    if (sample_unmerged) {
        std::uniform_int_distribution<long> thread_distribution(0, num_threads - 1);
        vector<std::uniform_int_distribution<long>> per_thread_distribution;
        for (int i = 0; i < num_threads; i++) {
            int64_t thread_max = start_moment_index_unmerged[i].size();
            per_thread_distribution.push_back(std::uniform_int_distribution<long>{0, thread_max});
        }
        // the first element of the pair is the thread, the second is the index in that thread's data structure
        set<pair<int, int64_t>> sampled_indices;
        int64_t num_samples_tried = 0;
        while (sampled_indices.size() < num_samples) {
            int thread_num = thread_distribution(random_engine);
            int64_t per_thread_index = per_thread_distribution[thread_num](random_engine);
            if (filter(moments, true, thread_num, per_thread_index, start_moment_index_unmerged[thread_num][per_thread_index])) {
                sampled_indices.insert(pair<int, int64_t>{thread_num, per_thread_index});
            }
            num_samples_tried++;
            if (num_samples_tried > 100000) {
                std::cout << "tried 100000 samples and didn't get "  << num_samples << " so stopping with " << sampled_indices.size() << " elements" << std::endl;
                break;
            }
        }

        for (auto const & window_index : sampled_indices) {
            SampleWindow sw;
            sw.sample_length = 0;
            sw.window_length = ticks_in_window;
            sw.window_html = get_concept_html_unmerged(moments, window_index.first, window_index.second);
            for (int64_t cur_time = start_moment_index_unmerged[window_index.first][window_index.second] - buffer_ticks_for_sample;
                 cur_time < start_moment_index_unmerged[window_index.first][window_index.second] + ticks_in_window + buffer_ticks_for_sample;
                 cur_time++) {
                // skip bad entries
                if (cur_time < 0 || cur_time >= moments.size) {
                    continue;
                }
                sw.sample_length++;
                if (cur_time == start_moment_index_unmerged[window_index.first][window_index.second]) {
                    sw.window_start = sw.sample_length - 1;
                }
                add_sample_tick(moments, sw.moments_in_sample, cur_time);
            }
            samples.push_back(sw);
        }
    }
    else {
        std::uniform_int_distribution<long> distribution(0, num_windows - 1);
        set<int64_t> sampled_indices;
        int64_t num_samples_tried = 0;
        while (sampled_indices.size() < num_samples) {
            int64_t index = distribution(random_engine);
            if (filter(moments, false, index, 0, start_moment_index[index])) {
                sampled_indices.insert(index);
            }
            num_samples_tried++;
            if (num_samples_tried > 100000) {
                std::cout << "tried 100000 samples and didn't get "  << num_samples << " so stopping with " << sampled_indices.size() << " elements" << std::endl;
                break;
            }
        }

        for (auto const & window_index : sampled_indices) {
            SampleWindow sw;
            sw.sample_length = 0;
            sw.window_length = ticks_in_window;
            sw.window_html = get_concept_html(moments, window_index);
            for (int64_t cur_time = start_moment_index[window_index] - buffer_ticks_for_sample;
                 cur_time < start_moment_index[window_index] + ticks_in_window + buffer_ticks_for_sample; cur_time++) {
                // skip bad entries
                if (cur_time < 0 || cur_time >= moments.size) {
                    continue;
                }
                sw.sample_length++;
                if (cur_time == start_moment_index[window_index]) {
                    sw.window_start = sw.sample_length - 1;
                }
                add_sample_tick(moments, sw.moments_in_sample, cur_time);
            }
            samples.push_back(sw);
        }
    }


    std::cout << "writing samples to " << sample_file_path << std::endl;
    std::fstream sample_file;
    sample_file.open(sample_file_path, std::fstream::out | std::fstream::trunc);
    print_cleaned_moment_csv_header(sample_file);
    for (const auto & w : samples) {
        print_sample_window_csv(sample_file, w);
    }
    sample_file.close();
}


