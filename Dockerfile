FROM ubuntu:latest

ENV BUILD_TYPE=Release

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    gcc

RUN apt remove --purge --auto-remove cmake -y
RUN apt update && \
  apt install -y software-properties-common lsb-release && \
  apt clean all
RUN wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc 2>/dev/null | gpg --dearmor - | tee /etc/apt/trusted.gpg.d/kitware.gpg >/dev/null
RUN apt-key adv --keyserver keyserver.ubuntu.com --recv-keys 42D5A192B819C5DA
RUN apt-add-repository "deb https://apt.kitware.com/ubuntu/ $(lsb_release -cs) main"
RUN apt update && \
  apt install kitware-archive-keyring && \
  rm /etc/apt/trusted.gpg.d/kitware.gpg
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake 

RUN gcc --version

ADD . .

RUN rm -rf build
RUN cmake -B build -DCMAKE_BUILD_TYPE=${BUILD_TYPE}
RUN cmake --build build --config ${BUILD_TYPE}

WORKDIR build
CMD ["ctest", "-C", "${BUILD_TYPE}", "--verbose"]
