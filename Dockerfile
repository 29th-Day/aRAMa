FROM ubuntu:latest AS build

ARG git_args="--depth 1"

ENV DEVKITPRO=/opt/devkitpro
ENV DEVKITPPC=/opt/devkitpro/devkitPPC

WORKDIR /wiiu

# Install build tools

RUN apt update &&\
    apt install -y git build-essential wget cmake

# Install wut & compiler

RUN wget https://apt.devkitpro.org/install-devkitpro-pacman &&\
    chmod +x ./install-devkitpro-pacman &&\
    yes | ./install-devkitpro-pacman &&\
    ln -s /proc/self/mounts /etc/mtab
# https://devkitpro.org/wiki/devkitPro_pacman - info boud the mtab stuff (needed on WSL)

RUN dkp-pacman -Sy &&\
    dkp-pacman -S --noconfirm wiiu-dev

# Install wups

RUN git clone $git_args https://github.com/wiiu-env/WiiUPluginSystem.git &&\
    cd WiiUPluginSystem &&\
    make &&\
    make install

# install libkernel

RUN git clone $git_args https://github.com/wiiu-env/libkernel.git &&\
    cd libkernel &&\
    make &&\
    make install

# install libnotifications

RUN git clone $git_args https://github.com/wiiu-env/libnotifications.git &&\
    cd libnotifications &&\
    make &&\
    make install

# copy aRAMa into container

WORKDIR /wiiu/aRAMa

COPY . .

RUN cmake -S . -B build &&\
    cmake --build build

FROM scratch
COPY --from=build /wiiu/aRAMa/build/aRAMa.wps .

CMD ["sleep", "infinity"]


# docker build -t wups/aRAMa --output=<dir> <Dockerfile>
