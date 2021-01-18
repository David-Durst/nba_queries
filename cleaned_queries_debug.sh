mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j4
./cleaned_nba_queries /home/durst/big_dev/nba-movement-data/data/csv/cleaned_debug_games.csv /home/durst/big_dev/nba-movement-data/data/shots/cleaned_shots_fixed.csv debug $(dirname $(pwd))/timing.csv
