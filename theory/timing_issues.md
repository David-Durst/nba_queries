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

above write 7821357 elements of 32bytes each. 

multiply by 2 as a write is a read and then a write to dram, so:
7821357 * 32 *2 bytes  = 500 MB / 0.021 s = 24 GB/s

This is 24 GB/s on a peak of 68 GB/s machine, not terrible but not perfect.
Maybe I'd eget up to 30 GB/s, but not much farther than that, so the above results are fine as a speed of light for inserting objects

# what's fastest i can get query 15?
9c03a9f41db21e27a76ec3ed9944da636ff52c8b - https://github.com/David-Durst/nba_queries/blob/9c03a9f41db21e27a76ec3ed9944da636ff52c8b/src/cpp/lib/clean/query15_players_in_paint_shot_clock.cpp#L57-L85
The timing of par part and then the sequential merging is:
running query 15 cleaned, parallel
it took 183.589ms to do par part
it took 236.332ms to do seq seqt
it took 65.3131ms to do par part
it took 61.9331ms to do seq seqt
it took 61.5688ms to do par part
it took 61.2673ms to do seq seqt
it took 72.3804ms to do par part
it took 61.1837ms to do seq seqt
it took 69.6268ms to do par part
it took 61.6564ms to do seq seqt
it took 70.8256ms to do par part
it took 60.7517ms to do seq seqt
it took 70.4063ms to do par part
it took 68.0406ms to do seq seqt
it took 74.8744ms to do par part
it took 66.8838ms to do seq seqt
it took 68.6452ms to do par part
it took 67.0085ms to do seq seqt
it took 70.5649ms to do par part
it took 62.1243ms to do seq seqt
it took 68.1727ms to do par part
it took 61.962ms to do seq seqt
it took 67.9074ms to do par part
it took 61.474ms to do seq seqt
it took 67.363ms to do par part
it took 62.8579ms to do seq seqt
it took 70.6233ms to do par part
it took 63.7806ms to do seq seqt
it took 72.4538ms to do par part
it took 63.7509ms to do seq seqt
it took 69.9215ms to do par part
it took 62.7138ms to do seq seqt
it took 67.1311ms to do par part
it took 61.5586ms to do seq seqt
it took 70.3646ms to do par part
it took 59.8162ms to do seq seqt
it took 69.1068ms to do par part
it took 59.457ms to do seq seqt
it took 69.5288ms to do par part
it took 60.9607ms to do seq seqt
it took 65.9725ms to do par part
it took 61.4324ms to do seq seqt
it took 65.3121ms to do par part
it took 61.418ms to do seq seqt
it took 66.4208ms to do par part
it took 62.7096ms to do seq seqt
it took 67.3941ms to do par part
it took 59.2995ms to do seq seqt
it took 67.2585ms to do par part
it took 59.1378ms to do seq seqt

the par part is a for loop over all 45792000*11 moments-player combinations with an inner if loop where I'm selecting the 7821357 and inserting them into vectors (one vector per thread)
the seq part is merging those vectors so a single vector is returned

## why par part is at speed of light
par part is 65-70 ms - this is 15-20 ms for loop (which I observed when inserting nothing). 
recording the results is 21 ms, but thats just the write, missing reading the results from memory
So with 20ms for for loop, 21 for writes, and multiply by 1.5 because write is a read and a write the read is just a read, so only 50% of the memory requirements
20+21*1.5 = 51.5
at 70 / 51.5 = 1.36, or 36% off speed of light. Reason for that is memory reads aren't contiguous due to if, so may be more than 1.5x (i.e. if it's 2.3x) At 2.3x its 68.3ms and then times make total sense

so part is speed of light. The question is whether to merge more efficiently. This would be 120ms -> 90ms. Alternatively, do I just want an unmerged result?