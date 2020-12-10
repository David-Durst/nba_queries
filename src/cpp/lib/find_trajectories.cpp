#include <iostream>
#include <vector>
#include "query_structs.h"
#include "find_trajectories.h"
using std::vector;
using std::string;

void find_trajectories_no_fixed_origin(vector<moment>& moments, vector<trajectory_data>& trajectories,
                                       float x_offset, float y_offset, float t_offset) {
    // since using abs, this is really 0.04
    float time_delta = 0.02;
    float space_delta = 0.1;
    int cur_moment_idx = 0;
    int future_moment_idx = 0;
    while (cur_moment_idx < (int) moments.size() && future_moment_idx < (int) moments.size()) {
        const moment& cur_moment = moments.at(cur_moment_idx);
        const moment& future_moment = moments.at(future_moment_idx);
        if (future_moment.quarter == cur_moment.quarter &&
            std::abs(cur_moment.game_clock - t_offset - future_moment.game_clock) < time_delta &&
            cur_moment.player_id == future_moment.player_id) {
            if (std::abs(cur_moment.x_loc + x_offset - future_moment.x_loc) < space_delta &&
                std::abs(cur_moment.y_loc + y_offset - future_moment.y_loc) < space_delta) {
                trajectory_data result = {cur_moment.team_id, cur_moment.player_id, cur_moment.x_loc, cur_moment.y_loc,
                                          cur_moment.game_clock, future_moment.x_loc, future_moment.y_loc, future_moment.game_clock,
                                          cur_moment.quarter};
                // filter out repeated entries due to multiple events at same time step
                if (trajectories.size() == 0 || trajectories.at(trajectories.size()-1).start_game_clock != result.start_game_clock) {
                    trajectories.push_back(result);
                }
            }
            cur_moment_idx++;
            future_moment_idx++;
        }
        // increment cur_moment if
        // 1. it's in a quarter before future_moment
        // 2. it's more than t_offset behind in current quarter than future_moment
        // 3. it's within the t_delta, but it's player_id is less than future_moment's
        else if (future_moment.quarter > cur_moment.quarter ||
                (future_moment.quarter == cur_moment.quarter && cur_moment.game_clock - t_offset - future_moment.game_clock >= time_delta) ||
                (future_moment.quarter == cur_moment.quarter && std::abs(cur_moment.game_clock - t_offset - future_moment.game_clock) < time_delta
                    && cur_moment.player_id < future_moment.player_id)) {
            cur_moment_idx++;
        }
        // increment future_moment if
        // 1. cur_moment is less than t_offset behind in current quarter than future moment
        // 2. cur_moment is within the t_delta, but cur_moments's player_id is greater than future_moment's
        else if ((future_moment.quarter == cur_moment.quarter && cur_moment.game_clock - t_offset - future_moment.game_clock <= -time_delta) ||
                 (future_moment.quarter == cur_moment.quarter && std::abs(cur_moment.game_clock - t_offset - future_moment.game_clock) < time_delta
                  && cur_moment.player_id > future_moment.player_id)) {
            future_moment_idx++;
        }
    }
}

std::ostream& operator<<(std::ostream& os, trajectory_data const& value) {
    os << "team_id: " << value.team_id
       << ", player_id: " << value.player_id
       << ", start_x_loc: " << value.start_x_loc
       << ", start_y_loc: " << value.start_y_loc
       << ", start_game_clock: " << value.start_game_clock
       << ", end_x_loc: " << value.end_x_loc
       << ", end_y_loc: " << value.end_y_loc
       << ", end_game_clock: " << value.end_game_clock
       << ", quarter: " << value.quarter;
    return os;
}

void print_trajectory_csv(std::ostream& os, const trajectory_data& value) {
    os << value.team_id
       << "," << value.player_id
       << "," << value.start_x_loc
       << "," << value.start_y_loc
       << "," << value.start_game_clock
       << "," << value.end_x_loc
       << "," << value.end_y_loc
       << "," << value.end_game_clock
       << "," << value.quarter;
}
