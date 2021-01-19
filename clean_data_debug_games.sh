mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j4
./preprocess_moments /home/durst/big_dev/nba-movement-data/data/csv/cleaned_debug_games.csv /home/durst/big_dev/nba-movement-data/data/shots/shots_fixed.csv /home/durst/big_dev/nba-movement-data/data/shots/cleaned_shots_fixed.csv /home/durst/big_dev/nba-movement-data/data/csv/debug_extra_game_data.csv /home/durst/big_dev/nba-movement-data/data/csv/00215001*6.csv

