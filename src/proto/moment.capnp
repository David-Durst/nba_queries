@0x99e7d5f9a9adb1db;
struct PlayerData {
    teamId @0 :Int64;
    playerId @1 :Int32;
    xLoc @2 :Float64;
    yLoc @3 :Float64;
    radius @4 :Float64;
}

struct ClockFixedPoint {
    seconds @0 :Int64;
    twentyFifthsOfSecond @1 :Int32;
}

struct EventMomentData {
    eventId @0 :Int64;
    momentInEvent @1 :Int32;
}

struct Moment {
    ball @0 :PlayerData;
    players @1 :List(PlayerData);
    gameClock @2 :ClockFixedPoint;
    shotClock @3 :Float64;
    quarter @4 :Int32;
    gameId @5 :Int64;
    gameNum @6 :Int32;
    events @7 :List(EventMomentData);
}

struct Segment {
    elements @0 :List(Moment);
}

struct Moments {
    segments @0 :List(Segment);
}
