import load_cython
from benchmark import benchmark
from load import *
from nearest_defender import *
from find_trajectories import *
import sys

parse_file("/home/durst/big_dev/nba-movement-data/data/csv/cleaned_default_game.csv",
           "/home/durst/big_dev/nba-movement-data/data/shots/cleaned_shots_fixed.csv")
get_shots_for_game(21500116)

