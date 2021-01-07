#include "clean_find_trajectories.h"
#include <functional>

void find_trajectories_fixed_origin_clean(vector<cleaned_moment>& moments, vector<trajectory_data>& trajectories,
                                          coordinate_range origin, coordinate_range destination,
                                          int t_offset, int t_delta_ticks) {
    int t_index_offset = t_offset * 25;
    vector<std::reference_wrapper<player_data>> src_players, dst_players;
    //cdef vector[player_data] src_players_unfiltered, src_players, dst_players
    for (size_t i = 0; i < moments.size() - t_index_offset + t_delta_ticks; i++) {
        cleaned_moment& src_m = moments.at(i);
        get_all_player_data(src_players, src_m);
        src_players.erase(std::remove_if(src_players.begin(), src_players.end(), [origin](player_data p){
            return point_intersect_no_time(origin, p);
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
    bool x_intersects = c.x_loc >= r.start.x and c.x_loc <= r.end.x;
    bool y_intersects = c.y_loc >= r.start.y and c.y_loc <= r.end.y;
    return x_intersects and y_intersects;
}
