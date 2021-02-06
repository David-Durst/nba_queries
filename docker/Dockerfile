FROM continuumio/anaconda3:latest

RUN apt clean; apt update; apt upgrade -y
ENV DEBIAN_FRONTEND="noninteractive" TZ="America/New_York"
RUN apt install -y curl vim neovim git g++ rlwrap openssh-server sudo make libc6-i386 gcc-multilib g++-multilib valgrind perf-tools-unstable

RUN mkdir /home/durst
WORKDIR /home/durst

#setup kdb
ENV QHOME "/home/durst/q/"
ENV PATH="$PATH:/home/durst/q/l32/"

# setup cmake
RUN mkdir cmake
WORKDIR /home/durst/cmake
RUN wget https://github.com/Kitware/CMake/releases/download/v3.19.2/cmake-3.19.2-Linux-x86_64.sh && \
    chmod u+x cmake-3.19.2-Linux-x86_64.sh && \
    ./cmake-3.19.2-Linux-x86_64.sh --skip-license
ENV PATH="$PATH:/home/durst/cmake/bin/"
WORKDIR /home/durst

RUN wget https://github.com/ispc/ispc/releases/download/v1.14.1/ispc-v1.14.1-linux.tar.gz && \
    tar -xvf ispc-v1.14.1-linux.tar.gz

# setup python
RUN conda config --prepend pkgs_dirs /home/durst/.conda/pkgs
RUN conda create -y -n nba_queries python=3.8

# setup Capn Proto
RUN apt install -y pkg-config
RUN curl -O https://capnproto.org/capnproto-c++-0.8.0.tar.gz && \
    tar zxf capnproto-c++-0.8.0.tar.gz && \
    cd capnproto-c++-0.8.0 && \
    ./configure && \
    make -j6 check && \
    make install
WORKDIR /home/durst

# setup code
RUN git clone https://github.com/David-Durst/nba_queries.git
WORKDIR /home/durst/nba_queries/src/cython
#SHELL ["conda", "run", "-n", "nba_queries", "pip", "install", "-e", "."]
RUN conda run -n nba_queries pip install -e .
WORKDIR /home/durst/nba_queries

# config bash setings
RUN echo "conda activate nba_queries" >> /root/.bashrc && \
    echo "export PATH=$PATH:/home/durst/.local/bin:/home/durst/ispc-v1.14.1-linux/bin" >> /root/.bashrc && \
    echo "alias q='taskset 1,2,3,4,5,6 rlwrap -r ${QHOME}/l64/q'" >> /root/.bashrc && \
    echo "alias perf='perf_4.19'" >> /root/.bashrc
