import load_cython
from load import *
from nearest_defender import *
from find_trajectories import *
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

start = timer()
res = find_trajectories_no_fixed_origin(10.0, 10.0, 5)
end = timer()
print("time: " + str(end - start))
print(str(res))

print("query 3: find all trajectories of a player or the ball from position " +
      "(x,y,t) to (x+10,y+10,t+5), such that x,y, and t are free variables")

start = timer()
res = find_trajectories_fixed_origin(
    {'start': {'x_loc': 70.0, 'y_loc': 16.0}, 'end': {'x_loc': 90.0, 'y_loc': 32.0}},
    {'start': {'x_loc': 71.9, 'y_loc': 24.9}, 'end': {'x_loc': 72.1, 'y_loc': 25.1}},
    5, 25)

end = timer()
print("time: " + str(end - start))
print(str(len(res)))