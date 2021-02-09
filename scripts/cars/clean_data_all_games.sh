mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
./src/cpp/cars/preprocess_cars /home/durst/big_dev/fiodar_data/2_6_21/cleaned_moments.csv  \
  /home/durst/big_dev/fiodar_data/2_6_21/cleaned_moments.pb 2938677 \
  /home/durst/big_dev/fiodar_data/2_6_21/id_2*.csv

