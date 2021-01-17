system "l benchmark.q"
a:1000000?1000
b:1000000?1000
c:1000000?1000
d:1000000?1000

show "running 1 operator experiments"
show ""
basic1_f:{a * a}
fused1_f:{{[x] x * x}'[a]}
parallel1_f:{{[x] x * x} peach a}
basic1_res: benchmark[4;4;basic1_f]
fused1_res: benchmark[4;4;fused1_f]
parallel1_res: benchmark[4;4;parallel1_f]
show "arg1: basic1, arg2: fused1"
compare_times[basic1_res;fused1_res]
show "arg1: fused1, arg2: parallel1"
compare_times[fused1_res;parallel1_res]

show "running 4 operator experiments"
show ""
basic4_f:{((a * b) + c) - d}
fused4_f:{{[x;y;z;zz] ((x * y) + z) - zz}'[a;b;c;d]}
flipped_input: flip(a;b;c;d)
parallel4_f:{.[{[x;y;z;zz] ((x * y) + z) - zz};] peach flipped_input}
basic4_res: benchmark[4;4;basic4_f]
fused4_res: benchmark[4;4;fused4_f]
parallel4_res: benchmark[4;4;parallel4_f]
show "arg1: basic4, arg2: fused4"
compare_times[basic4_res;fused4_res]
show "arg1: fused1, arg2: parallel1"
compare_times[fused4_res;parallel4_res]

show "running 1 operator repeated 4 times experiments"
show ""
basic4_rep_f:{((a * a) + a) - a}
fused4_rep_f:{{[x] ((x * x) + x) - x}'[a]}
parallel4_rep_f:{{[x] ((x * x) + x) - x} peach a}
basic4_rep_res: benchmark[4;4;basic4_rep_f]
fused4_rep_res: benchmark[4;4;fused4_rep_f]
parallel4_rep_res: benchmark[4;4;parallel4_rep_f]
show "arg1: basic4_rep, arg2: fused4_rep"
compare_times[basic4_rep_res;fused4_rep_res]
show "arg1: fused4_rep, arg2: parallel4_rep"
compare_times[fused4_rep_res;parallel4_rep_res]

exit 0
