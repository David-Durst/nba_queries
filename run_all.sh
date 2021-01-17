./cleaned_queries_measure.sh
cd src/cython
./measure.sh
cd ../ispc
./run.sh
cd ../q
source ./run_basic.sh
cd ../..
