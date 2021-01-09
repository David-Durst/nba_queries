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
COPY q q/
ENV QHOME "${HOME}/q/"
ENV PATH="$PATH:${QHOME}/l32/"
