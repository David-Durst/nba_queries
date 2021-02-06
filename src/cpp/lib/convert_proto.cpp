#include "convert_proto.h"
#include <capnp/message.h>
#include <capnp/serialize.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <errno.h>
#include <iostream>

int64_t SEGMENT_SIZE = 10000000;
void moments_memory_to_proto(vector<cleaned_moment>& internal_moments, string proto_file_path) {
    ::capnp::MallocMessageBuilder message;

    Moments::Builder proto_moments = message.initRoot<Moments>();
    int64_t num_segments = internal_moments.size() / SEGMENT_SIZE + 1;
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

    int fd = open(proto_file_path.c_str(), O_CREAT | O_WRONLY | O_TRUNC, S_IRWXU | S_IRWXG | S_IROTH | S_IWOTH);
    if (fd < 0)
    {
        fprintf(stderr, "Error opening file: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    capnp::writeMessageToFd(fd, message);
    close(fd);
}

void moments_proto_to_memory(string proto_file_path, vector<cleaned_moment>& internal_moments) {
    // https://groups.google.com/g/capnproto/c/f0-kF8JAqK4/m/TtDCO5BNCAAJ
    int fd = open(proto_file_path.c_str(), O_RDONLY);
    if (fd < 0)
    {
        fprintf(stderr, "Error opening file: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    struct stat stats;
    fstat(fd, &stats);
    void * addr = mmap(NULL, stats.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    kj::ArrayPtr<capnp::word> words(reinterpret_cast<capnp::word*>(addr),
                                    stats.st_size / sizeof(capnp::word));
    capnp::ReaderOptions options;
    options.traversalLimitInWords = 500ul * 1024ul * 1024ul * 1024ul;
    capnp::FlatArrayMessageReader messageReader(words, options);
    Moments::Reader proto_moments = messageReader.getRoot<Moments>();

    for (Segment::Reader segment : proto_moments.getSegments()) {
        for (Moment::Reader proto_moment : segment.getElements() ) {
            cleaned_moment internal_moment;
            internal_moment.ball.team_id = proto_moment.getBall().getTeamId();
            internal_moment.ball.player_id = proto_moment.getBall().getPlayerId();
            internal_moment.ball.x_loc = proto_moment.getBall().getXLoc();
            internal_moment.ball.y_loc = proto_moment.getBall().getYLoc();
            internal_moment.ball.radius = proto_moment.getBall().getRadius();

            int i = 0;
            for (PlayerData::Reader proto_player : proto_moment.getPlayers()) {
                internal_moment.players[i].team_id = proto_player.getTeamId();
                internal_moment.players[i].player_id = proto_player.getPlayerId();
                internal_moment.players[i].x_loc = proto_player.getXLoc();
                internal_moment.players[i].y_loc = proto_player.getYLoc();
                internal_moment.players[i].radius = proto_player.getRadius();
                i++;
            }

            internal_moment.game_clock.seconds = proto_moment.getGameClock().getSeconds();
            internal_moment.game_clock.twenty_fifths_of_second = proto_moment.getGameClock().getTwentyFifthsOfSecond();

            internal_moment.shot_clock = proto_moment.getShotClock();
            internal_moment.quarter = proto_moment.getQuarter();
            internal_moment.game_id = proto_moment.getGameId();
            internal_moment.game_num = proto_moment.getGameNum();

            for (EventMomentData::Reader proto_emd : proto_moment.getEvents()) {
                event_moment_data emd;
                emd.event_id = proto_emd.getEventId();
                emd.moment_in_event = proto_emd.getMomentInEvent();
                internal_moment.events.push_back(emd);
            }

            internal_moments.push_back(internal_moment);

        }
    }

    munmap(addr, stats.st_size);
    close(fd);

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