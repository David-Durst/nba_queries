#include <cmath>
#include <vector>
#include <string>
#include "catch.hpp"
#include "query_structs.h"
#include "check_distances.h"
using std::string;
using std::stringstream;
using std::vector;


// 3 shots, 2 players per shot per team
vector<moment> test_moments = {
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

vector<shot> test_shots = {
    shot{"Turnaround Hook Shot",567,"Made Shot","2015-11-11",7,21500116,
         "Shot Chart Detail","BOS",-4.42490,71.61750,9,1,101161,"Amir Johnson",
         1.00,27,1,4,1,655.590,"2PT Field Goal","Center(C)","In The Paint (Non-RA)",
         "Less Than 8 ft.",1610612738.0,"Boston Celtics","IND"},
    shot{"Jump Shot",548,"Missed Shot","2015-11-11",10,21500116,
         "Shot Chart Detail","BOS",-147.81380,74.91590,9,1,201588,"George Hill",
         1.00,8,1,17,0,635.420,"2PT Field Goal","Left Side(L)","Mid-Range",
         "16-24 ft.",1610612754.0,"Indiana Pacers","IND"},
    shot{"Jump Shot",535,"Missed Shot","2015-11-11",26,21500116,
         "Shot Chart Detail","BOS",191.23700,153.33700,8,1,101161,"Amir Johnson",
         1.00,55,true,25,false,570.130,"3PT Field Goal","Right Side Center(RC)",
         "Above the Break 3","24+ ft.",1610612738.0,"Boston Celtics","IND"}
};

TEST_CASE( "find_nearest_defender_at_each_shot", "[find_nearest_defender_at_each_shot, nearest_defender_query]" ) {
    vector<shot_and_player_data> shots_and_players;
    SECTION( "nearest defender for entire test set" ) {
        shot s = test_shots.at(0);
        moment m = test_moments.at(1);
        vector<moment> result_vector = {test_moments.at(3), test_moments.at(6), test_moments.at(4)};
        find_nearest_defender_at_each_shot(test_moments, test_shots, shots_and_players, 100.0f);
        REQUIRE( shots_and_players.at(0).defense_player_id  == result_vector.at(0).player_id );
        REQUIRE( shots_and_players.at(0).game_clock  == result_vector.at(0).game_clock );
        REQUIRE( shots_and_players.at(1).defense_player_id  == result_vector.at(1).player_id );
        REQUIRE( shots_and_players.at(1).game_clock  == result_vector.at(1).game_clock );
        REQUIRE( shots_and_players.at(2).defense_player_id  == result_vector.at(2).player_id );
        REQUIRE( shots_and_players.at(2).game_clock  == result_vector.at(2).game_clock );
    }
}

TEST_CASE( "get_shooter_team", "[get_shooter_team, nearest_defender_query]" ) {
    SECTION( "basic shooter request" ) {
        shot s = test_shots.at(0);
        moment m = test_moments.at(1);
        REQUIRE( get_shooter_team(test_moments, 0, s) == m );
    }
    SECTION( "on bad shooter request, return last moment" ) {
        shot s = test_shots.at(0);
        s.player_id = 23;
        moment m = test_moments.at(test_moments.size() - 1);
        REQUIRE( get_shooter_team(test_moments, 0, s) == m );
    }
}

TEST_CASE( "get_nearest_defender", "[get_nearest_defender, nearest_defender_query]" ) {
    SECTION( "forwards nearest defender, first shot, same time step" ) {
        int ball_moment_idx = 0;
        moment shooter_moment = test_moments.at(1);
        REQUIRE( get_nearest_defender(test_moments, ball_moment_idx,
                                      shooter_moment, 0.f, true) ==
                 test_moments.at(3) );
    }
    SECTION( "forwards nearest defender, second shot, same time step" ) {
        int ball_moment_idx = 5;
        moment shooter_moment = test_moments.at(8);
        REQUIRE( get_nearest_defender(test_moments, ball_moment_idx,
                                      shooter_moment, 0.f, true) ==
                 test_moments.at(6) );
    }
    SECTION( "backwards nearest defender, third shot, different time step" ) {
        int ball_moment_idx = 10;
        moment shooter_moment = test_moments.at(11);
        REQUIRE( get_nearest_defender(test_moments, ball_moment_idx,
                                      shooter_moment, 100.f, false) ==
                 test_moments.at(4) );
    }
}

TEST_CASE( "compute_distance", "[compute_distance, nearest_defender_query]" ) {
    SECTION( "basic distance test" ) {
        moment m0 = test_moments.at(0);
        moment m1 = test_moments.at(1);
        float predicted_distance = std::sqrt(std::pow(m0.x_loc-m1.x_loc, 2) +
                                             std::pow(m0.y_loc-m1.y_loc, 2));
        REQUIRE( compute_distance(test_moments.at(0), test_moments.at(1)) ==
                 predicted_distance );
    }
}
