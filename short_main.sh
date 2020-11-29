cd build
cmake ..
make -j4
./nba_queries /home/durst/big_dev/nba-movement-data/data/csv/shortened_game.csv /home/durst/big_dev/nba-movement-data/data/shots/shots_fixed.csv
