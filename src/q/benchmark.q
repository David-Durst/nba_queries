start_time:`halide_bench 2:(`start_time;1)
get_duration:`halide_bench 2:(`get_duration;1)

// @kind function
// example of how to use: double:{2*2}; benchmark[2;4;double]
benchmark:{[samples;iterations;op]
    best: 0w;
    i:0;
    while[i<samples;
        start_time[];
        j:0;
        while[j<iterations;
            res: op[];
            j:j+1];
        end: get_duration[];
        best: min (best;end);
        i:i+1];
    (best % iterations; res)}

