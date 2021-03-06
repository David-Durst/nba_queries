import load_cython
from benchmark import benchmark
from load import *
from nearest_defender import *
from find_trajectories import *
import sys
from math import nan
from dataclasses import dataclass, field

@dataclass(eq=True)
class Results():
    query1_rowstore_sequential_time: float = nan
    query1_colstore_sequential_time: float = nan
    query1_colstore_parallel_time: float = nan
    query3_rowstore_sequential_time: float = nan
    query3_colstore_sequential_time: float = nan
    query3_colstore_parallel_time: float = nan
    query3_binned_colstore_sequential_time: float = nan
    query3_binned_colstore_parallel_time: float = nan

time_obj = Results()

if len(sys.argv) != 3:
    print("please call this code with 2 arguments:")
    print("1. debug/measure")
    print("2. path/to/output/timing/file.csv ")
if sys.argv[1] == "debug":
    num_samples_and_iterations = 1
else:
    num_samples_and_iterations = 10
timing_file_path = sys.argv[2]


parse_file("/home/durst/big_dev/nba-movement-data/data/csv/cleaned_default_game.csv",
           "/home/durst/big_dev/nba-movement-data/data/shots/cleaned_shots_fixed.csv")
get_shots_for_game(21500116)

res = None


print("query 1: nearest defender within 2 seconds of each shot")
def query1():
    global res
    res = find_nearest_defender_at_each_shot(50)
time_res = benchmark(num_samples_and_iterations, num_samples_and_iterations, query1)
time_obj.query1_rowstore_sequential_time = time_res
print("time: " + str(time_res))
print(str(bucket_distances(res)))
print("first nearest at shot: " + str(res[0]))


print("query 2: find all trajectories of a player or the ball from position " +
      "(x,y,t) to (x+10,y+10,t+5), such that x,y, and t are free variables")
def query2():
    global res
    res = find_trajectories_no_fixed_origin(10.0, 10.0, 5)
time_res = benchmark(num_samples_and_iterations, num_samples_and_iterations, query2)
print("time: " + str(time_res))
print(str(res))


print("query 3: find all trajectories of a player or the ball from position " +
      "(x,y,t) to (x+10,y+10,t+5), such that x,y, and t are free variables")
def query3():
    global res
    res = find_trajectories_fixed_origin(
        {'start': {'x_loc': 70.0, 'y_loc': 16.0}, 'end': {'x_loc': 90.0, 'y_loc': 32.0}},
        {'start': {'x_loc': 71.9, 'y_loc': 24.9}, 'end': {'x_loc': 72.1, 'y_loc': 25.1}},
        5, 25)
time_res = benchmark(num_samples_and_iterations, num_samples_and_iterations, query3)
time_obj.query3_rowstore_sequential_time = time_res
print("time: " + str(time_res))
print(str(len(res)))
with open(timing_file_path, "a") as f:
    f.write(f"Cython,{time_obj.query1_rowstore_sequential_time*1e3:.2f}," +
            f"{time_obj.query1_colstore_sequential_time*1e3:.2f},{time_obj.query1_colstore_parallel_time*1e3:.2f}," +
            f"{time_obj.query3_rowstore_sequential_time*1e3:.2f},{time_obj.query3_colstore_sequential_time*1e3:.2f},"
            f"{time_obj.query3_colstore_parallel_time*1e3:.2f},{time_obj.query3_binned_colstore_sequential_time*1e3:.2f},"
            f"{time_obj.query3_binned_colstore_parallel_time*1e3:.2f}\n")

