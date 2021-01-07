mkdir -p build
cd build
cmake ..
make -j4
./cleaned_nba_queries /home/durst/big_dev/nba-movement-data/data/csv/cleaned_default_game.csv /home/durst/big_dev/nba-movement-data/data/shots/shots_fixed.csv
