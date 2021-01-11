1. [Download the 32 bit](https://kx.com/download/) version of kdb and unzip it as `q/` within the `docker/` folder
1. Build the 32-bit `halide_bench_32.so` and copy it as `halide_bench.so` to `docker/q/l32`
2. Run `./run.sh` to start the container
3. Run `./attach.sh` to attach to the container
4. Press CTRL+P and the CTRL+Q to detach from the container without stopping it
