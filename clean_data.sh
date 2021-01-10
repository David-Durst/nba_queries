mkdir -p build
cd build
cmake ..
make -j4
./preprocess_moments /home/durst/big_dev/nba-movement-data/data/csv/default_game.csv /home/durst/big_dev/nba-movement-data/data/csv/cleaned_default_game.csv /home/durst/big_dev/nba-movement-data/data/shots/shots_fixed.csv /home/durst/big_dev/nba-movement-data/data/shots/cleaned_shots_fixed.csv
