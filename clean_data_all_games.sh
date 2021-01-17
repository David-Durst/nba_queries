mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
./preprocess_moments /home/durst/big_dev/nba-movement-data/data/csv/cleaned_all_games.csv /home/durst/big_dev/nba-movement-data/data/shots/shots_fixed.csv /home/durst/big_dev/nba-movement-data/data/shots/cleaned_shots_fixed.csv /home/durst/big_dev/nba-movement-data/data/csv/*.csv

