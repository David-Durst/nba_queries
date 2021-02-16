# Concepts
## Concepts for Analytics
### State of Game
1. Player Possession -
    1. informal definition - which player has the ball
    2. formal defintion - \forall time windows w of n ticks, player_id_for_player(mode \forall t in w nearest_within_4_feet_or_ball(t, ball, players))
        1. restate formal definition - the player with the minimum distance to the ball during the time window
    3. correction - don't count ticks where nearest player is more than 4 feet away. If the mode is for no player to be closer than 4 feet, this is a pass or a shot
    4. assumption - 
        1. pick 4 for distance since largest wingspans are 8 feet and reach is half of wingspan at max 
        2. since players don't have height (only ball), assume they are 7 feet tall and arms at 5/6 way up
        3. since players can go arbitrarily low to ground, only count z when ball in air 
1. Team Possession -
    1. informal definition - which team has the ball
    2. formal definition - \forall time windows w of n ticks, player_possession(w).team
        1. restate formal definition - the team of the player with the ball
1. Stoppage in play -
    1. informal definition - players aren't standing totally still (which would indicate garbage data) and are moving continuously (no more than 1 foot in 1/25 of a second)
    2. formal definition - \forall time windows w of n ticks, (exists t in w. 
            exists player i. hypot(player[t][i].x - player[t-1][i].x, player[t][i].y != player[t-][i].y) >= 1)
            OR
            forall player i. hypot(player[t][i].x - player[t-1][i].x, player[t][i].y != player[t-][i].y) < 0.001)
        1. restate formal definition - there's at least 1 tick in the window where 1 player stands totally still or jumps a large amount
    3. correction - a couple players can stand totally still while others are moving. This isn't a stoppage.
    So need all players to be still for a stoppage. Also a stoppage if anyone teleports.

### 1-2 Player Action (Action)
1. Turnover -
    1. informal definition - change in possession without a score
        1. a pass out of bounds is a turnover, so stoppages are allowed
    2. formal definition - \forall time windows w of n tick, if team_possession(w) != team_poessesion(w - 1)
        1. restate formal definition - there's change
1. Pass -
    1. informal definition - when one player throws the ball to another player on the same team
    2. formal definition - \forall time windows w of n tick, if team_possession(w) == team_poessesion(w - 1) and player_possesion(w) != player_possesion(w - 1)
       and not stoppage(w) and not stoppage(w-1)
        1. restate formal definition - there's a change of possession without a turnover or a stoppage
1. Assist -
    1. informal definition - a pass followed by a shot
    2. formal definition - \forall shots s, if \exists pass p s.t. s.time > p.time >= s.time - 2 seconds
        1. restate formal definition - there's a pass right before a shot
1. Offensive Rebound -
    1. informal definition - missed shot without a change in possession or a stoppage
    2. formal definition - \forall shots s, if team_possesion(window_for(s.t+1)) == team_possesion(window_for(s.t+1) - 1) and not stoppage(window_for(s.t+1))
        1. restate formal definition - there's no change in possession around a shot
1. Defensive Reboud -
    1. informal definition - missed shot followed by a change in possesion or a stoppage
    2. formal definition - \forall shots s, if team_possesion(window_for(s.t+1)) != team_possesion(window_for(s.t+1) - 1) and not stoppage(window_for(s.t+1))
        1. restate formal definition - inverse of offensive rebound

### Whole Team Action (Plays)
1. Fast break -
    1. informal definition - when there are more offensive players than defensive players on a drive towards the basket
        1. typically this is a 2v1 or 3v2 situation
           I don't think a 4v3 counts since the 3 defenders can fill enough space that there isn't a signficant advantage.
           However, I don't know for sure that 4v3 isn't a fast break.
    2. formal definition - \forall time ticks t. t if (exists a set of first n sorted_players s.t. > 50% of those players.team = team_with_possesion)
       let players_sorted = sort players[t] by distance to basket
        1. restate formal definition - can find a way to slice court such that more offensive players than defensive players on the slice of the court with the hoop
1. Secondary break -
    1. informal definition - when a team moves into the half court offense following a fast break
        1. examples of secondary break - https://www.fastmodelsports.com/library/basketball/search?searchString=Secondary+Break
    2. formal definition - \forall time ticks t. t if \exists u \in (0 seconds, 3 seconds) s.t. fast_break[t-u] == true and (team_possession[t'] == true forall t >= t' >= t-u)
        1. restate formal definition - if you were in the fast break and now you aren't but you kept possession, you're in the half court, so this is the secondary break

### Geographic Region
1. Paint -
    1. informal definition - painted region of the court under the hoop
        1. aka paint or key - https://www.sportingcharts.com/dictionary/nba/lane/
    2. formal definition - paint0{{0.0f,16.0f,0.0}, {20.0f,32.0f, 60.0}}, paint1{{70.0f,16.0f,0.0}, {90.0f,32.0f, 60.0}}
1. Post - (what they call the low post https://www.coachesclipboard.net/BasketballTerminology.html)
    1. informal definition - the half of the paint closest to the baseline
    2. formal definition -  paint0{{0.0f,16.0f,0.0}, {10.0f,32.0f, 60.0}}, paint1{{80.0f,16.0f,0.0}, {90.0f,32.0f, 60.0}}
1. 3 point region
    1. informal definition - the area beyond the 3 point line
    2. formal defintion - yet to define, but I'll need more complex geometry than just squares. Either a polygon or a bezier curve to handle curve of 3 pointe line.
2. restricted area -
    1. informal definition - rectangle with semicircle directly under the hoop - https://www.sportingcharts.com/dictionary/nba/restricted-area.aspx
    2. formal definition - same as 3 point region


## Concepts For My "Realistic Questions"
1. possession -
    1. team - team has possession for a sequence of seconds such that: for each second the nearest player to the ball is on the current team
    2. player - same, but considering each player rather team
2. distance to basket - using a different coordinate system for each player, tracking distance to basket, could be good for sorting
    1. Could I bin by time and distance to basket?
3. player in game - track which players are in game. This will decrease seek times by 10x as only need to look at one player, not all players to find one's location
4. spatial bins - works well for binning when all court has values at all time
5. velocity
6. fakes - times when the acceleration flips sign frequently
7. pass - change of possession between two players of same team
8. open -
    1. for shooting - velocity taking him toa  sport where no on else will be
    2. for passing - velocity taking him to a sport where a trjaectory will exist from ball's possessor
9. offensive set - layout in court (could encode this using bins)
    1. does this need to include velocity? - https://hooptactics.net/premium/basketballbasics/bb5offenses.php suggests no
    2. the academic research does, for 2-3 second plays, needed an encoding for the plays - https://dl.acm.org/doi/pdf/10.1145/3185596
10. defensive set - seems like just man or zone
    1. this could be based on stickiness for a single man to detect man defenses - https://hooptactics.net/premium/basketballbasics/bb6defenses.php
    2. but you would need a different setup for zone defenses
11. stick - how close does a player stick to his man over time? - does this get thrown off by zone defense?
