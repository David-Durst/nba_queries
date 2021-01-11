system "l benchmark.q"
a:rand each 1000000#1000
b:rand each 1000000#1000
c:rand each 1000000#1000
d:rand each 1000000#1000

res:0
basic_f:{res:((a * b) + c) - d}
fused_f:{[x;y;z;zz] ((x * y) + z) - zz}'[a;b;c;d]
fused_f'[2;3;4;5]

basic[]
res
basic_res: benchmark[10;10;basic]
fused_res: benchmark[10;10;fused_f]

basic_res[0] % fused_res[0]

f:{sum exp x?1.0}
\t f each 2#1000000
\ts g'[2#1000000;2 3]
exit 0