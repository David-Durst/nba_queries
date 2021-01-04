mkdir -p build
cd build
cmake ..
make -j4
gdb -ex=r --args ./nba_queries /home/durst/big_dev/nba-movement-data/data/csv/default_game.csv /home/durst/big_dev/nba-movement-data/data/shots/shots_fixed.csv 3
