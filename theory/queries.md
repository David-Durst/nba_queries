# Queries
This document lists a set of queries and places them in the taxonomy. 
For the reference part of the taxonomy, I also state which entity is the reference.

## Already Implemented:
1. Query 1: compute the probability of making a shot in an NBA game when adjusting for the nearest defender at any point 2 seconds before or after the shot.
    1. Place in Taxonomy:
        1. reference - players/ball - moving, series
        2. comparison - nearest-neighbor in space, range in time
        3. data set changing - static
    2. Join Strategy: merge join of shots and moments for all implementations
    3. Indices: sorting by time for all implementations
    4. Performance (note: these are pre-DGX numbers. please see [timing](https://github.com/David-Durst/nba_queries/blob/master/timing.csv) for up to date numbers):
        1. PostgreSQL: 35s, even after adding custom indexes.  - yet to run on DGX
        2. C++: g++ auto-vectorization pass ran, didn't find anything 
        3. Q: 0.08s - yet to run on DGX
        4. Cython: 
2. Query 2: find all trajectories of a player or the ball from position (x,y,t) to (x+10,y+10,t+5), such that x,y, and t are free variables
    1. Place in Taxonomy:
        1. reference - players/ball - moving, series
        2. comparison - point in space and time
            1. Detail used a range of +/- 0.1 feet in x and y to handle issues with floating point comparisons.
        3. data set changing - static
    2. Join Strategy: no join required
    3. Indices: sorting by time for all implementations
    4. Performance:
        1. C++: 0.07s - yet to run on DGX
        2. Q: 0.164s - yet to run on DGX
        3. Cython: 0.38s - yet to run on DGX
3. Query 3: find all trajectories of a player or the ball that start in the range ((64.5, 24.9, t) <-> (65.5, 25.1, t)) and end in the range ((70, 16, t+1) <-> (90, 32, t+3)). 
   The starting range is top of the key on one side of the courrt. THe destination is the paint on that side of the court. 
    1. Place in Taxonomy:
        1. reference - region of court - static, single (using region of court as reference, not players)
        2. comparison - range in space, point in time
            1. Detail: used a range of +/- 1 second in time
        3. data set changing - static
    2. Join Strategy: no join required
    3. Indices: sort by x for Q, Octree for C++
    4. Performance:
        1. C++: g++ auto-vectorization pass ran, added some SIMD instructions
        2. Q: 48.18s (implemented using linear scan) - yet to run on DGX
        3. Cython: 
    5. Future work on this query: a better answer for this query appears to be sorting by time and taking the approach from query 2.
    It's surprising that the octree didn't help. Other indices focused on trajectories may improve over both the octree and sort by time approaches.
4. Query 12: when Paul George is in the paint, is he getting rewarded by being near the ball? - spatial
5. Query 13: give me the height of the ball in the last 24 seconds of a game - temporal
6. Query 14: give me all the players in the paint in the last 24 seconds of a game - space-time
7. Query 15: give me all the players in the paint in the last 2 seconds of the shot clock - space and sparse time

## More Realistic Queries
1. Query R1 - what percent of offensive possessions where a player is on a court does he iso?
    1. Place in taxonomy
        1. reference - player - moving, series, distributed in time
        2. comparison - nearest neighbor
        3. data set changing - static
    2. concepts - possession
    3. problem - this is just a temporal query, not spatial - need to look over all space for regions defined by time (when player has ball)
        1. this is temporal assuming you've built a spatial index of who has the ball
2. Query R2 - How frequently does a player pump fake/crossover/other fakeout move?
    1. Place in taxonomy
        1. reference - player - moving, series, distributed in time
        2. comparison - trajectory similarity
        3. data set changing - static
    2. concepts - velocity, fakes 
3. Query R3 - What percent of the time when a player is on the court do they havge the ball?
    1. Place in taxonomy
        1. reference - player - moving, series, distributed in time
        2. comparison - range query
        3. data set changing - static
    2. concepts - player possession
4. Query R4 - What does it mean to be open? - this will test if players receive passes when not open. Also, shots should increase rate of going in when open
    1. Place in taxonomy
        1. reference - player - moving, series, distributed in space
        2. comparison - trajectory
        3. data set changing - static
    2. concepts - open for passing or shooting
5. Query R5 - When a player is open, does he pass it to the open man?
    1. Place in taxonomy
        1. reference - player - moving, series, distributed in time
        2. comparison - trajectory, range query
        3. data set changing - static
    2. concepts - open for passing, player possession
6. Query R6 - Does a player have gravity?
    1. Place in taxonomy
        1. reference - player - static, series, distributed in space
        2. comparison - trajectory, range query
        3. data set changing - static
    2. concepts - offensive and defense sets
7. Query R7 - How sticky is a player
    1. Place in taxonomy
        1. reference - player - static, series, distributed in space and time
        2. comparison - trajectory, range query
        3. data set changing - static
    2. concepts - possesion, defense sets
    3. problem - does this get thrown off by zone? I could detect zone first
    

## Car Queries
1. Query C1 - When a car gets stuck behind a bike, how early could've the car seen the bike and prepard to go around it?
    1. Place in taxonomy
        1. reference - car - moving, series, distributed in time
        2. comparison - nearest neighbor
        3. data set changing - static
    2. problem - this is a tmeporal query - this filters down to looking in a window of time, then looking at two objects in that window and compaing their coordinates
2. Query C2 - Find me the places where cars get stuck behind bikes most frequently 
    1. Place in taxonomy - 
        1. reference - place in world, series, distributed in space
        2. comparison - range query
        3. data set changing - static
    2. it seems like queries where the reference is static are better for my system. Since the reference is a static region, can index in space for it rather than just filtering in time
3. Query C3 - find me the places where a car almost hit another entity
    1. could you hash by trajectory?
    2. does the approximate version of this reduce to matrix multiplication like patterns
        1. let A be the matrix of trajectories for cars. Each column is a triple of a car's x/y/z
        2. Let B be the matrix of trajectories for bikes. Each column is a triple of a bike's x/y/z
        3. Let intersection be the operator of the form
            intersection(A, B):
                for each car in A:
                    for each bike in B:
                        for each time t:
                            let v = vector produced by car_at_time_t+5 and car_at_time_t-5
                            let u = vector produced by bike_at_time_t+5 and bike_at_time_t-5
                            return if u and v intersect



## Going To Implement
1. Query 4: Fouls per player when moving more than 5 MPH (roughly 1.5 ft/s)
    1. Place in Taxonomy:
        1. reference - players - moving, series
        2. comparison - none
        3. data set changing - static
    2. Join Strategy: merge join of events and moments for all implementations
        1. The table of all events (including fouls) is a separate table from just the shots.
    3. Indices: sorting by time; possibly a secondary, velocity index
2. Query 5: The first player in each game to score when standing in the range ((70, 16, t) - (90, 32, t)) (aka in the paint)
    1. Place in Taxonomy:
        1. reference - region of court - static, series
        2. comparison - none
        3. data set changing - static
    2. Join Strategy: nested loop join of shots and moments
    3. Indices: shots sorted by time, octree on moments - try to do join with just subset of moments in the paint
3. Query 6: After George Hill or Montas Ellis (the two starting guards on the Pacers) pass to Paul George, is there a shot in the next 2 seconds? 
    1.  Place in Taxonomy
        1. reference - ball - moving, single 
        2. comparison - none
        3. data set changing - static
    2. Join Strategy: merge join of passes and moments
        1. I'll need to make a table of passes
    3. Indices: passes and moments sorted by time, maybe a trajectory index
4. Query 7: Players moving more than 1.5 ft/s through the range ((70, 16, t) - (90, 32, t)) (aka in the paint)
    1. Place in Taxonomy:
        1. reference - region of court - static, single
        2. comparison - range in space
        3. data set changing - static
    2. Join Strategy: none
    3. Indices: octree on moments with a secondary, velocity index
5. Query 8: How many passes are needed before the handler has at least 4 feet of space for an open shot
    1. Place in Taxonomy:
        1. reference - ball and players - moving, series 
        2. comparison - nearest-neighbor in space
        3. data set changing - static
    2. Join Strategy: none
    3. Indices: sort by time
6. Query 9: When Paul George shoots the ball, who is within 2 feet of him within 2 seconds of the shot?
    1. Place in Taxonomy:
        1. reference - Paul George - moving, single
        2. comparison - range
        3. data set changing - static
    2. Join Strategy: merge join or nested loop join depending on how many shots Paul George takes
    3. Indices: sort by time for joining shots with moments, octree for checking those nearby
7. Query 10: In a 2-on-1 situation when both offensive players are driving to the hoop (their trajectories will intersect the hoop),
   does the handler score or pass? 
    1. Place in Taxonomy:
        1. reference - offensive players - moving, kinda single
        2. comparison - trajectory intersection
        3. data set changing - static
    2. Join Strategy: merge join or nested loop join depending on how good indices are for finding just driving
    3. Indices: sort by time for joining shots with moments, trajectory index for finding whose trajectories intersect with hoop
8. Query 11: Do steals happen when the ball is moving faster or slower than 1.5 ft/s in x-y?
    1. Place in Taxonomy:
        1. reference - ball - moving, single
        2. comparison - none
        3. data set changing - static
    2. Join Strategy: merge join 
    3. Indices: sort by time for joining shots with moments, velocity index for not needing to recompute velocity
        1. Is velocity index reall necessary? I'm already doing a rolling window, so computing velocity is just a little arithmetic. 
           This won't contribute to memory utilization, which is the bottleneck.


# Notes on queries
1. It appears that forall queries are done by merge, since you're going to scan over the entire data set. 
   Queries that look for a single reference or a small series can benefit from nested loop joins with indices
           
           
    
    
    
   
   
