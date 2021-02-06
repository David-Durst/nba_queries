#include "convert_proto.h"
#include <capnp/message.h>
#include <capnp/serialize.h>
#include <fcntl.h>
#include <unistd.h>

int64_t SEGMENT_SIZE = 10000000;
void moments_memory_to_proto(vector<cleaned_moment>& internal_moments, string proto_file_path) {
    ::capnp::MallocMessageBuilder message;

    Moments::Builder proto_moments = message.initRoot<Moments>();
    int64_t num_segments = internal_moments.size() / SEGMENT_SIZE;
    int64_t last_segment_size = internal_moments.size() % SEGMENT_SIZE;
    ::capnp::List<Segment>::Builder proto_segment_builder = proto_moments.initSegments(num_segments);
    for (int segment_index = 0; segment_index < num_segments; segment_index++) {
        Segment::Builder proto_segment = proto_segment_builder[segment_index];
        int64_t cur_segment_size = segment_index == num_segments - 1 ? last_segment_size : SEGMENT_SIZE;
        ::capnp::List<Moment>::Builder proto_moment_builder = proto_segment.initElements(cur_segment_size);
        for (int m = 0; m < cur_segment_size; m++) {
            Moment::Builder proto_moment = proto_moment_builder[m];
            cleaned_moment& internal_moment = internal_moments.at(segment_index * SEGMENT_SIZE + m);
            PlayerData::Builder proto_ball = proto_moment.initBall();

            // setting up ball
            proto_ball.setTeamId(internal_moment.ball.team_id);
            proto_ball.setPlayerId(internal_moment.ball.player_id);
            proto_ball.setXLoc(internal_moment.ball.x_loc);
            proto_ball.setYLoc(internal_moment.ball.y_loc);
            proto_ball.setRadius(internal_moment.ball.radius);

            // setting up players
            ::capnp::List<PlayerData>::Builder proto_players = proto_moment.initPlayers(10);
            for (int i = 0; i < 10; i++) {
                PlayerData::Builder proto_player = proto_players[i];
                proto_player.setTeamId(internal_moment.players[i].team_id);
                proto_player.setPlayerId(internal_moment.players[i].player_id);
                proto_player.setXLoc(internal_moment.players[i].x_loc);
                proto_player.setYLoc(internal_moment.players[i].y_loc);
                proto_player.setRadius(internal_moment.players[i].radius);
            }

            // setting up clock
            ClockFixedPoint::Builder proto_game_clock = proto_moment.initGameClock();
            proto_game_clock.setSeconds(internal_moment.game_clock.seconds);
            proto_game_clock.setTwentyFifthsOfSecond(internal_moment.game_clock.twenty_fifths_of_second);

            // setting up other data
            proto_moment.setShotClock(internal_moment.shot_clock);
            proto_moment.setQuarter(internal_moment.quarter);
            proto_moment.setGameId(internal_moment.game_id);
            proto_moment.setGameNum(internal_moment.game_num);

            // set moments
            ::capnp::List<EventMomentData>::Builder proto_emds = proto_moment.initEvents(internal_moment.events.size());
            for (int i = 0; i < internal_moment.events.size(); i++) {
                EventMomentData::Builder emd = proto_emds[i];
                emd.setEventId(internal_moment.events[i].event_id);
                emd.setMomentInEvent(internal_moment.events[i].moment_in_event);
            }
        }
    }

    int fd = open(proto_file_path.c_str(), O_CREAT | O_WRONLY | O_TRUNC);
    capnp::writeMessageToFd(fd, message);

    /*
    ::capnp::List<::capnp::List<Moment>>::Builder proto_segement_builder = proto_moments.initElements(internal_moments.size());

    for (int segment = 0; segment < internal_moments.size() / SEGMENT_SIZE; segment++) {
        ::capnp::List<Moment>::Builder proto_moment_builder = proto_segement_builder[segment];
        proto_moment_builder.
        for (int m = 0; m < SEGMENT_SIZE; m++) {
            Moment::Builder proto_moment = proto_moment_builder[m];
            cleaned_moment & internal_moment = internal_moments.at(segment * SEGMENT_SIZE + m);
        }
    }
     */
    /*
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
    */
}

void moments_proto_to_memory(string proto_file_path, vector<cleaned_moment>& internal_moments) {
    /*
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
    */
}