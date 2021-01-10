#include "clean_find_trajectories.h"
#include <functional>
#include <iostream>

void find_trajectories_fixed_origin_clean(moment_col_store * moments, list<trajectory_data> * trajectories,
                                          coordinate_range origin, coordinate_range destination,
                                          int t_offset, int t_delta_ticks) {
    int t_index_offset = t_offset * 25;
    //cdef vector[player_data] src_players_unfiltered, src_players, dst_players
    bool players_match_src[] = {false,false,false,false,false,false,false,false,false,false,false};
    for (size_t src_time = 0; src_time < moments->size - t_index_offset + t_delta_ticks; src_time++) {
        for (int j = 1; j < 11; j++) {
            players_match_src[j-1] =
                    point_intersect_no_time(&origin, moments->x_loc[j][src_time], moments->y_loc[j][src_time]);
        }
        for (size_t dst_time = src_time + t_offset - t_delta_ticks;
            dst_time < src_time + t_offset + t_delta_ticks + 1; dst_time++) {
            if (dst_time > moments->size) {
                continue;
            }
            for (int src_player_index = 1; src_player_index < 11; src_player_index++) {
                if (!players_match_src[src_player_index-1]) {
                    continue;
                }
                for (int dst_player_index = 1; dst_player_index < 11; dst_player_index++) {
                    if (moments->player_id[src_player_index][src_time] == moments->player_id[dst_player_index][dst_time] &&
                        point_intersect_no_time(&destination, moments->x_loc[dst_player_index][dst_time],
                                                moments->y_loc[dst_player_index][src_time])) {
                        trajectories->append_node({
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
                        continue;
                    }
                }
            }
        }
    }

}


bool point_intersect_no_time(coordinate_range * r, double x_loc, double y_loc) {
    bool x_intersects = x_loc >= r->start.x && x_loc <= r->end.x;
    bool y_intersects = y_loc >= r->start.y && y_loc <= r->end.y;
    return x_intersects && y_intersects;
}


void find_trajectories_fixed_origin_clean_rowstore(vector<cleaned_moment>& moments, vector<trajectory_data>& trajectories,
                                                   coordinate_range origin, coordinate_range destination,
                                                   int t_offset, int t_delta_ticks) {
    int t_index_offset = t_offset * 25;
    vector<std::reference_wrapper<player_data>> src_players, dst_players;
    //cdef vector[player_data] src_players_unfiltered, src_players, dst_players
    for (size_t i = 0; i < moments.size() - t_index_offset + t_delta_ticks; i++) {
        cleaned_moment& src_m = moments.at(i);
        get_all_player_data(src_players, src_m);
        if (i == 602) {
            int x = 2;
        }
        src_players.erase(std::remove_if(src_players.begin(), src_players.end(), [origin](player_data p){
            return !point_intersect_no_time(origin, p);
        }), src_players.end());
        if (src_players.empty()) {
            continue;
        }
        for (int j = t_offset - t_delta_ticks; j < t_offset + t_delta_ticks + 1; j++) {
            if (i + j > moments.size()) {
                continue;
            }
            cleaned_moment& dst_m = moments.at(i+j);
            for (const auto& p : src_players) {
                get_all_player_data(dst_players, dst_m);
                for (const auto& q : dst_players) {
                    if (p.get().player_id == q.get().player_id && point_intersect_no_time(destination, q)) {
                        trajectories.push_back({
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


bool point_intersect_no_time(const coordinate_range& r, const player_data& c) {
    bool x_intersects = c.x_loc >= r.start.x && c.x_loc <= r.end.x;
    bool y_intersects = c.y_loc >= r.start.y && c.y_loc <= r.end.y;
    return x_intersects && y_intersects;
}