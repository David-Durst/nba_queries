// @fileOverview Enter a description here...
\l /home/durst/dev/kdb/csvutil.q

/ load shots for one game, sory by increasing quarter, decreasing time within a quarter
/ https://github.com/KxSystems/kdb/blob/master/utils/csvutil.q
/shots:.csv.read `:/home/durst/big_dev/nba_movement_data/data/shots/shots_fixed.csv / seems easier, but it screws up some types
shots:("SISSJJSSEEIIISEIBIBESSSSESS";enlist",") 0: `:/home/durst/big_dev/nba_movement_data/data/shots/shots_fixed.csv
meta shots / print table metadata

shots_one_game: shots where shots[`GAME_ID]=0021500116
/`QUARTER xasc `SHOT_TIME xasc `shots_one_game / don't do sort on multiple columns. KDB doesn't store multi-column sorts in attributes

shots_one_game / print first few shot times

/ enable sorting on quarter and shot time at once by creating a joined column
// @param quarter {} 
// @param game_clock {} 
// @returns {Type} Enter a return description here...
merge_times: {[quarter; game_clock] (quarter*1000) + 720-game_clock}
update time: merge_times[QUARTER;SHOT_TIME] from `shots_one_game

`time xasc `shots_one_game



/moments:.csv.read `:/home/durst/big_dev/nba_movement_data/data/csv/default_game.csv
moments:("JIEEEEEHJJI";enlist",") 0: `:/home/durst/big_dev/nba_movement_data/data/csv/default_game.csv
meta moments / print table metadata
moments
/ `quarter xasc `game_clock xasc `moments / don't do sort on multiple columns. KDB doesn't store multi-column sorts in attributes

/ enable sorting on quarter and shot time at once by creating a joined column
update time: merge_times[quarter;game_clock] from `moments

/ this needs to be ascending. Otherwise, it doesn't apply the ascending attribute and
/ queries are much slower
`time xasc `moments

\t select from moments where time within (4700-2; 4700+2)

moments2: update time2: time,player_id2:player_id from moments

get_shooter:{[query_time; query_shooter_id] first select from moments where time within (query_time-0.02;query_time+0.02),player_id=query_shooter_id}
get_shooter_renamed:{[query_time; query_shooter_id] first select from moments2 where time2 within (query_time-0.02;query_time+0.02),player_id2=query_shooter_id}
get_shooter2:{[query_time; query_shooter_id] first select from moments}
get_shooter3:{[query_time; query_shooter_id] first select from moments where player_id~query_shooter_id }
get_shooter4:{[query_time; query_shooter_id] first select from moments where player_id~query_shooter_id,time~query_time }

compute_distance:{[x1;y1;x2;y2] sqrt ((x1 - x2) xexp 2) + ((y1-y2) xexp 2)}
get_nearest_old:{[shooter] first select dist:min sqrt((((x_loc - shooter[`x_loc]) xexp 2) + ((y_loc - shooter[`y_loc]) xexp 2))) from moments where time within (shooter[`time]-2;shooter[`time]+2) }
get_nearest:{[shooter] first select dist:min compute_distance[x_loc;y_loc; shooter[`x_loc]; shooter[`y_loc]] from moments where time within (shooter[`time]-2.0;shooter[`time]+2.0),team_id<>first_shooter[`team_id],team_id<>-1}

update nearest_defender: get_nearest[get_shooter[time; PLAYER_ID]][`dist] from shots_one_game / this doesn't /it works when i replace equals or within with ~ / that's only because ~ drops too much
count get_nearest each shots_one_game[`time] get_shooter'shots_one_game[`PLAYER_ID] / this works for some of the values

update nearest_defender: time+PLAYER_ID from shots_one_game / this works
update nearest_defender: get_shooter[time; PLAYER_ID] from shots_one_game / this doesn't
small_shots: shots_one_game 0 1
update nearest_defender: get_shooter[time; PLAYER_ID][`time] from small_shots / trying with fewer elements to find issue 

update nearest_defender: (first select from moments)[`time] from small_shots / seeing if function call is the issue
update nearest_defender: get_shooter2[time;PLAYER_ID][`time] from small_shots / this works, so function call not the issue
update nearest_defender: get_shooter3[time;PLAYER_ID][`time] from small_shots / this only works after convert player_id= to player_id~
update nearest_defender: get_shooter4[time;PLAYER_ID][`time] from small_shots / this doesn't work, so filter is an sissue

/ issue with player_id, so copmaring values in shots and moments
distinct select player_id from moments
distinct select PLAYER_ID from small_shots
first select from moments where player_id=101145
first select from moments where player_id=101139

update nearest_defender: moments[0][`time] from small_shots / trying with fewer elements to find issue 
get_shooter[time; PLAYER_ID][`time]
get_shooter[small_shots[`time];small_shots[`PLAYER_ID]]
(first select from moments where time within (1041.56;1041.58))[`time]
first select from moments where time=1041.57e
\P 6
/ 1041.57

/ getting one shot and shooter to work with
first_shot: first shots_one_game
first_shooter: get_shooter[first_shot[`time];first_shot[`PLAYER_ID]]
(first_shot[`time];first_shot[`PLAYER_ID])

/ try out get_shooter and get_nearest on one shot/shooter
first select dist:min compute_distance[x_loc;y_loc; first_shooter[`x_loc]; first_shooter[`y_loc]] from moments where time within (first_shooter[`time]-2;first_shooter[`time]+2),team_id<>first_shooter[`team_id],team_id<>-1
get_nearest[first_shooter][`dist]
idx:5
get_shooter[shots_one_game[idx][`time];shots_one_game[idx][`PLAYER_ID]]
get_nearest[get_shooter[shots_one_game[idx][`time];shots_one_game[idx][`PLAYER_ID]]][`dist]
type "a"

/ try out computing distance
select from moments where time = first_shooter[`time]
select from moments where time within (first_shooter[`time]-2;first_shooter[`time]+2)
select d:(x_loc - first_shooter[`x_loc]), dist:compute_distance[x_loc; y_loc; first_shooter[`x_loc]; first_shooter[`y_loc]] from moments where time within (first_shooter[`time]-2;first_shooter[`time]+2)
compute_distance2:{[x1;y1;x2;y2] sqrt ((x1 - x2) xexp 2) + (y1-y2)}
compute_distance[2;3;4;1]

add:{x[`time]+y}

shots_one_game add\: 2.0

shots_one_game[`time]+2


add[;2] each 2 3 4

select from moments where time within (4700-2; 4700+2)

test_moment:first select from moments where time=4720,player_id=-1
test_moment[`team_id]

\t select from moments where x_loc within (5; 6)

\t moments[`time] within (4700-2; 4700+2)

distinct select quarter from moments
moments[1500000]