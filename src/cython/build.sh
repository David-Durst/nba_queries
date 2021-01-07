python setup.py build_ext
if [ $? -eq 0 ] 
then
    python run_analysis.py
fi

