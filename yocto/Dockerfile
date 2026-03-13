FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive
ENV TZ=Asia/Seoul

RUN apt-get update && apt-get install -y \
    gawk wget git diffstat unzip texinfo gcc build-essential \
    chrpath socat cpio python3 python3-pip python3-pexpect \
    xz-utils debianutils iputils-ping python3-git python3-jinja2 \
    libegl1-mesa libsdl1.2-dev pylint xterm python3-subunit mesa-common-dev \
    zstd liblz4-tool file locales libacl1 \
    lz4 zstd \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

RUN locale-gen en_US.UTF-8
ENV LANG=en_US.UTF-8
ENV LC_ALL=en_US.UTF-8

ARG HOST_UID=1000
ARG HOST_GID=1000
RUN groupadd -g ${HOST_GID} yocto 2>/dev/null || true \
    && useradd -u ${HOST_UID} -g ${HOST_GID} -m -s /bin/bash yocto

USER yocto
WORKDIR /work

CMD ["/bin/bash"]
