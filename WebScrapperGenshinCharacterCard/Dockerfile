FROM ubuntu:latest
LABEL Description="WSGCC build env"

RUN apt-get update && apt-get upgrade && \
    apt-get install -y build-essential git cmake curl zip tar unzip && \
    git clone https://github.com/Microsoft/vcpkg.git && cd vcpkg && \
    ./bootstrap-vcpkg.sh && \
    ./vcpkg install cpr gumbo curl && cd ~