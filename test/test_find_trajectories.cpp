#include <cmath>
#include <vector>
#include <string>
#include <numeric>
#include "catch.hpp"
#include "query_structs.h"
#include "find_trajectories.h"
using std::string;
using std::stringstream;
using std::vector;


// 3 shots, 2 players per shot per team
vector<moment> test_moments_trajectories = {
        // first shot
        moment{-1,-1,5.92671,10.85064,3.26341,655.590,22.930,1,21500116,7,289},
        moment{1610612738,101161,3.84763,26.61320,0.00000,655.590,22.930,1,21500116,7,289},
        moment{1610612738,202323,6.76628,41.32569,0.00000,655.590,22.930,1,21500116,7,289},
        moment{1610612754,201588,3.92816,29.07469,0.00000,655.590,22.930,1,21500116,7,289},
        moment{1610612754,101133,83.94442,25.85319,0.00000,655.590,22.930,1,21500116,7,289},

        // second shot
        moment{-1,-1,20.24824,10.47883,7.32415,635.420,13.910,1,21500116,10,214},
        moment{1610612738,101161,5.27648,48.50508,0.00000,635.420,13.910,1,21500116,10,214},
        moment{1610612738,202323,7.47053,34.31218,0.00000,635.420,13.910,1,21500116,10,214},
        moment{1610612754,201588,5.61198,48.11867,0.00000,635.420,13.910,1,21500116,10,214},
        moment{1610612754,101133,17.35359,33.95396,0.00000,635.420,13.910,1,21500116,10,214},

        // third shot
        moment{-1,-1,82.08825,24.55751,4.23344,570.130,11.540,1,21500116,26,269},
        moment{1610612738,101161,83.38536,23.88928,0.00000,570.130,11.540,1,21500116,26,269},
        moment{1610612738,202323,84.46266,24.60095,0.00000,570.130,11.540,1,21500116,26,269},
        moment{1610612754,201588,76.95525,18.10954,0.00000,570.130,11.540,1,21500116,26,269},
        moment{1610612754,101133,84.71030,27.48419,0.00000,570.130,11.540,1,21500116,26,269}
};

TEST_CASE( "compute_initial_range", "[compute_initial_range, trajectories_at_fixed_origin]" ) {
    st_index index;
    vector<int> moments_in_region(15);
    std::iota(moments_in_region.begin(), moments_in_region.end(), 0);
    SECTION( "create moment index" ) {
        index.cur_range = compute_initial_range(test_moments_trajectories);
        REQUIRE( index.cur_range.start.x == test_moments_trajectories.at(1).x_loc );
        REQUIRE( index.cur_range.start.y == test_moments_trajectories.at(5).y_loc );
        REQUIRE( index.cur_range.start.game_clock == test_moments_trajectories.at(10).game_clock );
        REQUIRE( index.cur_range.end.x == test_moments_trajectories.at(14).x_loc );
        REQUIRE( index.cur_range.end.y == test_moments_trajectories.at(6).y_loc );
        REQUIRE( index.cur_range.end.game_clock == test_moments_trajectories.at(0).game_clock );
    }
}

TEST_CASE( "create_moment_index", "[create_moment_index, trajectories_at_fixed_origin]" ) {
    st_index index;
    vector<int> moments_in_region(15);
    std::iota(moments_in_region.begin(), moments_in_region.end(), 0);
    SECTION( "create moment index" ) {
        index.cur_range = compute_initial_range(test_moments_trajectories);
        create_moment_index(index, test_moments_trajectories, moments_in_region);
        int x = 1;
    }
}
