cd build
cmake ..
make -j4
gdb -ex="catch throw" -ex=r --args tests -b
