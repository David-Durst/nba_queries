system "l benchmark.q"
results_file_path: .z.x[0]
a:100000000?1000i
b:100000000?1000i
c:100000000?1000i
d:100000000?1000i
vector_len: 100000000.0

toBW:{[bytes;sec] (((bytes % 1024.) % 1024.) % 1024.) % sec}
toGFLOPS:{[ops;sec] (ops % 1e9) % sec};

results:`sequential_square_time`sequential_square_bw`sequential_square_gops`four_ops_unfused_time`four_ops_unfused_bw`four_ops_unfused_gops`four_ops_one_input_unfused_time`four_ops_one_input_unfused_bw`four_ops_one_input_unfused_gops!(0.;0.;0.;0.;0.;0.;0.;0.;0.)

show "minimum benchmark time"
basic1_res: benchmark[4;4;{1+1}]

show "running 1 operator experiments"
show ""
basic1_f:{a * a}
basic1_res: benchmark[4;4;basic1_f]
square_bytes_traffic: 3 * vector_len * 4
square_total_ops: vector_len
results[`sequential_square_time]: basic1_res[`time] * 1e3
results[`sequential_square_bw]: toBW[square_bytes_traffic;basic1_res[`time]]
results[`sequential_square_gops]: toGFLOPS[square_total_ops;basic1_res[`time]]

show "running 4 operator experiments"
show ""
basic4_f:{((a * b) + c) - d}
basic4_res: benchmark[4;4;basic4_f]
four_ops_unfused_bytes_traffic: 10 * vector_len * 4
four_ops_total_ops: 4 * vector_len
results[`four_ops_unfused_time]: basic4_res[`time] * 1e3
results[`four_ops_unfused_bw]: toBW[four_ops_unfused_bytes_traffic;basic4_res[`time]]
results[`four_ops_unfused_gops]: toGFLOPS[four_ops_total_ops;basic4_res[`time]]

show "running 1 operator repeated 4 times experiments"
show ""
basic4_rep_f:{((a * a) + a) - a}
basic4_rep_res: benchmark[4;4;basic4_rep_f]
four_ops_one_input_unfused_bytes_traffic: 10 * vector_len * 4;
results[`four_ops_one_input_unfused_time]: basic4_rep_res[`time] * 1e3
results[`four_ops_one_input_unfused_bw]: toBW[four_ops_one_input_unfused_bytes_traffic;basic4_rep_res[`time]]
results[`four_ops_one_input_unfused_gops]: toGFLOPS[four_ops_total_ops;basic4_rep_res[`time]]

nan:"nan,nan,nan"
sequential_square_res: "," sv string (results[`sequential_square_time];results[`sequential_square_bw];results[`sequential_square_gops])
four_ops_unfused_res: "," sv string (results[`four_ops_unfused_time];results[`four_ops_unfused_bw];results[`four_ops_unfused_gops])
four_ops_one_input_unfused_res: "," sv string (results[`four_ops_one_input_unfused_time];results[`four_ops_one_input_unfused_bw];results[`four_ops_one_input_unfused_gops])
final_str_res: "," sv (enlist "Q";sequential_square_res;nan;four_ops_unfused_res;nan;four_ops_one_input_unfused_res;nan)

h:hopen (hsym `$results_file_path)
neg[h] enlist final_str_res
hclose h

exit 0
