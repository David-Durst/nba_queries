# Why is inlining not happening
e5faae4c407e32163e78b44f543fd21345a23329 - https://github.com/David-Durst/nba_queries/blob/e5faae4c407e32163e78b44f543fd21345a23329/src/cpp/lib/clean/query15_players_in_paint_shot_clock.cpp#L57-L107
it was happening, I screwed up my boundary contiions, so I wasn't adding any values. This meant I added no time


# What's the right way to build a vector 
just push on it. There's no point in doing anything fancy
7137d33362d60872aa23625f3888e9f4374aa840 - https://github.com/David-Durst/nba_queries/blob/7137d33362d60872aa23625f3888e9f4374aa840/src/bench_cpp/main.cpp
it took 3.35788ms to int_array_alloc
it took 2.82483ms to int_vec_alloc
it took 19.1363ms to int_vec_push
it took 36.3287ms to struct_new_array_alloc
it took 33.3316ms to struct_malloc_array_alloc
it took 139.853ms to struct_vec_alloc
it took 21.1508ms to struct_vec_push
it took 22.3823ms to struct_ncv_push
