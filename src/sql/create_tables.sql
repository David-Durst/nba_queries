CREATE TABLE default_moments
(
    team_id         BIGINT,
    player_id       INTEGER,
    x_loc           NUMERIC(8, 5),
    y_loc           NUMERIC(8, 5),
    radius          NUMERIC(7, 5),
    game_clock      NUMERIC(6, 3),
    shot_clock      NUMERIC(5, 3),
    quarter         SMALLINT,
    game_id         BIGINT,
    event_id        BIGINT,
    moment_in_event INTEGER,
    PRIMARY KEY (player_id, event_id, moment_in_event)
);

select count(*) from default_moments;

DROP TABLE default_moments;

COPY default_moments(team_id, player_id, x_loc, y_loc, radius, game_clock, shot_clock, quarter,
                    game_id, event_id, moment_in_event)
FROM '/home/durst/big_dev/nba-movement-data/data/csv/default_game.csv'
DELIMITER ','
CSV HEADER;

CREATE INDEX timing_idx_moments ON default_moments (game_id ASC, quarter ASC, game_clock DESC, player_id ASC, event_id ASC);
DROP INDEX timing_idx_moments;
CLUSTER default_moments using timing_idx_moments;

CREATE TABLE default_events
(
    game_id                   BIGINT,
    event_num                 BIGINT,
    event_msg_type            INTEGER,
    event_msg_action_type     INTEGER,
    period                    SMALLINT,
    wc_timestring             VARCHAR(10),
    pc_timestring             VARCHAR(10),
    home_description          VARCHAR(255),
    neutral_description       VARCHAR(255),
    visitor_description       VARCHAR(255),
    score                     VARCHAR(10),
    score_margin              VARCHAR(5),
    person1_type              INTEGER,
    player1_id                INTEGER,
    player1_name              VARCHAR(30),
    player1_team_id           NUMERIC(12, 1),
    player1_team_city         VARCHAR(30),
    player1_team_nickname     VARCHAR(30),
    player1_team_abbreviation VARCHAR(5),
    person2_type              INTEGER,
    player2_id                INTEGER,
    player2_name              VARCHAR(30),
    player2_team_id           NUMERIC(12, 1),
    player2_team_city         VARCHAR(30),
    player2_team_nickname     VARCHAR(30),
    player2_team_abbreviation VARCHAR(5),
    person3_type              INTEGER,
    player3_id                INTEGER,
    player3_name              VARCHAR(30),
    player3_team_id           NUMERIC(12, 1),
    player3_team_city         VARCHAR(30),
    player3_team_nickname     VARCHAR(30),
    player3_team_abbreviation VARCHAR(5),
    PRIMARY KEY (event_num)
);

DROP TABLE default_events;

COPY default_events
    FROM '/home/durst/big_dev/nba-movement-data/data/events/default_game.csv'
    DELIMITER ','
    CSV HEADER;

select count(*) from default_events;


CREATE TABLE shots_fixed
(
    action_type         VARCHAR(40),
    event_time          INTEGER,
    event_type          VARCHAR(40),
    game_date           DATE,
    game_event_id       BIGINT,
    game_id             BIGINT,
    grid_type           VARCHAR(50),
    htm                 VARCHAR(5),
    loc_x               NUMERIC(8, 5),
    loc_y               NUMERIC(8, 5),
    minutes_remaining   INTEGER,
    period              INTEGER,
    player_id           INTEGER,
    player_name         VARCHAR(30),
    quarter             NUMERIC(4, 2),
    seconds_remaining   INTEGER,
    shot_attempted_flag BOOLEAN,
    shot_distance       INTEGER,
    shot_made_flag      BOOLEAN,
    shot_time           NUMERIC(6, 3),
    shot_type           VARCHAR(30),
    shot_zone_area      VARCHAR(30),
    shot_zone_basic     VARCHAR(30),
    shot_zone_range     VARCHAR(30),
    team_id             NUMERIC(12, 1),
    team_name           VARCHAR(30),
    team_vtm            VARCHAR(5),
    PRIMARY KEY (game_id, game_event_id)
);

DROP TABLE shots_fixed;

COPY shots_fixed
    FROM '/home/durst/big_dev/nba-movement-data/data/shots/shots_fixed.csv'
    DELIMITER ','
    CSV HEADER;

select count(*) from shots_fixed;

CREATE INDEX timing_idx_shots ON shots_fixed (game_id ASC, period ASC, shot_time DESC, player_id ASC, game_event_id ASC);

CLUSTER shots_fixed using timing_idx_shots;
