mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
./cleaned_nba_queries /home/durst/big_dev/nba-movement-data/data/csv/cleaned_all_games.csv /home/durst/big_dev/nba-movement-data/data/shots/all_cleaned_shots_fixed.csv measure /home/durst/big_dev/nba-movement-data/data/csv/all_extra_game_data.csv  $(dirname $(pwd))/timing.csv
