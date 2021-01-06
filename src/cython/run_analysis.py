import load_cython
from nearest_defender import *
from timeit import default_timer as timer
parse_file("/home/durst/big_dev/nba-movement-data/data/csv/cleaned_default_game.csv",
           "/home/durst/big_dev/nba-movement-data/data/shots/shots_fixed.csv")
get_shots_for_game(21500116)
start = timer()
res = find_nearest_defender_at_each_shot()
end = timer()
print("time: " + str(end - start))
print(str(bucket_distances(res)))
