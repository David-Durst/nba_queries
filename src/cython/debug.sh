python setup.py build_ext
if [ $? -eq 0 ] 
then
    python run_analysis.py debug $(dirname $(dirname $(pwd)))/timing.csv
fi

