#include <iostream>
#include <vector>
#include <deque>
#include "query_structs.h"
#include "find_trajectories.h"
using std::vector;
using std::deque;
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

void create_moment_index(st_index& index, vector<moment>& moments, vector<int> moments_in_region, int max_bucket_size) {
    // if not enough nodes, contain them all at this layer
    if (moments_in_region.size() < max_bucket_size) {
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
    for (const auto & i : moments_in_region) {
        moment& cur_moment = moments.at(i);
        int x_factor = cur_moment.x_loc > x_median ? 2*2 : 0;
        int y_factor = cur_moment.y_loc > y_median ? 2 : 0;
        // larger game clock means earlier
        int clock_factor = cur_moment.game_clock > clock_median ? 0 : 1;
        moments_in_children.at(x_factor + y_factor + clock_factor).push_back(i);
    }

    // if all points are the same, stop, clear the children, and make them all values at this layer
    for (int i = 0; i < moments_in_children.size(); i++) {
        // first check if all same by seeing if all assigned to same bucker
        if (moments_in_children.at(i).size() == moments_in_region.size()) {
            bool all_moments_same = true;
            for (const auto & j : moments_in_children.at(i)) {
                // all same if all equal to first moment in this region
                if (moments.at(j).x_loc != moments.at(moments_in_region.at(0)).x_loc ||
                    moments.at(j).y_loc != moments.at(moments_in_region.at(0)).y_loc ||
                    moments.at(j).game_clock != moments.at(moments_in_region.at(0)).game_clock) {
                    all_moments_same = false;
                    break;
                }
            }
            if (all_moments_same) {
                index.children.clear();
                index.values = moments_in_region;
                return;
            }
        }
    }

    // now determining ranges of each child
    for (unsigned int c = 0; c < (unsigned int) moments_in_children.size(); c++) {
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
    bool t_intersects = !consider_t || (r0.start.game_clock <= r1.start.game_clock && r0.start.game_clock >= r1.end.game_clock) ||
                        (r0.end.game_clock <= r1.start.game_clock && r0.end.game_clock >= r1.end.game_clock);
    return x_intersects && y_intersects && t_intersects;
}

bool point_intersect(coordinate_range r, coordinate c, bool consider_x, bool consider_y, bool consider_t) {
    bool x_intersects = !consider_x || (c.x >= r.start.x && c.x <= r.end.x);
    bool y_intersects = !consider_y || (c.y >= r.start.y && c.y <= r.end.y);
    bool t_intersects = !consider_t || (c.game_clock <= r.start.game_clock && c.game_clock >= r.end.game_clock);
    return x_intersects && y_intersects && t_intersects;
}

void traverse_index_for_points_in_range(vector<moment> &moments, coordinate_range &search_range,
                                        st_index& index, vector<int> &values_in_range, bool consider_t) {
    deque<reference_wrapper<st_index>> origin_frontier = {index};
    while (!origin_frontier.empty()) {
        st_index& cur_index = origin_frontier.front();
        origin_frontier.pop_front();
        // if this st_index has no children, it's a leaf so check all values if in search_range
        if (cur_index.children.empty()) {
            for (const auto & i : cur_index.values) {
                moment& cur_moment = moments.at(i);
                if (point_intersect(search_range, {cur_moment.x_loc, cur_moment.y_loc, cur_moment.game_clock},
                                    true, true, consider_t)) {
                    values_in_range.push_back(i);
                }
            }
        }
        // otherwise, look at all children that intersect search_range
        else {
            for (int c = 0; c < cur_index.children_ranges.size(); c++) {
                if (range_intersect(search_range, cur_index.children_ranges.at(c), true, true, consider_t)) {
                    origin_frontier.push_back(cur_index.children.at(c));
                }
            }
        }
    }
}

void find_trajectories_fixed_origin(vector<moment>& moments, vector<trajectory_data>& trajectories, st_index& index,
                                    coordinate_range origin, coordinate_range destination, float t_offset, float t_delta) {
    // find valid starting points for trajectories
    vector<int> trajectory_starts;
    traverse_index_for_points_in_range(moments, origin, index, trajectory_starts, false);

    // for each possible trajectory starting point, if you can find at least one ending point, then add it to trajectories
    for (const auto & trajectory_start : trajectory_starts) {
        vector<int> trajectory_ends;
        coordinate_range destination_with_time_set = destination;
        destination_with_time_set.start.game_clock = moments.at(trajectory_start).game_clock - t_offset + t_delta;
        destination_with_time_set.end.game_clock = moments.at(trajectory_start).game_clock - t_offset - t_delta;
        traverse_index_for_points_in_range(moments, destination_with_time_set, index, trajectory_ends, true);
        // if one trajectory end has same player id and quarter as start, then add this trajectory to list and move on to next start
        for (const auto & trajectory_end : trajectory_ends) {
            moment& start_moment = moments.at(trajectory_start);
            moment& end_moment = moments.at(trajectory_end);
            if (start_moment.player_id == end_moment.player_id && start_moment.quarter == end_moment.quarter) {
                trajectories.push_back({start_moment.team_id, start_moment.player_id, start_moment.x_loc, start_moment.y_loc,
                                        start_moment.game_clock, end_moment.x_loc, end_moment.y_loc, end_moment.game_clock,
                                        start_moment.quarter});
                break;
            }
        }
    }
}
