mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j4
mkdir -p $(dirname $(pwd))/samples
./concepts /home/durst/big_dev/nba-movement-data/data/csv/cleaned_debug_games.pb /home/durst/big_dev/nba-movement-data/data/shots/debug_cleaned_shots_fixed.csv debug  /home/durst/big_dev/nba-movement-data/data/csv/debug_extra_game_data.csv $(dirname $(pwd))/timing.csv $(dirname $(pwd))/samples
