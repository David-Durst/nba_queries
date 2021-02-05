mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j4
./generate_nba_proto /home/durst/big_dev/nba-movement-data/data/csv/cleaned_debug_games.csv /home/durst/big_dev/nba-movement-data/data/csv/cleaned_debug_games.pb

