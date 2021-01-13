mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
./cleaned_nba_queries /home/durst/big_dev/nba-movement-data/data/csv/cleaned_default_game.csv /home/durst/big_dev/nba-movement-data/data/shots/cleaned_shots_fixed.csv measure $(dirname $(pwd))/timing.csv
