CXXFLAGS = -I. -shared -fPIC -DKXVER=3

.PHONY: halide_bench clean install install32

halide_bench: halide_bench.so halide_bench_32.so

halide_bench.so: halide_bench.cpp 
	${CXX} ${CXXFLAGS} -o $@ $<

halide_bench32.so: halide_bench.cpp 
	${CXX} ${CXXFLAGS} -m32 -o $@ $<

clean:
	rm -f *.so

install:
	cp halide_bench.so ${QHOME}/l64/
	
install32:
	cp halide_bench32.so ${QHOME}/l32/
