#ifndef FIND_TRAJECTORIES_H
#define FIND_TRAJECTORIES_H
#include <memory>
#include <functional>
#include <string>
#include <vector>
#include "query_structs.h"
using std::vector;
using std::shared_ptr;
using std::reference_wrapper;

struct trajectory_data {
    long int team_id;
    int player_id;
    float start_x_loc;
    float start_y_loc;
    float start_game_clock;
    float end_x_loc;
    float end_y_loc;
    float end_game_clock;
    short int quarter;
} ;

std::ostream& operator<<(std::ostream& os, trajectory_data const& value);
void print_trajectory_csv(std::ostream& os, const trajectory_data& value);

/* For a list of moments from one game, find all the trajectories that are over the specified distances in x,y,t. */
void find_trajectories_no_fixed_origin(vector<moment>& moments, vector<trajectory_data>& trajectories,
        float x_offset=10, float y_offset=10, float t_offset=10);

struct coordinate {
    float x;
    float y;
    float game_clock;
};

struct coordinate_range {
    coordinate start;
    coordinate end;
};

struct st_index {
   vector<st_index> children;
   vector<int> values; // only fill these values in for leaf index nodes
   vector<coordinate_range> children_ranges;
   coordinate_range cur_range;
};

coordinate_range compute_initial_range(vector<moment>& moments);
void create_moment_index(st_index& index, vector<moment>& moments,
                         vector<int> moments_in_region);
void find_trajectories_fixed_origin(vector<moment>& moments, vector<trajectory_data>& trajectories,
                                    coordinate_range origin, coordinate_range destination, float t_offset, float t_delta);

#endif
