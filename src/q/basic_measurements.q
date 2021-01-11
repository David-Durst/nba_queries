system "l benchmark.q"
a:rand each 1000000#1000
b:rand each 1000000#1000
c:rand each 1000000#1000
d:rand each 1000000#1000

basic1_f:{a * a}
fused1_f:{{[x] x * x}'[a]}
basic1_res: benchmark[4;4;basic1_f]
fused1_res: benchmark[4;4;fused1_f]
compare_times[basic1_res;fused1_res]



basic4_f:{((a * b) + c) - d}
fused4_f:{{[x;y;z;zz] ((x * y) + z) - zz}'[a;b;c;d]}
basic4_res: benchmark[4;4;basic4_f]
fused4_res: benchmark[4;4;fused4_f]
compare_times[basic4_res;fused4_res]


basic4_rep_f:{((a * a) + a) - a}
fused4_rep_f:{{[x] ((x * x) + x) - x}'[a]}
basic4_rep_res: benchmark[4;4;basic4_rep_f]
fused4_rep_res: benchmark[4;4;fused4_rep_f]
compare_times[basic4_rep_res;fused4_rep_res]

exit 0