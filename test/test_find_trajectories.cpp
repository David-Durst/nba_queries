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
        moment{1610612754,201588,3.92816,26.07469,0.00000,655.590,22.930,1,21500116,7,289},
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
        REQUIRE( index.cur_range.start.game_clock == test_moments_trajectories.at(0).game_clock );
        REQUIRE( index.cur_range.end.x == test_moments_trajectories.at(14).x_loc );
        REQUIRE( index.cur_range.end.y == test_moments_trajectories.at(6).y_loc );
        REQUIRE( index.cur_range.end.game_clock == test_moments_trajectories.at(10).game_clock );
    }
}

TEST_CASE( "create_moment_index", "[create_moment_index, trajectories_at_fixed_origin]" ) {
    st_index index;
    vector<int> moments_in_region(15);
    std::iota(moments_in_region.begin(), moments_in_region.end(), 0);
    SECTION( "create moment index" ) {
        index.cur_range = compute_initial_range(test_moments_trajectories);
        create_moment_index(index, test_moments_trajectories, moments_in_region);
        for (int i = 0; i < (int) index.children.size(); i++) {
            for (const auto & j : index.children.at(i).values) {
                REQUIRE( test_moments_trajectories.at(j).x_loc >= index.children.at(i).cur_range.start.x );
                REQUIRE( test_moments_trajectories.at(j).y_loc >= index.children.at(i).cur_range.start.y );
                REQUIRE( test_moments_trajectories.at(j).game_clock <= index.children.at(i).cur_range.start.game_clock );
                REQUIRE( test_moments_trajectories.at(j).x_loc <= index.children.at(i).cur_range.end.x );
                REQUIRE( test_moments_trajectories.at(j).y_loc <= index.children.at(i).cur_range.end.y );
                REQUIRE( test_moments_trajectories.at(j).game_clock >= index.children.at(i).cur_range.end.game_clock );
            }
        }
    }
}

TEST_CASE( "find_trajectories_fixed_origin", "[find_trajectories_fixed_origin, trajectories_at_fixed_origin]" ) {
    st_index index;
    vector<trajectory_data> trajectories;
    vector<int> moments_in_region(15);
    std::iota(moments_in_region.begin(), moments_in_region.end(), 0);
    SECTION( "create moment index" ) {
        index.cur_range = compute_initial_range(test_moments_trajectories);
        create_moment_index(index, test_moments_trajectories, moments_in_region);
        coordinate_range origin{{3.8,26.0,655.5}, {3.95,26.9, 0}};
        coordinate_range destination{{5.2,48.0,635.5}, {5.9,48.9, 0}};
        find_trajectories_fixed_origin(test_moments_trajectories, trajectories, index, origin, destination, 20.0, 2.0);
        REQUIRE( trajectories.at(0).player_id == test_moments_trajectories.at(1).player_id );
        REQUIRE( trajectories.at(0).start_game_clock == test_moments_trajectories.at(1).game_clock );
        REQUIRE( trajectories.at(0).end_game_clock == test_moments_trajectories.at(6).game_clock );
        REQUIRE( trajectories.at(1).player_id == test_moments_trajectories.at(3).player_id );
        REQUIRE( trajectories.at(1).start_game_clock == test_moments_trajectories.at(3).game_clock );
        REQUIRE( trajectories.at(1).end_game_clock == test_moments_trajectories.at(8).game_clock );
    }
}


