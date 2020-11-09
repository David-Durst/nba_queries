-- enable cross tabulation https://www.vertabelo.com/blog/creating-pivot-tables-in-postgresql-using-the-crosstab-function/
CREATE extension tablefunc;

-- basic join
select *
from default_moments
         inner join shots_fixed
                    on default_moments.event_id = shots_fixed.game_event_id and default_moments.game_clock = shots_fixed.shot_time
where shots_fixed.game_id = 21500116
limit 10
;

-- joining data to get shooters position at same time as other shots
select shooter_moments.player_id, other_moments.player_id, shots_fixed.player_id,
       shooter_moments.game_clock, shooter_moments.quarter,
       shooter_moments.x_loc, shooter_moments.y_loc,
       other_moments.x_loc, other_moments.x_loc,
       shots_fixed.shot_attempted_flag, shots_fixed.shot_made_flag
from default_moments as shooter_moments
         inner join shots_fixed
                    on shooter_moments.event_id = shots_fixed.game_event_id and
                       shooter_moments.game_clock = shots_fixed.shot_time and
                       shooter_moments.quarter = shots_fixed.period and
                       shooter_moments.player_id = shots_fixed.player_id
         inner join default_moments as other_moments
                    on other_moments.event_id = shots_fixed.game_event_id and
                       other_moments.game_clock = shots_fixed.shot_time
where shots_fixed.game_id = 21500116
order by shooter_moments.quarter, shooter_moments.game_clock desc
limit 22
;

-- getting closest opponent on each shot
select shooter_moments.player_id as player_id,
       shooter_moments.game_clock as game_clock, shooter_moments.quarter as quarter,
       min(sqrt(pow(shooter_moments.x_loc - other_moments.x_loc,2) +
                pow(shooter_moments.y_loc - other_moments.y_loc,2))) as min_dist,
       bool_or(shots_fixed.shot_attempted_flag) as shot_attempted, bool_or(shots_fixed.shot_made_flag) as shot_made
from default_moments as shooter_moments
         inner join shots_fixed
                    on shooter_moments.event_id = shots_fixed.game_event_id and
                       shooter_moments.game_clock = shots_fixed.shot_time and
                       shooter_moments.quarter = shots_fixed.period and
                       shooter_moments.player_id = shots_fixed.player_id
         inner join default_moments as other_moments
                    on other_moments.event_id = shots_fixed.game_event_id and
                       other_moments.game_clock = shots_fixed.shot_time
where shots_fixed.game_id = 21500116 and other_moments.player_id != -1 and
        shooter_moments.team_id != other_moments.team_id
group by shooter_moments.player_id, shooter_moments.game_clock, shooter_moments.quarter, shooter_moments.event_id
order by shooter_moments.quarter, shooter_moments.game_clock desc
limit 22
;


-- find probability of a make with distance bucketed by 1 foot
select round(min_dist,0) as bucketed_distance, count(*) as num_shots_in_bucket,
       sum(case when shot_made then 1 else 0 end)::numeric / sum(case when shot_attempted then 1 else 0 end)::numeric as pct
from(
        select shooter_moments.player_id as player_id,
               shooter_moments.game_clock as game_clock, shooter_moments.quarter as quarter,
               min(sqrt(pow(shooter_moments.x_loc - other_moments.x_loc,2) +
                        pow(shooter_moments.y_loc - other_moments.y_loc,2))) as min_dist,
               bool_or(shots_fixed.shot_attempted_flag) as shot_attempted, bool_or(shots_fixed.shot_made_flag) as shot_made
        from default_moments as shooter_moments
                 inner join shots_fixed
                            on shooter_moments.event_id = shots_fixed.game_event_id and
                               shooter_moments.game_clock = shots_fixed.shot_time and
                               shooter_moments.quarter = shots_fixed.period and
                               shooter_moments.player_id = shots_fixed.player_id
                 inner join default_moments as other_moments
                            on other_moments.event_id = shots_fixed.game_event_id and
                               other_moments.game_clock = shots_fixed.shot_time
        where shots_fixed.game_id = 21500116 and other_moments.player_id != -1 and
                shooter_moments.team_id != other_moments.team_id
        group by shooter_moments.player_id, shooter_moments.game_clock, shooter_moments.quarter, shooter_moments.event_id

    ) distance_per_shot
group by bucketed_distance
having count(*) > 4
;


-- find probability of a make with distance bucketed by 1 foot, allowing 2 seconds for nearest defender
select round(min_dist,0) as bucketed_distance, count(*) as num_shots_in_bucket,
       sum(case when shot_made then 1 else 0 end)::numeric / sum(case when shot_attempted then 1 else 0 end)::numeric as pct
from(
        select shooter_moments.player_id as player_id,
               shooter_moments.game_clock as game_clock, shooter_moments.quarter as quarter,
               min(sqrt(pow(shooter_moments.x_loc - other_moments.x_loc,2) +
                        pow(shooter_moments.y_loc - other_moments.y_loc,2))) as min_dist,
               bool_or(shots_fixed.shot_attempted_flag) as shot_attempted, bool_or(shots_fixed.shot_made_flag) as shot_made
        from default_moments as shooter_moments
                 inner join shots_fixed
                            on shooter_moments.event_id = shots_fixed.game_event_id and
                               shooter_moments.game_clock = shots_fixed.shot_time and
                               shooter_moments.quarter = shots_fixed.period and
                               shooter_moments.player_id = shots_fixed.player_id
                 inner join default_moments as other_moments
                            on other_moments.event_id = shots_fixed.game_event_id and
                               abs(other_moments.game_clock - shots_fixed.shot_time) < 2
        where shots_fixed.game_id = 21500116 and other_moments.player_id != -1 and
                shooter_moments.team_id != other_moments.team_id
        group by shooter_moments.player_id, shooter_moments.game_clock, shooter_moments.quarter, shooter_moments.event_id

    ) distance_per_shot
group by bucketed_distance
having count(*) > 4
;

select *
from default_moments
limit 10;

select *
from shots_fixed
limit 10;
