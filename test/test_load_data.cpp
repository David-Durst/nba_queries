#include <vector>
#include <string>
#include <sstream>
#include "catch.hpp"
#include "query_structs.h"
#include "load_data.h"
using std::string;
using std::stringstream;

string test_moments = R"(team_id,player_id,x_loc,y_loc,radius,game_clock,shot_clock,quarter,game_id,event_id,moment_in_event
-1,-1,46.99036,25.24731,5.6787,720.0,24.0,1,0021500116,1,0
1610612738,101161,45.50782,26.43745,0.0,720.0,24.0,1,0021500116,1,0
1610612738,202323,50.36068,26.11638,0.0,720.0,24.0,1,0021500116,1,0
)";

string one_moment = "1610612738,101161,45.50782,26.43745,0.0,720.0,24.0,1,0021500116,1,0";

TEST_CASE( "one row of load_moment_row", "[load_moment_row, loading]" ) {
    moment m;
    load_moment_row(one_moment, m);
    SECTION( "team_id" ) {
        REQUIRE( m.team_id == 1610612738);
    }
    SECTION( "player_id" ) {
        REQUIRE( m.player_id == 101161);
    }
    SECTION( "x_loc" ) {
        REQUIRE( m.x_loc == 45.50782f);
    }
    SECTION( "moment_in_event" ) {
        REQUIRE( m.moment_in_event == 0);
    }
}

TEST_CASE( "many rows of load_moment_rows", "[load_moment_rows, loading]" ) {
    vector<moment> ms;
    stringstream ss(test_moments);
    load_moment_rows(ss, ms);
    SECTION( "team_id" ) {
        REQUIRE( ms.at(0).team_id == -1);
        REQUIRE( ms.at(1).team_id == 1610612738);
        REQUIRE( ms.at(2).team_id == 1610612738);
    }
    SECTION( "player_id" ) {
        REQUIRE( ms.at(0).player_id == -1);
        REQUIRE( ms.at(1).player_id == 101161);
        REQUIRE( ms.at(2).player_id == 202323);
    }
    SECTION( "x_loc" ) {
        REQUIRE( ms.at(0).x_loc == 46.99036f);
        REQUIRE( ms.at(1).x_loc == 45.50782f);
        REQUIRE( ms.at(2).x_loc == 50.36068f);
    }
    SECTION( "moment_in_event" ) {
        REQUIRE( ms.at(0).moment_in_event == 0);
        REQUIRE( ms.at(1).moment_in_event == 0);
        REQUIRE( ms.at(2).moment_in_event == 0);
    }
}

string test_events = R"(GAME_ID,EVENTNUM,EVENTMSGTYPE,EVENTMSGACTIONTYPE,PERIOD,WCTIMESTRING,PCTIMESTRING,HOMEDESCRIPTION,NEUTRALDESCRIPTION,VISITORDESCRIPTION,SCORE,SCOREMARGIN,PERSON1TYPE,PLAYER1_ID,PLAYER1_NAME,PLAYER1_TEAM_ID,PLAYER1_TEAM_CITY,PLAYER1_TEAM_NICKNAME,PLAYER1_TEAM_ABBREVIATION,PERSON2TYPE,PLAYER2_ID,PLAYER2_NAME,PLAYER2_TEAM_ID,PLAYER2_TEAM_CITY,PLAYER2_TEAM_NICKNAME,PLAYER2_TEAM_ABBREVIATION,PERSON3TYPE,PLAYER3_ID,PLAYER3_NAME,PLAYER3_TEAM_ID,PLAYER3_TEAM_CITY,PLAYER3_TEAM_NICKNAME,PLAYER3_TEAM_ABBREVIATION
0021500116,0,12,0,1,7:41 PM,12:00,,,,,,0,0,,,,,,0,0,,,,,,0,0,,,,,
0021500116,1,10,0,1,7:41 PM,12:00,Jump Ball Johnson vs. Mahinmi: Tip to Miles,,,,,4,101161,Amir Johnson,1610612738.0,Boston,Celtics,BOS,5,101133,Ian Mahinmi,1610612754.0,Indiana,Pacers,IND,5,101139,CJ Miles,1610612754.0,Indiana,Pacers,IND
0021500116,2,1,75,1,7:42 PM,11:39,,,Ellis 2' Driving Finger Roll Layup (2 PTS),2 - 0,-2,5,101145,Monta Ellis,1610612754.0,Indiana,Pacers,IND,0,0,,,,,,0,0,,,,,
)";

