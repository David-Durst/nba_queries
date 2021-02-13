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

void Concept::sample(vector<cleaned_moment> samples, const moment_col_store& moments, int64_t num_samples, bool sample_unmerged) {
    std::random_device random_device;
    std::mt19937 random_engine(random_device());
    int64_t num_elements = 0;
    if (sample_unmerged) {
        for (int i = 0; i < MAX_THREADS; i++) {
            num_elements += this->start_moment_index_unmerged[i].size();
        }

        std::uniform_int_distribution<long> thread_distribution(0, MAX_THREADS - 1);
        vector<std::uniform_int_distribution<long>> per_thread_distribution;
        for (int i = 0; i < MAX_THREADS; i++) {
            int64_t thread_max = this->start_moment_index_unmerged[i].size();
            per_thread_distribution.push_back(std::uniform_int_distribution<long>{0, thread_max});
        }
        // the first element of the pair is the thread, the second is the index in that thread's data structure
        set<pair<int, int64_t>> sampled_indices;
        while (sampled_indices.size() < num_samples) {
            int thread_num = thread_distribution(random_engine);
            int64_t per_thread_index = per_thread_distribution[thread_num](random_engine);
            sampled_indices.insert(pair<int, int64_t>{thread_num, per_thread_index});
        }

        for (auto const & window_index : sampled_indices) {
            for (int64_t cur_time = this->start_moment_index_unmerged[window_index.first][window_index.second];
                 cur_time < this->start_moment_index_unmerged[window_index.first][window_index.second] + this->ticks_in_window; cur_time++) {
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
        }
    }
    else {
        std::uniform_int_distribution<long> distribution(0, this->start_moment_index.size() - 1);
        set<int64_t> sampled_indices;
        while (sampled_indices.size() < num_samples) {
            sampled_indices.insert(distribution(random_engine));
        }

        for (auto const & window_index : sampled_indices) {
            for (int64_t cur_time = this->start_moment_index[window_index];
                 cur_time < this->start_moment_index[window_index] + this->ticks_in_window; cur_time++) {
                cleaned_moment c;
                c.ball.team_id = moments.team_id[0][cur_time];
                c.ball.player_id = moments.player_id[0][cur_time];
                c.ball.x_loc = moments.x_loc[0][cur_time];
                c.ball.y_loc = moments.y_loc[0][cur_time];
                c.ball.radius = moments.radius[0][cur_time];

                for (int i = 1; i < NUM_PLAYERS_AND_BALL; i++) {
                    c.players[i-1].team_id = moments.team_id[i][cur_time];
                    c.players[i-1].player_id = moments.player_id[i][cur_time];
                    c.players[i-1].x_loc = moments.x_loc[i][cur_time];
                    c.players[i-1].y_loc = moments.y_loc[i][cur_time];
                    c.players[i-1].radius = moments.radius[i][cur_time];
                }

                c.game_clock = moments.game_clock[cur_time];
                c.shot_clock = moments.shot_clock[cur_time];
                c.quarter = moments.quarter[cur_time];
                c.game_id = moments.game_id[cur_time];
                c.game_num = moments.game_num[cur_time];
                samples.push_back(c);
            }
        }
    }
}

