FROM continuumio/anaconda3:latest

RUN apt clean; apt update; apt upgrade -y
ENV DEBIAN_FRONTEND="noninteractive" TZ="America/New_York"
RUN apt install -y curl vim neovim git g++ rlwrap openssh-server sudo make libc6-i386 gcc-multilib g++-multilib 

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

# setup code
RUN conda config --prepend pkgs_dirs /home/durst/.conda/pkgs
RUN conda create -y -n nba_queries python=3.8
RUN git clone https://github.com/David-Durst/nba_queries.git
WORKDIR /home/durst/nba_queries/src/cython
#SHELL ["conda", "run", "-n", "nba_queries", "pip", "install", "-e", "."]
RUN conda run -n nba_queries pip install -e .
WORKDIR /home/durst/nba_queries

# config bash setings
RUN echo "conda activate nba_queries" >> /root/.bashrc && \
    echo "export PATH=$PATH:/home/durst/ispc-v1.14.1-linux/bin" >> /root/.bashrc && \
    echo "alias q='rlwrap -r ${QHOME}/l32/q'" >> /root/.bashrc
