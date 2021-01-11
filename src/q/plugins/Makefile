CXXFLAGS = -I. -shared -fPIC -DKXVER=3

.PHONY: halide_bench clean

halide_bench: halide_bench.so 

halide_bench.so: halide_bench.cpp 
	${CXX} ${CXXFLAGS} -o $@ $<

halide_bench_32: halide_bench_32.so 

halide_bench_32.so: halide_bench.cpp 
	${CXX} ${CXXFLAGS} -m32 -o $@ $<

clean:
	rm -f *.so