string one_event = "0021500116,1,10,0,1,7:41 PM,12:00,Jump Ball Johnson vs. Mahinmi: Tip to Miles,,,,,4,101161,Amir Johnson,1610612738.0,Boston,Celtics,BOS,5,101133,Ian Mahinmi,1610612754.0,Indiana,Pacers,IND,5,101139,CJ Miles,1610612754.0,Indiana,Pacers,IND";


TEST_CASE( "one row of load_event_row", "[load_event_row, loading]" ) {
    event e;
    load_event_row(one_event, e);
    SECTION( "game_id" ) {
        REQUIRE( e.game_id == 21500116l);
    }
    SECTION( "period" ) {
        REQUIRE( e.period == 1);
    }
    SECTION( "wc_timestring" ) {
        REQUIRE( e.wc_timestring.compare("7:41 PM") == 0);
    }
    SECTION( "player3_team_abbreviation" ) {
        REQUIRE( e.player3_team_abbreviation.compare("IND") == 0);
    }
}

TEST_CASE( "many rows of load_event_rows", "[load_event_rows, loading]" ) {
    vector<event> es;
    stringstream ss(test_events);
    load_event_rows(ss, es);
    SECTION( "size" ) {
        REQUIRE( es.size() == 3 );
    }
    SECTION( "player3_team_abbreviation" ) {
        REQUIRE( es.at(0).player3_team_abbreviation.compare("") == 0);
        REQUIRE( es.at(1).player3_team_abbreviation.compare("IND") == 0);
        REQUIRE( es.at(2).player3_team_abbreviation.compare("") == 0);
    }
}

string test_shots = R"(ACTION_TYPE,EVENTTIME,EVENT_TYPE,GAME_DATE,GAME_EVENT_ID,GAME_ID,GRID_TYPE,HTM,LOC_X,LOC_Y,MINUTES_REMAINING,PERIOD,PLAYER_ID,PLAYER_NAME,QUARTER,SECONDS_REMAINING,SHOT_ATTEMPTED_FLAG,SHOT_DISTANCE,SHOT_MADE_FLAG,SHOT_TIME,SHOT_TYPE,SHOT_ZONE_AREA,SHOT_ZONE_BASIC,SHOT_ZONE_RANGE,TEAM_ID,TEAM_NAME,VTM
Jump Shot,215,Missed Shot,20151223,93,0021500435,Shot Chart Detail,LAL,-23.9825,157.0968,3,1,101138,Brandon Bass,1.0,35,1,15,0,218.5,2PT Field Goal,Center(C),Mid-Range,8-16 ft.,1610612747,Los Angeles Lakers,OKC
Dunk Shot,191,Made Shot,20151223,97,0021500435,Shot Chart Detail,LAL,224.3221,24.9662,3,1,101138,Brandon Bass,1.0,11,1,0,1,204.83,2PT Field Goal,Center(C),Restricted Area,Less Than 8 ft.,1610612747,Los Angeles Lakers,OKC
Jump Shot,518,Made Shot,20151223,164,0021500435,Shot Chart Detail,LAL,-5.4122,173.0601,8,2,101138,Brandon Bass,2.0,38,1,16,1,520.65,2PT Field Goal,Center(C),Mid-Range,16-24 ft.,1610612747,Los Angeles Lakers,OKC
)";

string one_shot = "Jump Shot,215,Missed Shot,20151223,93,0021500435,Shot Chart Detail,LAL,-23.9825,157.0968,3,1,101138,Brandon Bass,1.0,35,1,15,0,218.5,2PT Field Goal,Center(C),Mid-Range,8-16 ft.,1610612747,Los Angeles Lakers,OKC";
