#include "query_structs.h"

bool operator==(moment const & lhs, moment const & rhs) {
    return
        lhs.team_id == rhs.team_id &&
        lhs.player_id == rhs.player_id &&
        lhs.x_loc == rhs.x_loc &&
        lhs.y_loc == rhs.y_loc &&
        lhs.radius == rhs.radius &&
        lhs.game_clock == rhs.game_clock &&
        lhs.shot_clock == rhs.shot_clock &&
        lhs.quarter == rhs.quarter &&
        lhs.game_id == rhs.game_id &&
        lhs.event_id == rhs.event_id &&
        lhs.moment_in_event == rhs.moment_in_event;
}
