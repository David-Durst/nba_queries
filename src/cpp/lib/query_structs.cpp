#include <iostream>
#include "query_structs.h"

bool operator==(moment const& lhs, moment const& rhs) {
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

std::ostream& operator<<(std::ostream& os, const moment& value) {
    os << "team_id: " << value.team_id
         << ", player_id: " << value.player_id
         << ", x_loc: " << value.x_loc
         << ", y_loc: " << value.y_loc
         << ", radius: " << value.radius
         << ", game_clock: " << value.game_clock
         << ", shot_clock: " << value.shot_clock
         << ", quarter: " << value.quarter
         << ", game_id: " << value.game_id
         << ", event_id: " << value.event_id
         << ", moment_in_event: " << value.moment_in_event;
    return os;
}
