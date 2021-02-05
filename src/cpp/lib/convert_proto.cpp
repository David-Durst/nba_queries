/*#include "convert_proto.h"

void moments_memory_to_proto(vector<cleaned_moment>& internal_moments, proto::Moments& proto_moments) {
    for (const auto & internal_moment : internal_moments) {
        proto::Moment * proto_moment = proto_moments.add_elements();

        // set ball
        proto::PlayerData * proto_ball = proto_moment->mutable_ball();
        proto_ball->set_team_id(internal_moment.ball.team_id);
        proto_ball->set_player_id(internal_moment.ball.player_id);
        proto_ball->set_x_loc(internal_moment.ball.x_loc);
        proto_ball->set_y_loc(internal_moment.ball.y_loc);
        proto_ball->set_radius(internal_moment.ball.radius);

        // set players
        for (int i = 0; i < 10; i++) {
            proto::PlayerData * proto_player = proto_moment->add_players();
            proto_player->set_team_id(internal_moment.players[i].team_id);
            proto_player->set_player_id(internal_moment.players[i].player_id);
            proto_player->set_x_loc(internal_moment.players[i].x_loc);
            proto_player->set_y_loc(internal_moment.players[i].y_loc);
        }

        proto::ClockFixedPoint * proto_game_clock = proto_moment->mutable_game_clock();
        proto_game_clock->set_seconds(internal_moment.game_clock.seconds);
        proto_game_clock->set_twenty_fifths_of_second(internal_moment.game_clock.twenty_fifths_of_second);

        proto_moment->set_shot_clock(internal_moment.shot_clock);
        proto_moment->set_quarter(internal_moment.quarter);
        proto_moment->set_game_id(internal_moment.game_id);
        proto_moment->set_game_num(internal_moment.game_num);

        // set moments
        for (const auto & internal_emd : internal_moment.events) {
            proto::EventMomentData * emd = proto_moment->add_events();
            emd->set_event_id(internal_emd.event_id);
            emd->set_moment_in_event(internal_emd.moment_in_event);
        }
    }
}

void moments_proto_to_memory(proto::Moments& proto_moments, vector<cleaned_moment>& internal_moments) {
    for (const auto & proto_moment : proto_moments.elements()) {
        cleaned_moment internal_moment;
        internal_moment.ball.team_id = proto_moment.ball().team_id();
        internal_moment.ball.player_id = proto_moment.ball().player_id();
        internal_moment.ball.x_loc = proto_moment.ball().x_loc();
        internal_moment.ball.y_loc = proto_moment.ball().y_loc();
        internal_moment.ball.radius = proto_moment.ball().radius();

        for (int i = 0; i < 10; i++) {
            internal_moment.players[i].team_id = proto_moment.players(i).team_id();
            internal_moment.players[i].player_id = proto_moment.players(i).player_id();
            internal_moment.players[i].x_loc = proto_moment.players(i).x_loc();
            internal_moment.players[i].y_loc = proto_moment.players(i).y_loc();
            internal_moment.players[i].radius = proto_moment.players(i).radius();
        }

        internal_moment.game_clock.seconds = proto_moment.game_clock().seconds();
        internal_moment.game_clock.twenty_fifths_of_second = proto_moment.game_clock().twenty_fifths_of_second();

        internal_moment.shot_clock = proto_moment.shot_clock();
        internal_moment.quarter = proto_moment.quarter();
        internal_moment.game_id = proto_moment.game_id();
        internal_moment.game_num = proto_moment.game_num();

        for (const auto & proto_emd : proto_moment.events()) {
            event_moment_data emd;
            emd.event_id = proto_emd.event_id();
            emd.moment_in_event = proto_emd.moment_in_event();
        }

        internal_moments.push_back(internal_moment);
    }
}*/