mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug -DENABLE_CALLGRIND=ON
make -j4
valgrind --tool=callgrind --instr-atstart=no ./cleaned_nba_queries /home/durst/big_dev/nba-movement-data/data/csv/cleaned_debug_games.pb /home/durst/big_dev/nba-movement-data/data/shots/cleaned_shots_fixed.csv debug  /home/durst/big_dev/nba-movement-data/data/csv/debug_extra_game_data.csv $(dirname $(pwd))/timing.csv
