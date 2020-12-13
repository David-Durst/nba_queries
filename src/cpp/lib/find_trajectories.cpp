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

coordinate_range compute_initial_range(vector<moment>& moments) {
    coordinate_range range;
    range.start.x = moments.at(0).x_loc;
    range.start.y = moments.at(0).y_loc;
    range.start.game_clock = moments.at(0).game_clock;
    range.end.x = moments.at(0).x_loc;
    range.end.y = moments.at(0).y_loc;
    range.end.game_clock = moments.at(0).game_clock;
    for (int i = 1; i < (int) moments.size(); i++) {
        range.start.x = std::min(moments.at(i).x_loc, range.start.x);
        range.start.y = std::min(moments.at(i).y_loc, range.start.y);
        // largest game_clock comes first
        range.start.game_clock = std::max(moments.at(i).game_clock, range.start.game_clock);
        range.end.x = std::max(moments.at(i).x_loc, range.end.x);
        range.end.y = std::max(moments.at(i).y_loc, range.end.y);
        range.end.game_clock = std::min(moments.at(i).game_clock, range.end.game_clock);
    }
    return range;
}

void create_moment_index(st_index& index, vector<moment>& moments,
                         vector<int> moments_in_region) {
    // if not enough nodes, contain them all at this layer
    if (moments_in_region.size() < 8) {
        index.values = moments_in_region;
        return;
    }
    // otherwise, collect values for children, find child ranges, and then create children nodes
    // first collecting values for children
    vector<vector<int>> moments_in_children;
    for (int i = 0; i < 2*2*2; i++){
        moments_in_children.push_back(vector<int>{});
        index.children.push_back(st_index{});
    }
    float x_median = (index.cur_range.end.x + index.cur_range.start.x) / 2;
    float y_median = (index.cur_range.end.y + index.cur_range.start.y) / 2;
    float clock_median = (index.cur_range.end.game_clock + index.cur_range.start.game_clock) / 2;
    for (int i = 0; i < (int) moments_in_region.size(); i++) {
        moment& cur_moment = moments.at(i);
        int x_factor = cur_moment.x_loc > x_median ? 2*2 : 0;
        int y_factor = cur_moment.y_loc > y_median ? 2 : 0;
        // larger game clock means earlier
        int clock_factor = cur_moment.game_clock > clock_median ? 0 : 1;
        moments_in_children.at(x_factor + y_factor + clock_factor).push_back(i);
    }

    // now determining ranges of each child
    for (unsigned int c = 0; c < (unsigned int) moments_in_children.size(); c++) {
        vector<int> child_moments = moments_in_children.at(c);

        coordinate_range child_range;
        // x is largest bit
        if ((c & 4) == 4) {
            child_range.start.x = x_median;
            child_range.end.x = index.cur_range.end.x;
        } else {
            child_range.start.x = index.cur_range.start.x;
            child_range.end.x = x_median;
        }
        // y is middle bit
        if ((c & 2) == 2) {
            child_range.start.y = y_median;
            child_range.end.y = index.cur_range.end.y;
        } else {
            child_range.start.y = index.cur_range.start.y;
            child_range.end.y = y_median;
        }
        // x is largest bit
        if ((c & 1) == 1) {
            // since end.game_clock is smaller and start.game_clock is larger
            // this puts children in right order
            child_range.start.game_clock = clock_median;
            child_range.end.game_clock = index.cur_range.end.game_clock;
        } else {
            child_range.start.game_clock = index.cur_range.start.game_clock;
            child_range.end.game_clock = clock_median;
        }
        index.children_ranges.push_back(child_range);
        index.children.at(c).cur_range = child_range;
    }

   // now creating children
   for (unsigned int c = 0; c < (unsigned int) moments_in_children.size(); c++) {
       create_moment_index(index.children.at(c), moments, moments_in_children.at(c));
   }
}

bool range_intersect(coordinate_range r0, coordinate_range r1, bool consider_x, bool consider_y, bool consider_t) {
    bool x_intersects = !consider_x || (r0.start.x >= r1.start.x && r0.start.x <= r1.end.x) ||
            (r0.end.x >= r1.start.x && r0.end.x <= r1.end.x);
    bool y_intersects = !consider_y || (r0.start.y >= r1.start.y && r0.start.y <= r1.end.y) ||
                        (r0.end.y >= r1.start.y && r0.end.y <= r1.end.y);
    bool t_intersects = !consider_t || (r0.start.game_clock >= r1.start.game_clock && r0.start.game_clock <= r1.end.game_clock) ||
                        (r0.end.game_clock >= r1.start.game_clock && r0.end.game_clock <= r1.end.game_clock);
    return x_intersects && y_intersects && t_intersects;
}

void find_trajectories_fixed_origin(vector<moment>& moments, vector<trajectory_data>& trajectories,
                                    coordinate_range origin, coordinate_range destination, float t_offset, float t_delta) {

}
