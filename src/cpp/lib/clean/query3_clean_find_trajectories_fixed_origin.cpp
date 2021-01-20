#include "clean_queries.h"
#include <functional>
#include <iostream>

void find_trajectories_fixed_origin_clean(moment_col_store * moments, list<trajectory_data> * trajectories,
                                          coordinate_range origin, coordinate_range destination,
                                          int t_offset, int t_delta_ticks, bool parallel) {
    int t_index_offset = t_offset * 25;
    #pragma omp parallel for if(parallel)
    for (int64_t src_time = 0; src_time < moments->size - t_index_offset + t_delta_ticks; src_time++) {
        bool players_match_src[] = {false,false,false,false,false,false,false,false,false,false,false,false};
        bool any_match = false;
        for (int j = 0; j < 11; j++) {
            players_match_src[j] =
                    point_intersect_no_time(&origin, moments->x_loc[j][src_time], moments->y_loc[j][src_time]);
            any_match |= players_match_src[j];
        }
        if (!any_match) {
            continue;
        }
        for (int64_t dst_time = src_time + t_index_offset - t_delta_ticks;
            dst_time < src_time + t_index_offset + t_delta_ticks + 1; dst_time++) {
            if (dst_time < 0 || dst_time > moments->size) {
                continue;
            }
            for (int src_player_index = 0; src_player_index < 11; src_player_index++) {
                if (!players_match_src[src_player_index]) {
                    continue;
                }
                for (int dst_player_index = 0; dst_player_index < 11; dst_player_index++) {
                    if (moments->player_id[src_player_index][src_time] == moments->player_id[dst_player_index][dst_time] &&
                        moments->game_num[src_time] == moments->game_num[dst_time] &&
                        moments->quarter[src_time] == moments->quarter[dst_time] &&
                        point_intersect_no_time(&destination, moments->x_loc[dst_player_index][dst_time],
                                                moments->y_loc[dst_player_index][dst_time])) {
                        #pragma omp critical
                        {
                            trajectories->append_node({
                                                              moments->game_id[src_time],
                                                              moments->game_num[src_time],
                                                              moments->team_id[src_player_index][src_time],
                                                              moments->player_id[src_player_index][src_time],
                                                              moments->x_loc[src_player_index][src_time],
                                                              moments->y_loc[src_player_index][src_time],
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

void find_trajectories_fixed_origin_clean_rowstore(vector<cleaned_moment>& moments, vector<trajectory_data>& trajectories,
                                                   coordinate_range origin, coordinate_range destination,
                                                   int t_offset, int t_delta_ticks) {
    int t_index_offset = t_offset * 25;
    vector<std::reference_wrapper<player_data>> src_players, dst_players;
    for (size_t i = 0; i < moments.size() - t_index_offset + t_delta_ticks; i++) {
        cleaned_moment& src_m = moments.at(i);
        get_all_player_data(src_players, src_m);
        src_players.erase(std::remove_if(src_players.begin(), src_players.end(), [origin](player_data p){
            return !point_intersect_no_time(origin, p);
        }), src_players.end());
        if (src_players.empty()) {
            continue;
        }
        for (int j = t_index_offset - t_delta_ticks; j < t_index_offset + t_delta_ticks + 1; j++) {
            if (i + j >= moments.size()) {
                continue;
            }
            cleaned_moment& dst_m = moments.at(i+j);
            for (const auto& p : src_players) {
                get_all_player_data(dst_players, dst_m);
                for (const auto& q : dst_players) {
                    if (p.get().player_id == q.get().player_id &&
                        src_m.game_num == dst_m.game_num &&
                        src_m.quarter == dst_m.quarter &&
                        point_intersect_no_time(destination, q)) {
                        trajectories.push_back({
                                                    src_m.game_id,
                                                    src_m.game_num,
                                                    p.get().team_id,
                                                    p.get().player_id,
                                                    p.get().x_loc,
                                                    p.get().y_loc,
                                                    src_m.game_clock.to_double(),
                                                    q.get().x_loc,
                                                    q.get().y_loc,
                                                    dst_m.game_clock.to_double(),
                                                    src_m.quarter
                                               });
                        continue;
                    }
                }
            }
        }
    }
}

