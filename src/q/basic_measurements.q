system "l benchmark.q"
a:rand each 1000000#1000
b:rand each 1000000#1000
c:rand each 1000000#1000
d:rand each 1000000#1000

basic_f:{res:((a * b) + c) - d}
fused_f:{[x;y;z;zz] ((x * y) + z) - zz}'[a;b;c;d]
fused_f'[2;3;4;5]


basic_res: benchmark[10;10;basic_f]
fused_res: benchmark[10;10;fused_f]
compare_times[basic_res;fused_res]
$[basic_res[`result] ~ fused_res[`result];
    [
        show " " sv ("basic time: ";string basic_res[`time]);
        show " " sv ("fused time: ";string fused_res[`time]);
        show " " sv ("difference: ";string basic_res[`time] % fused_res[`time];string "x");
    ];
    show "results differ"]


\t a*a
\t {x*x} each a
\t {x*x}'[a]
(a*a) ~ ({x*x}'[a])

basic[]
res

\t basic_f[]
\t fused_f

basic_res[0] % fused_res[0]

f:{sum exp x?1.0}
\t f each 2#1000000
\ts g'[2#1000000;2 3]
exit 0