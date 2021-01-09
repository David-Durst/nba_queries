FROM continuumio/anaconda3:latest

RUN apt clean; apt update; apt upgrade -y
ENV DEBIAN_FRONTEND="noninteractive" TZ="America/New_York"
RUN apt install -y curl vim neovim git cmake g++ rlwrap openssh-server sudo

RUN adduser --disabled-password --gecos '' db
RUN adduser db sudo
RUN echo '%sudo ALL=(ALL) NOPASSWD:ALL' >> /etc/sudoers
USER db
WORKDIR /home/db

#setup kdb
COPY --chown=db:db q q/
ENV QHOME "/home/db/q/"
#ENV PATH="$PATH:/home/db/q/l32/"

# setup code
RUN git clone https://github.com/David-Durst/nba_queries.git
