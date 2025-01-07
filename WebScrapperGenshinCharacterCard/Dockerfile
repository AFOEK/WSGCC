FROM alpine:latest
LABEL Description="WSGCC build env"

RUN apk update && apk upgrade && \
    apk add build-base ninja git bash cmake make curl zip tar unzip linux-headers perl pkgconfig && \
    cd ~ && \
    git clone https://github.com/Microsoft/vcpkg.git && cd vcpkg && \
    export VCPKG_FORCE_SYSTEM_BINARIES=1 && \
    ./bootstrap-vcpkg.sh && \
    ./vcpkg install cpr gumbo curl indicators argparse termcolor && \
    cd ~ && \
    git clone https://github.com/AFOEK/WSGCC.git && cd WSGCC && mkdir build && \
    cd WebScrapperGenshinCharacterCard && \
    export VCPKG_ROOT=~/vcpkg && \
    cmake -B ../build -S . -DCMAKE_TOOLCHAIN_FILE=~/WSGCC/WebScrapperGenshinCharacterCard/CMakeToolkits/linux_x64.cmake && \
    cmake --build ../build