FROM continuumio/anaconda3:latest

RUN apt clean; apt update; apt upgrade -y
ENV DEBIAN_FRONTEND="noninteractive" TZ="America/New_York"
RUN apt install -y curl vim neovim git g++ rlwrap openssh-server sudo make

RUN adduser --disabled-password --gecos '' durst
RUN adduser durst sudo
RUN echo '%sudo ALL=(ALL) NOPASSWD:ALL' >> /etc/sudoers
USER durst
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

# setup code
RUN git clone https://github.com/David-Durst/nba_queries.git
RUN conda config --prepend pkgs_dirs /home/durst/.conda/pkgs
RUN conda create -y -n nba_queries python=3.8
WORKDIR /home/durst/nba_queries/src/cython
SHELL ["conda", "run", "-n", "nba_queries", "pip", "install", "-e", "."]
WORKDIR /home/durst/nba_queries
