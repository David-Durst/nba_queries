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
    `time`result!(best % iterations; res)}

compare_times:{[arg1;arg2]
    $[arg1[`result] ~ arg2[`result];
        [
            show " " sv ("arg1 time: ";string arg1[`time]);
            show " " sv ("arg2 time: ";string arg2[`time]);
            show " " sv ("difference: ";string arg1[`time] % arg2[`time];string "x");
        ];
        show "results differ"]
    }