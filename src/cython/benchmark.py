from typing import Callable
import time
import math

# port of https://github.com/halide/Halide/blob/master/tools/halide_benchmark.h
counter = time.perf_counter if time.get_clock_info('perf_counter').monotonic \
    else time.monotonic

def benchmark(samples: int, iterations: int, f: Callable[[None], None]) -> float:
    best = math.inf
    for _ in range(samples):
        start = counter()
        for _ in range(iterations):
            f()
        end = counter()
        best = min(best, start-end)
    return best / iterations
