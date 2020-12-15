// @fileOverview Enter a description here...
moments:("JIEEEEEHJJI";enlist",") 0: `:/home/durst/big_dev/nba_movement_data/data/csv/default_game.csv
meta moments / print table metadata
moments
/ `quarter xasc `game_clock xasc `moments / don't do sort on multiple columns. KDB doesn't store multi-column sorts in attributes

// @param quarter {} 
// @param game_clock {} 
// @returns {Type} Enter a return description here...
merge_times: {[quarter; game_clock] (quarter*1000) + 720-game_clock}
rh:{0.01*floor 0.5+x*100}
/ enable sorting on quarter and shot time at once by creating a joined column
update time: rh merge_times[quarter;game_clock] from `moments

/ this needs to be ascending. Otherwise, it doesn't apply the ascending attribute and
/ queries are much slower
`x_loc xasc `time xasc `event_id xasc `moment_in_event xasc `player_id xasc `moments

/ working verison of query
count starting_moments_2: select from moments where x_loc within (70.0;90.0),y_loc within (16.0;32.0)
\t sum 0 < {[row]  count select from moments where x_loc within(71.9; 72.1), y_loc within(24.9; 25.1), player_id = row[`player_id], quarter = row[`quarter], time within((row[`time]-5)-1; (row[`time]-5)+1) } each starting_moments_2


/ this version of the query went the wrong direction (+time_offset meant looking backwards in time)
/ fixing that issue revelaed 0 hits, so when flipped direction fo query (and wrapped -5 in brackets due to kdb weird precedence rules) things starting working
space_delta: 0.1
\t starting_moments: select from moments where x_loc within (72.0-space_delta;72.0+space_delta),y_loc within (25.0-space_delta;25+space_delta)
time_offset: 5
space_offset: 10
\t sum 0 < {[row]  count select from moments where x_loc within(70; 90), y_loc within(16; 32), player_id = row[`player_id], quarter = row[`quarter], time within(row[`time]+time_offset-space_delta; row[`time]+time_offset+space_delta) } each starting_moments
({[row]  select from moments where x_loc within(70; 90), y_loc within(16; 32), player_id = row[`player_id], quarter = row[`quarter], time within(row[`time]+time_offset-space_delta; row[`time]+time_offset+space_delta) } each starting_moments)[16]
count `time xasc starting_moments


/ ignore below this line

{[row] count select from moments where x_loc within(row[`x_loc]+space_offset-space_delta; row[`x_loc]+space_offset+space_delta), y_loc within(row[`y_loc]+space_offset-space_delta; row[`y_loc]+space_offset+space_delta), time within(row[`time]+time_offset-space_delta; row[`time]+time_offset+space_delta) } each starting_moments

moments_x_y: update x_y: (enlist x_loc) from moments


time_offset: 5
/ need within 0.04 second range as that's how often the ticks occur
\t row_offset: select i from moments where time within (moments[0;`time]+time_offset-0.02;moments[0;`time]+time_offset+0.019999),player_id=moments[0;`player_id] 
row_offset: row_offset[0;`x]

filter_by_x: select t:moments[i+4741;`x_loc] within (x_loc-0.1;x_loc+0.1) from moments
count select from filter_by_x where t=1
space_offset: 10
\t filter_by_x_and_y: select t:(moments[i+row_offset;`x_loc] within (x_loc+space_offset-0.1;x_loc+space_offset+0.1))&(moments[i+row_offset;`y_loc] within (y_loc+space_offset-0.1;y_loc+space_offset+0.1)) from moments
count select from filter_by_x_and_y where t=1

t:([] name:`Dent`Beeblebrox`Prefect; iq:98 42 126; a: ((1;2);enlist 2;enlist 3))

update b: (1;2) from t
a: (enlist `b)!enlist ((1;2))
![t;();0b;a]
iqs:{[row] (row[`iq]+1;row[`iq]+2)} each t

show t0:([]n:`x`y`x`z`z`y;p:0 15 12 20 25 14)
c: enlist (>;`p;0)
b: (enlist `n)!enlist `n
a: (enlist `p)!enlist (max;`p)
a: (enlist `p)!enlist (2)
![t0;();0b;a]
