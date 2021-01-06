import load_cython
from load import *
from nearest_defender import *
from timeit import default_timer as timer
parse_file("/home/durst/big_dev/nba-movement-data/data/csv/cleaned_default_game.csv",
           "/home/durst/big_dev/nba-movement-data/data/shots/shots_fixed.csv")
get_shots_for_game(21500116)

print("query 1: nearest defender within 2 seconds of each shot")
start = timer()
res = find_nearest_defender_at_each_shot()
end = timer()
print("time: " + str(end - start))
print(str(bucket_distances(res)))

print("query 2: find all trajectories of a player or the ball from position " +
      "(x,y,t) to (x+10,y+10,t+5), such that x,y, and t are free variables")
