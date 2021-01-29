#include <algorithm>
#include <iostream>
#include <iomanip>
#include <vector>
#include <fstream>
#include <string>
#include <thread>
#include <chrono>
#include <numeric>
#include <limits>
#include <map>
#include <sys/types.h>
#include <omp.h>
#include <unistd.h>
#include "check_distances.h"
#include "find_trajectories.h"
#include "clean_queries.h"
#include "benchmark.h"
#include "load_data.h"
#include "query_structs.h"
#include "col_stores.h"
#ifdef CALLGRIND
#include <valgrind/callgrind.h>
#endif
using std::string;

struct results {
    double query1_rowstore_sequential_time;
    double query1_colstore_sequential_time;
    double query1_colstore_parallel_time;
    double query3_rowstore_sequential_time;
    double query3_colstore_sequential_time;
    double query3_colstore_parallel_time;
    double query3_binned_colstore_sequential_time;
    double query3_binned_colstore_parallel_time;
    double query12_colstore_parallel_time;
    double query13_colstore_parallel_time;
    double query14_colstore_parallel_time;
    double query14_binned_colstore_parallel_time;
    double query14_binned_with_time_colstore_parallel_time;
    double query15_colstore_parallel_time;
    double query15_binned_colstore_parallel_time;
};

int main(int argc, char * argv[]) {
    vector<cleaned_moment> moments;
    moment_col_store * moments_col;
    vector<cleaned_shot> shots;
    shot_col_store * shots_col;
    court_bins * bins;
    court_and_game_clock_bins * time_bins;
    vector<shot_and_player_data> shots_and_players_seq, shots_and_players_par, shots_and_players;
    list<shot_and_player_data> shots_and_players_list;
    vector<trajectory_data> trajectories;
    trajectory_data * trajectories_arr = new trajectory_data[1];
    vector<extra_game_data> extra_data;
    results res;
    if (argc != 6) {
        std::cout << "please call this code with 4 arguments: " << std::endl;
        std::cout << "1. path/to/cleaned_moments_file.csv " << std::endl;
        std::cout << "2. path/to/shots_file.csv " << std::endl;
        std::cout << "3. debug/measure " << std::endl;
        std::cout << "4. path/to/extra_data_file.csv " << std::endl;
        std::cout << "5. path/to/output/timing/file.csv " << std::endl;
    }
    string moments_file_path = argv[1], shots_file_path = argv[2], run_type = argv[3], extra_data_file_path = argv[4],
        timing_file_path = argv[5];
    int num_samples_and_iterations = (run_type.compare("debug") == 0) ? 1 : 10;
    std::fstream moments_file, shots_file, timing_file, extra_data_file;

    // load the cleaned moments
    std::cout << "loading cleaned moments file: " << moments_file_path << std::endl;
    moments_file.open(moments_file_path);
    load_cleaned_moment_rows_vec(moments_file, moments);
    moments_file.close();
    std::cout << "moments size: " << moments.size() << std::endl;
    moments_col = new moment_col_store(moments);

    // bin the moments
    bins = new court_bins(moments_col);
    time_bins = new court_and_game_clock_bins(moments_col);

    // load the shots
    std::cout << "loading shots file: " << shots_file_path << std::endl;
    shots_file.open(shots_file_path);
    load_cleaned_shot_rows_vec(shots_file, shots);
    shots_file.close();
    std::cout << "shots size: " << shots.size() << std::endl;
    shots_col = new shot_col_store(shots);

    // load the extra game data
    std::cout << "loading extra game data file: " << extra_data_file_path << std::endl;
    extra_data_file.open(extra_data_file_path);
    load_extra_game_data_rows(extra_data_file, extra_data);
    extra_data_file.close();
    std::cout << "extra game data size: " << extra_data.size() << std::endl;


    res.query1_rowstore_sequential_time = std::numeric_limits<double>::quiet_NaN();

    std::cout << "running query 1 cleaned, sequential" << std::endl;
    double min_time = Halide::Tools::benchmark(num_samples_and_iterations, num_samples_and_iterations, [&]() {
        shots_and_players_list.clear();
        find_nearest_defender_at_each_shot_clean(moments_col, shots_col, &shots_and_players_list, extra_data, 50, false);
    });
    shots_and_players_list.to_vector(shots_and_players_seq);
    printf("compute time: %gms\n", min_time * 1e3);
    std::cout << "shot_and_players size: " << shots_and_players_seq.size() << std::endl;
    vector<shot_distance_bucket> buckets = bucket_shots_by_distance(shots_and_players_seq);
    std::cout << "distance,num_shot_made,num_shot_attempt,percent_made" << std::endl;
    int num_buckets = 0;
    for (const auto & b : buckets) {
        print_shot_distance_bucket_csv(std::cout, b);
        std::cout << std::endl;
        if (num_buckets > 30) {
            break;
        }
        num_buckets++;
    }
    std::cout << "first nearest at shot: " << shots_and_players_seq.at(0) << std::endl;
    res.query1_colstore_sequential_time = min_time;

    std::cout << "running query 1 cleaned, parallel" << std::endl;
    min_time = Halide::Tools::benchmark(num_samples_and_iterations, num_samples_and_iterations, [&]() {
        shots_and_players_list.clear();
        find_nearest_defender_at_each_shot_clean(moments_col, shots_col, &shots_and_players_list, extra_data, 50, true);
    });
    shots_and_players_list.to_vector(shots_and_players_par);
    printf("compute time: %gms\n", min_time * 1e3);
    std::cout << "shot_and_players size: " << shots_and_players_par.size() << std::endl;
    buckets = bucket_shots_by_distance(shots_and_players_par);
    std::cout << "distance,num_shot_made,num_shot_attempt,percent_made" << std::endl;
    num_buckets = 0;
    for (const auto & b : buckets) {
        print_shot_distance_bucket_csv(std::cout, b);
        std::cout << std::endl;
        if (num_buckets > 30) {
            break;
        }
        num_buckets++;
    }
    std::cout << "first nearest at shot: " << shots_and_players_par.at(0) << std::endl;
    res.query1_colstore_parallel_time = min_time;
    coordinate_range origin{{64.5f,24.9f,0}, {65.5f,25.1f, 0}};
    coordinate_range destination{{70.0f,16.0f,0}, {90.0f,32.0f, 0}};
    int64_t elements_in_bins = 0;
    int64_t elements_in_region = 0;
    const std::vector<int>& origin_bins = court_bins::get_bins_in_region(origin);
    for (int player_num = 0; player_num < bins->players_indices_in_bins.size(); player_num++) {
        long int player_id = bins->player_ids[player_num];
        // all trajectory starts for the current player
        for (const auto &src_bin : origin_bins) {
            for (const player_pointer *src_moment = bins->bin_start(player_id, src_bin);
                 src_moment != bins->bin_end(player_id, src_bin); src_moment++) {
                elements_in_bins++;
                if (point_intersect_no_time(&origin, moments_col->x_loc[src_moment->player_index][src_moment->moment_index],
                                            moments_col->y_loc[src_moment->player_index][src_moment->moment_index])) {
                    elements_in_region++;
                }
            }
        }
    }
    std::cout << "player_pointers pairs in bins: " << elements_in_bins << std::endl;
    std::cout << "player_pointers pairs in region: " << elements_in_region << std::endl;
    std::cout << "size of player_pointers: " << sizeof(player_pointer) << std::endl;
    /*
    std::cout << "running query 3 cleaned with colstore, sequential" << std::endl;
    min_time = Halide::Tools::benchmark(num_samples_and_iterations, num_samples_and_iterations, [&]() {
        trajectories_list.clear();
        find_trajectories_fixed_origin_clean(moments_col, &trajectories_list, origin, destination, 5, 25, false);
    });
    trajectories_list.to_vector(trajectories);
    printf("compute time: %gms\n", min_time * 1e3);
    std::cout << "trajectories size: " << trajectories.size() << std::endl;
     */
    res.query3_colstore_sequential_time = -1; //min_time

    std::cout << "running query 3 cleaned with colstore, parallel" << std::endl;
    std::cout << "trajectory data size: " << sizeof(trajectory_data) << std::endl;

    min_time = Halide::Tools::benchmark(num_samples_and_iterations, num_samples_and_iterations, [&]() {
        trajectories.clear();
        find_trajectories_fixed_origin_clean(moments_col, trajectories, origin, destination, 2, 25, true);
    });
    printf("compute time: %gms\n", min_time * 1e3);
    std::cout << "trajectories size: " << trajectories.size() << std::endl;
    std::cout << "first trajectory: " << trajectories.at(0) << std::endl;
    std::cout << "first trajectory starting index" << clock_fixed_point(trajectories.at(0).start_game_clock).time_to_index(extra_data, trajectories.at(0).game_num, trajectories.at(0).quarter) << std::endl;
    res.query3_colstore_parallel_time = min_time;

    std::cout << "running just outer loop of query 3 cleaned with colstore, parallel" << std::endl;

    min_time = Halide::Tools::benchmark(num_samples_and_iterations, num_samples_and_iterations, [&]() {
        trajectories.clear();
        find_trajectories_fixed_origin_clean_just_outer(moments_col, trajectories, origin, destination, 2, 25, true);
    });
    printf("compute time: %gms\n", min_time * 1e3);
    /*
    std::cout << "running query 3 cleaned and binned with colstore, sequential" << std::endl;
    min_time = Halide::Tools::benchmark(num_samples_and_iterations, num_samples_and_iterations, [&]() {
        trajectories_list.clear();
        find_trajectories_fixed_origin_clean_binned(moments_col, bins, &trajectories_list, origin, destination, 5, 25, false);
    });
    trajectories_list.to_vector(trajectories);
    printf("compute time: %gms\n", min_time * 1e3);
    std::cout << "trajectories size: " << trajectories.size() << std::endl;
     */
    res.query3_binned_colstore_sequential_time = -1; //min_time;

    std::cout << "running query 3 cleaned and binned with colstore, parallel" << std::endl;

#ifdef CALLGRIND
    CALLGRIND_START_INSTRUMENTATION;
    CALLGRIND_TOGGLE_COLLECT;
    std::cout << "starting collect" << std::endl;
#endif
    min_time = Halide::Tools::benchmark(num_samples_and_iterations, num_samples_and_iterations, [&]() {
        trajectories.clear();
        find_trajectories_fixed_origin_clean_binned(moments_col, bins, trajectories, origin, destination, 2, 25, true);
    });
#ifdef CALLGRIND
    CALLGRIND_TOGGLE_COLLECT;
    CALLGRIND_STOP_INSTRUMENTATION;
#endif
    printf("compute time: %gms\n", min_time * 1e3);
    std::cout << "trajectories size: " << trajectories.size() << std::endl;
    std::cout << "first trajectory: " << trajectories.at(0) << std::endl;
    res.query3_binned_colstore_parallel_time = min_time;

    std::cout << "running just outer loop of query 3 cleaned and binned with colstore, parallel" << std::endl;

    min_time = Halide::Tools::benchmark(num_samples_and_iterations, num_samples_and_iterations, [&]() {
        trajectories.clear();
        find_trajectories_fixed_origin_clean_binned_just_outer(moments_col, bins, trajectories, origin, destination, 2, 25, true);
    });
    printf("compute time: %gms\n", min_time * 1e3);

    std::cout << "running min time of query 3 cleaned and binned with colstore, parallel" << std::endl;

    min_time = Halide::Tools::benchmark(num_samples_and_iterations, num_samples_and_iterations, [&]() {
        trajectories.clear();
        find_trajectories_fixed_origin_clean_binned_min_time(moments_col, bins, trajectories, origin, destination, 2, 25, true);
    });
    printf("compute time: %gms\n", min_time * 1e3);

    /*
    std::cout << "running query 3 cleaned with row store, sequential" << std::endl;
    min_time = Halide::Tools::benchmark(num_samples_and_iterations, num_samples_and_iterations, [&]() {
        trajectories.clear();
        find_trajectories_fixed_origin_clean_rowstore(moments, trajectories, origin, destination, 5, 25);
    });
    printf("compute time: %gms\n", min_time * 1e3);
    std::cout << "trajectories size: " << trajectories.size() << std::endl;
     */
    res.query3_rowstore_sequential_time = -1; //min_time;
    coordinate_range paint0{{0.0f,16.0f,0.0}, {20.0f,32.0f, 60.0}};
    coordinate_range paint1{{70.0f,16.0f,0.0}, {90.0f,32.0f, 60.0}};
    std::cout << "running query 12 cleaned, parallel" << std::endl;
    min_time = Halide::Tools::benchmark(num_samples_and_iterations, num_samples_and_iterations, [&]() {
        shots_and_players.clear();
        // paul george is 202331
        is_paul_george_near_ball_in_paint_clean(moments_col, bins, shots_and_players, paint0, paint1, 202331);
    });
    printf("compute time: %gms\n", min_time * 1e3);
    std::cout << "shot_and_players size: " << shots_and_players.size() << std::endl;
    buckets = bucket_shots_by_distance(shots_and_players);
    std::cout << "distance,num_shots" << std::endl;
    num_buckets = 0;
    for (const auto & b : buckets) {
        std::cout << b.distance << "," << b.num_shot_attempts << std::endl;
        if (num_buckets > 30) {
            break;
        }
        num_buckets++;
    }
    std::cout << "first paul george in paint and ball moment: " << shots_and_players_par.at(0) << std::endl;
    res.query12_colstore_parallel_time = min_time;

    std::cout << "running query 13 cleaned, parallel" << std::endl;
    vector<ball_height_at_time> ball_heights;
    min_time = Halide::Tools::benchmark(num_samples_and_iterations, num_samples_and_iterations, [&]() {
        ball_heights.clear();
        get_end_game_moments(moments_col, extra_data, ball_heights, 24);
    });
    printf("compute time: %gms\n", min_time * 1e3);
    double sum_heights = 0;
    for (const auto& h : ball_heights) {
        sum_heights += h.height;
    }
    std::cout << "average height: " << sum_heights / ball_heights.size() << std::endl;
    res.query13_colstore_parallel_time = min_time;

    std::cout << "running query 14 cleaned, parallel" << std::endl;
    int query_14_endtime = 10;
    coordinate_range paint0_14{{0.0f,16.0f,0.0}, {20.0f,32.0f, 1.0*query_14_endtime}};
    coordinate_range paint1_14{{70.0f,16.0f,0.0}, {90.0f,32.0f, 1.0*query_14_endtime}};
    vector<players_in_paint_at_time> players_in_paint1, players_in_paint2, players_in_paint3;
    min_time = Halide::Tools::benchmark(num_samples_and_iterations, num_samples_and_iterations, [&]() {
        players_in_paint1.clear();
        get_players_in_paint_at_end(moments_col, extra_data, players_in_paint1, paint0_14, paint1_14, query_14_endtime);
    });
    printf("compute time: %gms\n", min_time * 1e3);
    std::cout << "num players in paint at end of game " << players_in_paint1.size() << std::endl;
    int64_t end_points_considered_by_time = 0;
    clock_fixed_point start_of_end(query_14_endtime);
    for (int i = 0; i < extra_data.size(); i++) {
        const extra_game_data &game_data = extra_data.at(i);
        for (int quarter = 1; quarter < 5 + game_data.num_ot_periods; quarter++) {
            for (int i = 0; i < NUM_PLAYERS_AND_BALL; i++) {
                for (int64_t time = start_of_end.time_to_index(extra_data, game_data.game_num, quarter);
                     moments_col->quarter[time] == quarter; time++) {
                    end_points_considered_by_time++;
                }
            }
        }
    }
    std::cout << "data points without binning: " << end_points_considered_by_time << std::endl;
    res.query14_colstore_parallel_time = min_time;

    std::cout << "running query 14 cleaned and binned, parallel" << std::endl;
    min_time = Halide::Tools::benchmark(num_samples_and_iterations, num_samples_and_iterations, [&]() {
        players_in_paint2.clear();
        get_players_in_paint_at_end_binned(moments_col, bins, extra_data, players_in_paint2, paint0_14, paint1_14, query_14_endtime);
    });
    printf("compute time: %gms\n", min_time * 1e3);
    std::cout << "num players in paint at end of game " << players_in_paint2.size() << std::endl;
    std::cout << "data points in bins: " << bins->get_elems_in_region(paint0_14) + bins->get_elems_in_region(paint1_14) << std::endl;
    res.query14_binned_colstore_parallel_time = min_time;

    std::cout << "running query 14 cleaned and binned with time, parallel" << std::endl;
    min_time = Halide::Tools::benchmark(num_samples_and_iterations, num_samples_and_iterations, [&]() {
        players_in_paint3.clear();
        get_players_in_paint_at_end_binned_with_time(moments_col, time_bins, extra_data, players_in_paint3, paint0_14, paint1_14, query_14_endtime);
    });
    printf("compute time: %gms\n", min_time * 1e3);
    std::cout << "num players in paint at end of game " << players_in_paint3.size() << std::endl;
    std::cout << "data points in bins: " << time_bins->get_elems_in_region(paint0_14) + time_bins->get_elems_in_region(paint1_14) << std::endl;

    std::cout << "running query 14 cleaned and binned with time fix par, parallel" << std::endl;
    min_time = Halide::Tools::benchmark(num_samples_and_iterations, num_samples_and_iterations, [&]() {
        players_in_paint3.clear();
        get_players_in_paint_at_end_binned_with_time_fix_par(moments_col, time_bins, extra_data, players_in_paint3, paint0_14, paint1_14, query_14_endtime);
    });
    printf("compute time: %gms\n", min_time * 1e3);
    std::cout << "num players in paint at end of game " << players_in_paint3.size() << std::endl;
    std::cout << "data points in bins: " << time_bins->get_elems_in_region(paint0_14) + time_bins->get_elems_in_region(paint1_14) << std::endl;
    res.query14_binned_with_time_colstore_parallel_time = min_time;


    int num_threads = omp_get_max_threads();
    double * temp_time = new double[num_threads * 4];
    for (int i = 0; i < num_threads * 4; i++) {
        temp_time[i] = 0;
    }

    std::cout << "running query 15 cleaned, parallel" << std::endl;
    min_time = Halide::Tools::benchmark(num_samples_and_iterations, num_samples_and_iterations, [&]() {
        players_in_paint1.clear();
        get_players_in_paint_shot_clock(moments_col, players_in_paint1, paint0_14, paint1_14, 2.0, temp_time);
    });
    printf("compute time: %gms\n", min_time * 1e3);
    std::cout << "num players in paint at end of game " << players_in_paint1.size() << std::endl;
    res.query15_colstore_parallel_time = min_time;
    for (int i = 0; i < 4; i++) {
        double total = 0;
        for (int j = 0; j < num_threads; j++) {
            total += temp_time[i * num_threads + j];
        }
        std::cout << "loop " << i << " time across all threads " << total << std::endl;
    }

    std::cout << "running query 15 no funcs cleaned, parallel" << std::endl;
    min_time = Halide::Tools::benchmark(num_samples_and_iterations, num_samples_and_iterations, [&]() {
        players_in_paint1.clear();
        get_players_in_paint_shot_clock_no_funcs(moments_col, players_in_paint1, paint0_14, paint1_14, 2.0);
    });
    printf("compute time: %gms\n", min_time * 1e3);

    std::cout << "running query 15 no ptr funcs cleaned, parallel" << std::endl;
    min_time = Halide::Tools::benchmark(num_samples_and_iterations, num_samples_and_iterations, [&]() {
        players_in_paint1.clear();
        get_players_in_paint_shot_clock_no_ptr_funcs(moments_col, players_in_paint1, paint0_14, paint1_14, 2.0);
    });
    printf("compute time: %gms\n", min_time * 1e3);

    std::cout << "running query 15 one paint cleaned, parallel" << std::endl;
    min_time = Halide::Tools::benchmark(num_samples_and_iterations, num_samples_and_iterations, [&]() {
        players_in_paint1.clear();
        get_players_in_paint_shot_clock_one_paint(moments_col, players_in_paint1, paint0_14, 2.0);
    });
    printf("compute time: %gms\n", min_time * 1e3);

    std::cout << "running query 15 one paint no vec cleaned, parallel" << std::endl;
    min_time = Halide::Tools::benchmark(num_samples_and_iterations, num_samples_and_iterations, [&]() {
        players_in_paint1.clear();
        get_players_in_paint_shot_clock_one_paint_no_vec(moments_col, players_in_paint1, paint0_14, 2.0);
    });
    printf("compute time: %gms\n", min_time * 1e3);

    std::cout << "running query 15 cleaned and binned, parallel" << std::endl;
    min_time = Halide::Tools::benchmark(num_samples_and_iterations, num_samples_and_iterations, [&]() {
        players_in_paint2.clear();
        get_players_in_paint_shot_clock_binned(moments_col, bins, players_in_paint2, paint0_14, paint1_14, 2.0);
    });
    printf("compute time: %gms\n", min_time * 1e3);
    std::cout << "num players in paint at end of game " << players_in_paint2.size() << std::endl;
    res.query15_binned_colstore_parallel_time = min_time;

    /*
    int num_hits = 0;
    for (int i = 0; i < players_in_paint3.size(); i++) {
       if (players_in_paint3[i].player_id == 201941 && players_in_paint3[i].moment_index == 17401)  {
           num_hits++;
       }
       if (num_hits > 1) {
           int x =1;
       }
    }

    std::sort(players_in_paint1.begin(), players_in_paint1.end(), [](players_in_paint_at_time pa, players_in_paint_at_time pb) { return pa.moment_index < pb.moment_index || (pa.moment_index == pb.moment_index && pa.player_id < pb.player_id) ; });
    std::sort(players_in_paint2.begin(), players_in_paint2.end(), [](players_in_paint_at_time pa, players_in_paint_at_time pb) { return pa.moment_index < pb.moment_index || (pa.moment_index == pb.moment_index && pa.player_id < pb.player_id) ;  });
    std::sort(players_in_paint3.begin(), players_in_paint3.end(), [](players_in_paint_at_time pa, players_in_paint_at_time pb) { return pa.moment_index < pb.moment_index || (pa.moment_index == pb.moment_index && pa.player_id < pb.player_id) ;  });
    std::map<int, int> p1_to_p3_index;
    for (int p3_index = 1; p3_index < players_in_paint3.size(); p3_index++) {
        bool found_moment = false;
        int p1_index = 0;
        players_in_paint_at_time p1, p3;
        p3 = players_in_paint3[p3_index];
        if (p3.player_id == players_in_paint3[p3_index-1].player_id && p3.moment_index == players_in_paint3[p3_index-1].moment_index) {
            std::cout << "repeat on index " << p3_index << std::endl;
        }
        for (p1_index = 0; players_in_paint1[p1_index].moment_index <= players_in_paint3[p3_index].moment_index && p1_index < players_in_paint1.size(); p1_index++) {
            if (players_in_paint1[p1_index].moment_index == players_in_paint3[p3_index].moment_index &&
                players_in_paint1[p1_index].player_id == players_in_paint3[p3_index].player_id) {
                found_moment = true;
                p1 = players_in_paint1[p1_index];
                break;
            }
        }
        if (p3_index > players_in_paint1.size()) {
            int y = 3;
        }
        if (!found_moment) {
            int x = 3;
        }
        if (p1_to_p3_index.count(p1_index) > 0) {
            std::cout << "p1 index" << p1_index << std::endl;
            std::cout << "old p3_index" << p1_to_p3_index.at(p1_index) << std::endl;
            std::cout << "new p3 index" << p3_index << std::endl;
        }
        p1_to_p3_index[p1_index] = p3_index;
    }
    */

    std::cout << "min time measured" << std::endl;

    min_time = Halide::Tools::benchmark(num_samples_and_iterations, num_samples_and_iterations, [&]() {
        return 2;
    });
    printf("compute time: %gms\n", min_time * 1e3);

    // write results
    std::cout << "writing to file: " << timing_file_path << std::endl;
    timing_file.open(timing_file_path, std::fstream::out);
    timing_file << "Language,Query1 Rowstore Sequential Time (ms),Query1 Colstore Sequential Time (ms),Query1 Colstore Parallel Time (ms),"
                << "Query3 Rowstore Sequential Time (ms),Query3 Colstore Sequential Time (ms),"
                << "Query3 Colstore Parallel Time (ms),Query3 Binned Colstore Sequential Time (ms),"
                << "Query3 Binned Colstore Parallel Time (ms),Query12 Colstore Parallel Time (ms),"
                << "Query13 Colstore Parallel Time (ms),Query14 Colstore Parallel Time (ms),"
                << "Query14 Binned Colstore Parallel Time (ms)" << "Query14 Binned With Time Colstore Parallel Time (ms)" << std::endl;
    timing_file << std::fixed << std::setprecision(2)
                << "CPP," << res.query1_rowstore_sequential_time*1e3 << "," << res.query1_colstore_sequential_time*1e3
                << "," << res.query1_colstore_parallel_time*1e3 << ","
                << res.query3_rowstore_sequential_time*1e3 << "," << res.query3_colstore_sequential_time*1e3 << ","
                << res.query3_colstore_parallel_time*1e3 << "," << res.query3_binned_colstore_sequential_time*1e3 << ","
                << res.query3_binned_colstore_parallel_time*1e3 << res.query12_colstore_parallel_time*1e3
                << res.query13_colstore_parallel_time*1e3 << res.query14_colstore_parallel_time*1e3
                << res.query14_binned_colstore_parallel_time*1e3 << res.query14_binned_colstore_parallel_time*1e3 << std::endl;
    timing_file.close();

    return 0;
}