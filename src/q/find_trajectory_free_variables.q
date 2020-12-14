// @fileOverview Enter a description here...
/ load shots for one game, sory by increasing quarter, decreasing time within a quarter
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


moments:("JIEEEEEHJJI";enlist",") 0: `:/home/durst/big_dev/nba_movement_data/data/csv/default_game.csv
meta moments / print table metadata
moments
/ `quarter xasc `game_clock xasc `moments / don't do sort on multiple columns. KDB doesn't store multi-column sorts in attributes

rh:{0.01*floor 0.5+x*100}
/ enable sorting on quarter and shot time at once by creating a joined column
update time: rh merge_times[quarter;game_clock] from `moments

/ this needs to be ascending. Otherwise, it doesn't apply the ascending attribute and
/ queries are much slower
`time xasc `event_id xasc `moment_in_event xasc `player_id xasc`moments


time_offset: 5
/ need within 0.04 second range as that's how often the ticks occur
\t row_offset: select i from moments where time within (moments[0;`time]+time_offset-0.02;moments[0;`time]+time_offset+0.019999),player_id=moments[0;`player_id] 
row_offset: row_offset[0;`x]

filter_by_x: select t:moments[i+4741;`x_loc] within (x_loc-0.1;x_loc+0.1) from moments
count select from filter_by_x where t=1
space_offset: 10
\t filter_by_x_and_y: select t:(moments[i+row_offset;`x_loc] within (x_loc+space_offset-0.1;x_loc+space_offset+0.1))&(moments[i+row_offset;`y_loc] within (y_loc+space_offset-0.1;y_loc+space_offset+0.1)) from moments
count select from filter_by_x_and_y where t=1









/ get offset
time_offset: 5
/ need within 0.04 second range as that's how often the ticks occur
\t row_offset: select i from moments where time within (moments[0;`time]+time_offset-0.02;moments[0;`time]+time_offset+0.019999),player_id=moments[0;`player_id] 
row_offset-9000

filter_by_x: select t:moments[i+4741;`x_loc] within (x_loc-0.1;x_loc+0.1) from moments
count select from filter_by_x where t=1
/\t filter_by_x_and_y: select t:(moments[i+4741;`x_loc] within (x_loc-0.1;x_loc+0.1))&(moments[i+4741;`y_loc] within (y_loc-0.1;y_loc+0.1)) from moments
/count select from filter_by_x_and_y where t=1




\t filter_by_x_and_y: select t:(moments[i+3377;`x_loc] within (x_loc+10-0.1;x_loc+10+0.1))&(moments[i+3377;`y_loc] within (y_loc+10-0.1;y_loc+10+0.1)) from moments
count select from filter_by_x_and_y where t=1



/ below not needed



get_endpoint:{[query_player] first select from moments where time within (query_player[`time]+10-0.01;query_player[`time]+10+0.01),player_id=query_player[`player_id]}
get_endpoint each moments
meta moments_shifted

moments[0;`time]
select from moments where time within (1009.96;1010.04)
\P 6
select from moments where time within (1000.01;1000.05)
(first select from moments where time > 1000)[`time]=1000.04

(moments[0;`time]+0.04)=1000.04


select (10+x_loc)=moments[i+4741] & ((10+y_loc)=moments[i+4741]) from moments

moments_shifted: update time: time-10 from moments
`time xasc `moments_shifted
joined:aj[`time;moments_shifted;moments]
joined